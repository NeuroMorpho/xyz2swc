import skeletor as sk
import trimesh as tm
import pandas as pd
import numpy as np
import ast
import re
import os

# import pdb


def extract_params(params_file):
    with open(params_file, "rt") as fileswc:
        waveparams = []
        contractparams = []
        quoted = re.compile("(?<=')[^']+(?=')")

        try:
            Lines = fileswc.readlines()

            waveparams.append(ast.literal_eval(Lines[1].strip().split("=")[1]))
            waveparams.append(ast.literal_eval(Lines[2].strip().split("=")[1]))
            waveparams.append(ast.literal_eval(Lines[3].strip().split("=")[1]))
            waveparams.append(quoted.findall(Lines[4].strip().split("=")[1])[0])
            waveparams.append(ast.literal_eval(Lines[5].strip().split("=")[1]))

            mesh_contract = ast.literal_eval(Lines[8].strip().split("=")[1])

            contractparams.append(ast.literal_eval(Lines[11].strip().split("=")[1]))
            contractparams.append(ast.literal_eval(Lines[12].strip().split("=")[1]))
            contractparams.append(ast.literal_eval(Lines[13].strip().split("=")[1]))
            contractparams.append(ast.literal_eval(Lines[14].strip().split("=")[1]))
            contractparams.append(ast.literal_eval(Lines[15].strip().split("=")[1]))
            contractparams.append(ast.literal_eval(Lines[16].strip().split("=")[1]))
            contractparams.append(quoted.findall(Lines[17].strip().split("=")[1])[0])
            contractparams.append(quoted.findall(Lines[18].strip().split("=")[1])[0])
            contractparams.append(ast.literal_eval(Lines[19].strip().split("=")[1]))
            contractparams.append(ast.literal_eval(Lines[20].strip().split("=")[1]))

            set_default = False

        except UnicodeDecodeError:
            set_default = True
            mesh_contract = True

        # print(waveparams)
        # print(mesh_contract)
        # print(contractparams)

    return set_default, waveparams, mesh_contract, contractparams


def mesh2swc(inputfile, outputfile, params_file=None):
    if params_file is None:
        set_default = True
    else:
        set_default, waveparams, mesh_contract, contractparams = extract_params(params_file)

    if set_default:
        waveparams = [1, None, 1, "mean", False]
        mesh_contract = True
        contractparams = [0.1, 100, 40, 1e-07, 2, 1, "auto", "cotangent", False, True]

    # Load mesh file
    mesh = tm.load_mesh(inputfile)
    # Run some general clean-up (see docstring for details)
    mesh = sk.pre.fix_mesh(mesh, remove_disconnected=5, inplace=False)

    if mesh_contract:
        # Contract mesh to percentage of original volume
        mesh = sk.pre.contract(mesh, *contractparams)

    # Skeletonize
    skel = sk.skeletonize.by_wavefront(mesh, *waveparams)

    # cleanup
    skel = sk.post.clean_up(skel)

    # write as swc
    skel.save_swc(outputfile)

    # set TypeIDs
    swc_matrix = pd.read_table(outputfile, sep=r"\s+", header=None, comment="#")
    swc_matrix.columns = ["Index", "TypeID", "X", "Y", "Z", "Radius", "ParentIndex"]
    swc_matrix.loc[:, "TypeID"] = 0
    np.savetxt(outputfile, swc_matrix, fmt="%u %u %f %f %f %f %d")

    if os.stat(outputfile).st_size == 0:
        return "FAIL"
    else:
        return "SUCCESS"
