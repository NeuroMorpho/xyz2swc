import numpy as np
import pandas as pd
import re
import math
import subprocess
import os
import shutil


# ------------------------------------------------------
# -- 3d distance
def distance3d(a, b):

    d = math.sqrt(math.pow(a[0] - b[0], 2) + math.pow(a[1] - b[1], 2) + math.pow(a[2] - b[2], 2) * 1.0)
    return d


# ------------------------------------------------------
# -- Contour center and radius
def somacenter(soma_xyz):

    mean_x = np.mean(soma_xyz['X'])
    mean_y = np.mean(soma_xyz['Y'])
    mean_z = np.mean(soma_xyz['Z'])
    center_xyz = [mean_x, mean_y, mean_z]

    distance_vector = [None] * len(soma_xyz)
    for i in range(0, len(soma_xyz)):
        point_cord = soma_xyz.loc[i].tolist()
        distance_vector[i] = distance3d(point_cord, center_xyz)

    soma_radius = np.mean(distance_vector)

    return center_xyz, soma_radius


# ------------------------------------------------------
# -- convert asc to swc
def asc2swc(inputfile, outputfile):

    exec_name = "nmoc"
    res = subprocess.run([exec_name, inputfile, outputfile, 'swc'])

    # if(res.returncode == 0):
    #     try:
    #         swc_matrix = pd.read_table(outputfile, comment='#', sep=r'\s+', header=None)
    #         return 'SUCCESS'
    #     except pd.errors.EmptyDataError:
    #         exec_name = "morph-tool convert file --sanitize --quiet "
    #         command = exec_name + " " + inputfile + " " + outputfile
    #         res = subprocess.run([command], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    # else:
    #     exec_name = "morph-tool convert file --sanitize --quiet "
    #     command = exec_name + " " + inputfile + " " + outputfile
    #     res = subprocess.run([command], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    if(res.returncode == 0):
        try:
            swc_matrix = pd.read_table(outputfile, comment='#', sep=r'\s+', header=None)
            return 'SUCCESS'
        except pd.errors.EmptyDataError:
            return 'FAIL'
    else:
        return 'FAIL'


# ------------------------------------------------------
# -- read swc matrix
def read_swc_matrix(filename):

    commentline_numbers = []
    with open(filename, "rt") as fileswc:
        Lines = fileswc.readlines()
        for i in range(0, len(Lines)):
            line = Lines[i]
            line = line.strip()
            if line.startswith("#"):
                commentline_numbers.append(i)

    try:
        swc_matrix = pd.read_table(filename, sep=r'\s+', skiprows=commentline_numbers, header=None)
        return swc_matrix
    except pd.errors.EmptyDataError:
        return None


# ------------------------------------------------------
# -- replace soma points created by NeuronLand or morph-tool
# -- by the single-point-soma extracted directly from asc
def replace_soma_points(swc_matrix, single_point_soma_df):

    pd.options.mode.chained_assignment = None
    soma_rows = swc_matrix.loc[swc_matrix['TypeID'] == 1, ]
    tree_rows = swc_matrix.loc[swc_matrix['TypeID'] != 1, ]

    # sample points that have soma as parent
    soma_root_indicies = tree_rows.index[tree_rows['ParentIndex'].isin(soma_rows['Index'])].tolist()
    # sample points that have root (-1) for parent
    root_indicies = tree_rows.index[tree_rows['ParentIndex'] == -1].tolist()

    tree_rows[:]['Index'] = tree_rows['Index'] - len(soma_rows.index) + len(single_point_soma_df)
    tree_rows[:]['ParentIndex'] = tree_rows['ParentIndex'] - len(soma_rows.index) + len(single_point_soma_df)

    if len(soma_root_indicies) > 0:
        if len(single_point_soma_df) == 0:
            tree_rows.loc[soma_root_indicies, 'ParentIndex'] = [-1] * len(soma_root_indicies)
        else:
            tree_rows.loc[soma_root_indicies, 'ParentIndex'] = [1] * len(soma_root_indicies)

    if len(root_indicies) > 0:
        tree_rows.loc[root_indicies, 'ParentIndex'] = [-1] * len(root_indicies)
    # tree_rows.at[tree_rows.first_valid_index(), 'ParentIndex'] = len(single_point_soma_df)

    updated_swc_matrix = single_point_soma_df.append(tree_rows, ignore_index=True)
    # df = df.sort_index().reset_index(drop=True) # insert at 0.5 location
    # df.loc[len(df)] = list

    return updated_swc_matrix


