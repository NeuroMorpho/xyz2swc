import subprocess
import os
import shutil
import numbers
import pandas as pd
import numpy as np
from oct2py import octave
from .utils import nml2swc, hoc2swc, mergetraces, eswc2swc, mesh2swc, vtk2swc
from .standard import chksingle, swccorrect
from .swcco import extract_contours


# -----------------------------------------------------------------------------------
def check_if_empty(outputfile):
    try:
        swc_matrix = pd.read_table(outputfile, comment="#", sep=r"\s+", header=None)
        return "SUCCESS"
    except Exception:
        return "FAIL"


# -----------------------------------------------------------------------------------
def single(inputfile, outputfile=None, logdir="./logs", mesh_configfile=None, verbose=True):
    """
    Method for converting a single reconstruction file to SWC format.

    Required arguments:
        inputfile:          [string]    Path of the reconstruction format to convert.

    Optional keyword arguments:
        outputfile:         [string]    Path of converted SWC file. If unspecified will save converted SWC file in the same folder as [inputfile]
        logdir:             [string]    Path of logfiles. Logs will be deleted if 'verbose' option is False.
        mesh_configfile     [string]    Path to mesh2swc_config.txt, for passing optional parameters when skeletonizing mesh files
        verbose:            [bool]      Toggle to display/suppress console output.

    Returns:
        conversion_status:  [string]    'SUCCESS' if conversion had no errors; 'FAIL' if they were errors; 'IGNORE' if format currently not supported; 'STANDARDIZED' if input is SWC
        correction_list_df: [pandas.df] If the input file was a SWC a list of the attempted corrections is generated; Else returns a 'None' value.
    """

    # create (overwrite) log directory
    if os.path.exists(logdir):
        shutil.rmtree(logdir)
    os.mkdir(logdir)

    # filename = os.path.basename(inputfile)
    ending = os.path.splitext(inputfile)[1]

    if ending.isupper():
        new_filename = os.path.splitext(inputfile)[0] + ending.lower()
        cmd_rename = "mv " + inputfile + " " + new_filename
        res_rename = subprocess.run(cmd_rename, shell=True)
        inputfile = new_filename
        ending = os.path.splitext(inputfile)[1]

    if outputfile is None:
        if ending == ".swc":
            outputfile = os.path.splitext(inputfile)[0] + "_standardized.swc"
        else:
            outputfile = os.path.splitext(inputfile)[0] + ".swc"

    # infolder = os.path.dirname(inputfile)
    # retval = os.getcwd()
    # os.chdir(infolder)
    # command = "prename -l -i 'y/A-Z/a-z/' " + filename
    # res_rename = subprocess.run([command], shell=True)
    # os.chdir(retval)

    if verbose:
        print("Converting %s to %s" % (inputfile, outputfile))

    correction_list_df = None

    if ending == ".dat":
        # first convert dat to asc
        ascfilename = os.path.join(logdir, (os.path.splitext(os.path.basename(inputfile))[0] + ".asc"))
        exec_name = "nmoc"
        res = subprocess.run([exec_name, inputfile, ascfilename, "NeurolucidaASC"])

        # convert asc to swc
        conversion_status = "FAIL"  # initialize
        if res.returncode == 0:
            conversion_status = extract_contours.extract_contours(ascfilename, outputfile, logdir)
            # delete the temporary .asc file from logs folder
            os.remove(ascfilename)

        if conversion_status == "FAIL":
            exec_name = "node /app/modules/hbp/node_modules/morphology_io/app/hbp2swc.js"
            command = exec_name + " --infile " + inputfile + " --outfile " + outputfile
            res = subprocess.run([command], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            conversion_status = check_if_empty(outputfile) if (res.returncode == 0) else "FAIL"

    elif ending == ".asc":
        conversion_status = extract_contours.extract_contours(inputfile, outputfile, logdir)

        if conversion_status == "FAIL":
            exec_name = "node /app/modules/hbp/node_modules/morphology_io/app/hbp2swc.js"
            command = exec_name + " --infile " + inputfile + " --outfile " + outputfile
            res = subprocess.run([command], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            conversion_status = check_if_empty(outputfile) if (res.returncode == 0) else "FAIL"

    elif ending == ".nrx":
        exec_name = "node /app/modules/hbp/node_modules/morphology_io/app/hbp2swc.js"
        command = exec_name + " --infile " + inputfile + " --outfile " + outputfile
        res = subprocess.run([command], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        conversion_status = check_if_empty(outputfile) if (res.returncode == 0) else "FAIL"

        if conversion_status == "FAIL":
            exec_name = "nmoc"
            res = subprocess.run([exec_name, inputfile, outputfile, "swc"])
            conversion_status = "SUCCESS" if (res.returncode == 0) else "FAIL"

    elif ending in [".ims", ".nts", ".ntr", ".nst", ".anat", ".bur", ".p"]:  # other formats supported by neuronland
        exec_name = "nmoc"
        res = subprocess.run([exec_name, inputfile, outputfile, "swc"])
        conversion_status = "SUCCESS" if (res.returncode == 0) else "FAIL"

    elif ending == ".eswc":
        conversion_status = eswc2swc.eswc2swc(inputfile, outputfile)

    elif ending == ".hoc":
        conversion_status = hoc2swc.hoc2swc(inputfile, outputfile)

    elif ending == ".ndf":
        octave.addpath("/app/modules/ndf")
        try:
            octave.ndf2swc(inputfile, outputfile)
            conversion_status = "SUCCESS"
        except Exception:
            conversion_status = "FAIL"

    elif ending in [".nml", ".nmx"]:
        conversion_status = nml2swc.nml2swc(inputfile, outputfile)

    elif ending == ".xml":
        # try Neuronland
        exec_name = "nmoc"
        res = subprocess.run([exec_name, inputfile, outputfile, "swc"])
        conversion_status = check_if_empty(outputfile) if (res.returncode == 0) else "FAIL"

        # if that fails try using HBP
        if conversion_status == "FAIL":
            exec_name = "node /app/modules/hbp/node_modules/morphology_io/app/hbp2swc.js"
            command = exec_name + " --infile " + inputfile + " --outfile " + outputfile
            res = subprocess.run([command], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            conversion_status = check_if_empty(outputfile) if (res.returncode == 0) else "FAIL"

        # if that fails try converting to nml
        if conversion_status == "FAIL":
            nmlfilename = os.path.join(logdir, (os.path.splitext(os.path.basename(inputfile))[0] + ".nml"))
            shutil.copyfile(inputfile, nmlfilename)
            conversion_status = nml2swc.nml2swc(nmlfilename, outputfile)

    elif ending in [".mtr", ".mat"]:
        octave.addpath("/app/modules/mtr")
        #try:
        octave.mtr2swc(inputfile, outputfile)
        conversion_status = "SUCCESS"
        #except Exception
        #    conversion_status = "FAIL"

    elif ending == ".traces":
        exec_name = "/app/modules/snt/Fiji.app/ImageJ-linux64 --ij2 --headless --console --run "
        conversionscript = "/app/modules/snt/snt2swc.py "
        inputarg = "'traces_file=\"" + inputfile + "\"' > "
        logfile_folder = "/app/modules/snt/logfiles/"
        logfiles = logfile_folder + "log.txt 2>&1"
        command = exec_name + conversionscript + inputarg + logfiles
        res = subprocess.run([command], shell=True)
        conversion_status = mergetraces.mergetraces(inputfile, outputfile, logfile_folder)

    elif ending == ".am":
        prog_name = "Rscript --vanilla "
        conversionscript = "/app/modules/am/am2swc.r "
        command = prog_name + conversionscript + inputfile + " " + outputfile + " > /app/modules/am/logfiles/log.txt 2>&1"
        res = subprocess.run([command], shell=True)
        conversion_status = "SUCCESS" if (res.returncode == 0) else "FAIL"

    elif ending in [".stl", ".obj", "ply"]:
        conversion_status = mesh2swc.mesh2swc(inputfile, outputfile, mesh_configfile)

    elif ending in [".vtk"]:
        conversion_status = vtk2swc.vtk2swc(inputfile,
                                            outputfile)

    # -- if input file is already swc
    # -- new standardized output swc file needs to be created (also need to return correction_list to user)
    elif ending == ".swc":
        check_list_df, check_status, conversion_flags = chksingle(inputfile, outfolder=None, verbose=False, write_csv=False)
        correction_list_df = swccorrect(inputfile, check_list_df, conversion_flags, filename_corrected=outputfile, spider_leg_type=0, verbose=False, write_csv=False)
        conversion_status = "FAIL" if (isinstance(correction_list_df, numbers.Number)) else "STANDARDIZED"

    else:
        conversion_status = "IGNORE"

    # if "res" in locals():
    # if isinstance(res, subprocess.CompletedProcess):

    # -- if input file is NOT swc
    # -- internally generated swc need to be cleaned and overwritten
    # -- run standardization in quiet mode, i.e., correction list is produced for debugging but should not be returned to user and is set to 'None'
    if conversion_status == "SUCCESS":
        # pre clean-up
        swc_matrix = pd.read_table(outputfile, sep=r"\s+", header=None, comment="#")
        np.savetxt(outputfile, swc_matrix, fmt="%u %u %f %f %f %f %d")
        # check and correct
        check_list_df, check_status, conversion_flags = chksingle(outputfile, outfolder=None, verbose=False, write_csv=False)
        correction_list_df = swccorrect(outputfile, check_list_df, conversion_flags, filename_corrected=outputfile, spider_leg_type=0, verbose=False, write_csv=False)

    # if verbose is False:
    #     shutil.rmtree(logdir)

    return conversion_status, correction_list_df


# -----------------------------------------------------------------------------------
def folder(infolder=None, outfolder=None):
    """
    Method for converting all files in [infolder] and saving result in [outfolder].
    """
    # failedfiles = []
    # filecount=0
    if (infolder is None) and (outfolder is None):
        infolder = "../input/to_convert/"
        outfolder = "../output/converted/"
        csvfolder = "../output/swc_standardized_check"

    if (infolder is not None) and (outfolder is None):
        outfolder = os.path.join(infolder, "xyz2swc_output")
        csvfolder = os.path.join(infolder, "xyz2swc_output", "swc_standardized_check")

    if not os.path.isdir(outfolder):
        os.mkdir(outfolder)

    if not os.path.isdir(csvfolder):
        os.mkdir(csvfolder)

    converted_checklist_df = pd.DataFrame(columns=["FileName", "CoversionStatus"])
    print("\n##################################################################\n")
    for filename in os.listdir(infolder):
        inputfile = os.path.join(infolder, filename)
        outputfile_name = os.path.splitext(filename)[0] + ".swc"
        outputfile = os.path.join(outfolder, outputfile_name)
        csv_outfile = os.path.splitext(filename)[0] + ".csv"

        mesh_configfile = os.path.join(infolder, "mesh2swc_config.txt")
        if not os.path.isfile(mesh_configfile):
            mesh_configfile = None

        conversion_status, correction_list_df = single(inputfile, outputfile, mesh_configfile=mesh_configfile)
        converted_checklist_df.loc[len(converted_checklist_df)] = {"FileName": filename, "CoversionStatus": conversion_status}
        if not (correction_list_df is None) and isinstance(correction_list_df, pd.DataFrame):
            correction_list_df.to_csv(os.path.join(csvfolder, csv_outfile), index=None, sep=",", mode="w")

    print("\n------------------------------------------------------------------\n\n")
    print(converted_checklist_df)
    # -- write checklist to output folder
    converted_checklist_df.to_csv(os.path.join(outfolder, "converted_checklist.csv"), index=None, sep=",", mode="w")
    print("\n******************************************************************")
    print("** Done! Program terminated successfully **\n")


# -----------------------------------------------------------------------------------
if __name__ == "__main__":
    folder()