# ------------------------------------------------------
# -- extract contours from asc file
def extract_contours(inputfile, outputfile=None, logdir='./logs', verbose=False):

    if outputfile is None:
        outputfile = os.path.splitext(inputfile)[0] + '.swc'

    if os.path.exists(logdir) is False:
        os.mkdir(logdir)
    swcfilelog = os.path.join(logdir, (os.path.splitext(os.path.basename(inputfile))[0] + '.swc'))
    # filename_corrected = os.path.splitext(outputfile)[0] + '_standardized.swc'

    contour_count = 0
    linenum_last_contour = 0

    cont_df = pd.DataFrame(columns=["Type", "SomaDetected", "StartLine", "EndLine"])

    with open(inputfile, "rt", errors='ignore') as fileasc:
        Lines = fileasc.readlines()
        for i in range(0, len(Lines)):
            line = Lines[i].lower()

            if line.startswith("(\""):
                inside_block = True
                linenum_first_contour = i
                type_str = re.findall(r'"([^"]*)"', line)[0]
                if ("soma" in type_str) or ("cell" in type_str) or ("body" in type_str):
                    block_is_contour = True
                else:
                    block_is_contour = False

            elif "end of contour" in line:
                if inside_block:
                    contour_count += 1
                    linenum_last_contour = i
                    cont_df.loc[len(cont_df.index)] = [type_str, block_is_contour, linenum_first_contour, linenum_last_contour]
                    type_str = None
                    linenum_first_contour = 0
                    inside_block = False

            else:
                continue

    if verbose:
        print(cont_df)
        print("\nNumber of contours: %d" % (contour_count))

    single_point_soma_df = pd.DataFrame(columns=['Index', 'TypeID', 'X', 'Y', 'Z', 'Radius', 'ParentIndex'])

    with open(inputfile, "rt", errors='ignore') as fileasc:
        somapoint_index_number = 0
        Lines = fileasc.readlines()
        for contour_index in range(0, len(cont_df)):
            if cont_df.loc[contour_index, "SomaDetected"]:
                soma_xyz = pd.DataFrame(columns=["X", "Y", "Z"])
                somapoint_index_number += 1
                for i in range(cont_df.loc[contour_index, "StartLine"] + 1, cont_df.loc[contour_index, "EndLine"]):
                    line = Lines[i].strip()
                    if len(line) == 0:
                        continue
                    elif line.endswith(")"):
                        continue
                    else:
                        line = line.split()
                        if len(line[0]) == 1:
                            try:
                                soma_xyz.loc[len(soma_xyz.index)] = [float(line[1]), float(line[2]), float(line[3])]
                            except ValueError:
                                continue
                        elif line[0][0] == '(':
                            try:
                                soma_xyz.loc[len(soma_xyz.index)] = [float(line[0][1:len(line[0])]), float(line[1]), float(line[2])]
                            except ValueError:
                                continue
                        else:
                            continue
                if verbose:
                    print(soma_xyz)
                soma_centerxyz, soma_radius = somacenter(soma_xyz)
                if somapoint_index_number == 1:
                    single_point_soma = [somapoint_index_number, 1, soma_centerxyz[0], soma_centerxyz[1], soma_centerxyz[2], soma_radius, -1]
                else:
                    single_point_soma = [somapoint_index_number, 1, soma_centerxyz[0], soma_centerxyz[1], soma_centerxyz[2], soma_radius, somapoint_index_number - 1]
                single_point_soma_df.loc[len(single_point_soma_df.index)] = single_point_soma

    conversion_status = asc2swc(inputfile, swcfilelog)

    if conversion_status == 'SUCCESS':
        swc_matrix = read_swc_matrix(swcfilelog)
        swc_matrix.columns = ['Index', 'TypeID', 'X', 'Y', 'Z', 'Radius', 'ParentIndex']
    else:
        return 'FAIL'

    if verbose:
        print(single_point_soma_df)

    if not(swc_matrix is None):
        updated_swc_matrix = replace_soma_points(swc_matrix, single_point_soma_df)

    hdr = ("****************************************************************************************"
           "\nStandardized SWC Format File\nCreated by 'xyz2swc' project"
           "\nFor the conversion service, please visit: http://cng-nmo-main.orc.gmu.edu/xyz2swc/ui/"
           "\nStandard specification available at: https://swc-specification.readthedocs.io/en/latest/"
           "\n****************************************************************************************"
           "\n\n\nIndex TypeID X Y Z Radius ParentIndex"
           "\n----------------------------------------------")

    # save corrected swc
    try:
        np.savetxt(outputfile, updated_swc_matrix, fmt="%u %u %f %f %f %f %d", header=hdr)
        return 'SUCCESS'
    except Exception:
        return 'FAIL'
