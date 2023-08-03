import os
import pandas as pd
import numpy as np
from collections import namedtuple
from .swcch import soma_check, general_check, parentindex_check, datatype_check, typeid_check, index_check, spiderleg_check
from .swcco import (
    contour_convert,
    swc_sort,
    set_unspecified_typeID,
    set_standard_somaID,
    set_default_radius,
    remove_bifuc_neuronstudio,
    set_invalidparent_to_root,
    cut_spiderlegs,
    correct_invalid_xyz,
    break_loop,
)


# ------------------------------------------------------
# -- Printing Verbose Output
def PrintErrorChkList(filename, check_list_df):
    print("\n")
    print(filename)
    print("-----------------------------------------")
    print(check_list_df)
    print("\n")


# ------------------------------------------------------
# -- check SWC file to see if it is standardized
def chksingle(filename, outfolder=None, verbose=True, write_csv=True):
    """
    Method for implementing the 'check phase' during standardization.

    Required arguments:
        filename:          [string]     Path of the SWC file to check.

    Returns:
        check_list_df:     [pandas.df]  Log of the formatting errors.
        conversion_flags:  [tuple]      Collection of flags that are used by swccorrect(), determining what correction actions need to be performed.
    """

    do_NStudio = True
    do_swcPlus = True

    con_flags_tuple = namedtuple("con_flags_tuple", ["stop", "contour", "root", "invalidparent", "reset", "sort", "radius", "typeID", "somaID", "swcPlusCustom", "bifucNS", "spiderleg", "xyz"])
    stop_conversion_flag = False
    convert_contour_flag = False
    set_root_flag = False
    invalidparent_to_root_flag = False
    reset_index_flag = False
    do_sort_flag = False
    set_defaultradius_flag = False
    unspecified_typeid_flag = False
    cut_spiderlegs_flag = False
    nonstandard_somaID = None
    swcPlus_remove = None
    remove_bifucNS_flag = False
    invalid_XYZ_flag = 0

    conversion_flags = con_flags_tuple(
        stop_conversion_flag,
        convert_contour_flag,
        set_root_flag,
        invalidparent_to_root_flag,
        reset_index_flag,
        do_sort_flag,
        set_defaultradius_flag,
        unspecified_typeid_flag,
        nonstandard_somaID,
        swcPlus_remove,
        remove_bifucNS_flag,
        cut_spiderlegs_flag,
        invalid_XYZ_flag,
    )

    check_attribute_namelist = [
        "Missing Field Columns",
        "No. of tree samples",
        "No. of soma samples",
        "Root ParentIndex",
        "Invalid ParentIndex",
        "NonStandard TypeID",
        "ITP int",
        "XYZ double",
        "Radius positive double",
        "Index sequential",
        "Sorted Index Order",
        "Contour",
        "Abnormal Compartments",
    ]

    check_list_df = pd.DataFrame(
        {"Check": check_attribute_namelist, "Value": [" "] * len(check_attribute_namelist), "Status": [" "] * len(check_attribute_namelist), "ErrorMsg": [" "] * len(check_attribute_namelist)}
    )

    if outfolder is None:
        outfolder = os.path.dirname(filename)

    file_extension = os.path.splitext(filename)[1]
    if file_extension.lower() != ".swc":
        check_status = "FAIL"
        return [check_list_df, check_status, conversion_flags]

    # linecount = 0
    # invalid_sample = 0
    # invalid_sample_lineno = []
    commentline_numbers = []

    with open(filename, "rt") as fileswc:
        try:
            Lines = fileswc.readlines()
        except UnicodeDecodeError:
            check_list_df.loc[0].ErrorMsg = "UnicodeDecode ERROR: Unable to read and parse SWC File [UTF-8]."
            stop_conversion_flag = True
            check_status = "FAIL"
            conversion_flags = con_flags_tuple(
                stop_conversion_flag,
                convert_contour_flag,
                set_root_flag,
                invalidparent_to_root_flag,
                reset_index_flag,
                do_sort_flag,
                set_defaultradius_flag,
                unspecified_typeid_flag,
                nonstandard_somaID,
                swcPlus_remove,
                remove_bifucNS_flag,
                cut_spiderlegs_flag,
                invalid_XYZ_flag,
            )
            if verbose:
                PrintErrorChkList(filename, check_list_df)
            return [check_list_df, check_status, conversion_flags]
        for i in range(0, len(Lines)):
            line = Lines[i]
            line = line.strip()
            if line.startswith("#") or line.startswith("*"):
                commentline_numbers.append(i)
                # Lines.pop(linecount) #remove comments can also be done using dropwhile
            # else:
            #     # num_words_per_line = len(line.split())
            #     # if(num_words_per_line != 7):
            #     #     invalid_sample += 1
            #     #     invalid_sample_lineno.append(linecount+1)
            #     linecount += 1
            #     print(line)
            #     print("Linecount = %d" %(linecount))
            #     breakpoint()
    # for i in sorted(commentline_numbers, reverse=True):
    #     Lines.pop(i)

    try:
        swc_matrix = pd.read_table(filename, sep=r"\s+", skiprows=commentline_numbers, header=None)
    except pd.errors.EmptyDataError:
        check_list_df.loc[0].ErrorMsg = "EmptyData ERROR: Unable to read and parse SWC File."
        if verbose:
            PrintErrorChkList(filename, check_list_df)
        check_status = "FAIL"
        return [check_list_df, check_status, conversion_flags]
    except Exception:
        check_list_df.loc[0].ErrorMsg = "ERROR: Unable to read and parse SWC File."
        if verbose:
            PrintErrorChkList(filename, check_list_df)
        check_status = "FAIL"
        return [check_list_df, check_status, conversion_flags]
    # swc_matrix = np.loadtxt(filename)
    # swc_matrix = swc_matrix.replace({np.nan:None})

    if len(swc_matrix.columns) != 7:
        try:
            swc_matrix = swc_matrix.iloc[:, :7]
        except Exception:
            check_list_df.loc[0].Value = True
            check_list_df.loc[0].Status = "FAIL"
            check_list_df.loc[0].ErrorMsg = "ERROR: SWC File requires a points matrix with 7 columns; Only " + str(len(swc_matrix.columns)) + " columns detected"
            stop_check_flag = True
            stop_conversion_flag = True
            check_status = "FAIL"
            conversion_flags = con_flags_tuple(
                stop_conversion_flag,
                convert_contour_flag,
                set_root_flag,
                invalidparent_to_root_flag,
                reset_index_flag,
                do_sort_flag,
                set_defaultradius_flag,
                unspecified_typeid_flag,
                nonstandard_somaID,
                swcPlus_remove,
                remove_bifucNS_flag,
                cut_spiderlegs_flag,
                invalid_XYZ_flag,
            )
            if verbose:
                PrintErrorChkList(filename, check_list_df)
            return [check_list_df, check_status, conversion_flags]

    swc_matrix.columns = ["Index", "TypeID", "X", "Y", "Z", "Radius", "ParentIndex"]

    soma_rows = swc_matrix.loc[swc_matrix["TypeID"] == 1,]
    tree_rows = swc_matrix.loc[swc_matrix["TypeID"] != 1,]
    # print("\nNumber of soma samples: %d" %(len(soma_rows.index)))
    # print("\nNumber of tree samples: %d" %(len(tree_rows.index)))

    check_list_df, stop_check_flag = general_check.general_check(swc_matrix, soma_rows, tree_rows, check_list_df)

    if stop_check_flag:
        check_status = "FAIL"
        return [check_list_df, check_status, conversion_flags]

    check_list_df, stop_check_flag, set_root_flag, invalidparent_to_root_flag = parentindex_check.parentindex_check(swc_matrix, soma_rows, tree_rows, check_list_df)

    check_list_df, stop_check_flag, unspecified_typeid_flag, nonstandard_somaID, swcPlus_remove, remove_bifucNS_flag = typeid_check.typeid_check(
        swc_matrix, soma_rows, tree_rows, check_list_df, Lines, commentline_numbers, do_NStudio, do_swcPlus
    )

    check_list_df, stop_check_flag, set_defaultradius_flag, invalid_XYZ_flag = datatype_check.datatype_check(swc_matrix, soma_rows, tree_rows, check_list_df)

    check_list_df, stop_check_flag, reset_index_flag, do_sort_flag = index_check.index_check(swc_matrix, soma_rows, tree_rows, check_list_df)

    check_list_df, stop_check_flag, soma_centerxyz, soma_radius, convert_contour_flag = soma_check.soma_check(swc_matrix, soma_rows, tree_rows, check_list_df, verbose=False)

    check_list_df, cut_spiderlegs_flag = spiderleg_check.spiderleg_check(swc_matrix, soma_rows, tree_rows, check_list_df)

    if verbose:
        PrintErrorChkList(filename, check_list_df)

    if all(check_list_df.Status == "PASS"):
        check_status = "PASS"
    else:
        check_status = "FAIL"

    if write_csv:
        csv_outfile = os.path.basename(filename).split(".")[0] + ".csv"
        check_list_df.to_csv(os.path.join(outfolder, csv_outfile), index=None, sep=",", mode="w")

        # np.savetxt('./output/new_file.swc', updated_swc_matrix.values, fmt="%d %d %f %f %f %f %d")
        # updated_swc_matrix.to_csv('./output/new_file_v2.swc',
        #                           header=None, #      '# '+' '.join(list(updated_swc_matrix.columns)),
        #                           index=None,
        #                           sep=' ',
        #                           mode='w')

    conversion_flags = con_flags_tuple(
        stop_conversion_flag,
        convert_contour_flag,
        set_root_flag,
        invalidparent_to_root_flag,
        reset_index_flag,
        do_sort_flag,
        set_defaultradius_flag,
        unspecified_typeid_flag,
        nonstandard_somaID,
        swcPlus_remove,
        remove_bifucNS_flag,
        cut_spiderlegs_flag,
        invalid_XYZ_flag,
    )

    return [check_list_df, check_status, conversion_flags]


# -----------------------------------------------------------------------------------
def chkfolder(infolder=None, outfolder=None, verbose=True):
    """
    Standardized SWC check for all files in [infolder] and saving check-result in [outfolder].
    """

    if (infolder is None) and (outfolder is None):
        infolder = "../input/swc_check_only/"
        outfolder = "../output/swc_standardized_check/"

    check_status_df = pd.DataFrame(columns=["FileName", "CheckStatus"])
    for f in os.listdir(infolder):
        filename = os.path.join(infolder, f)
        # checking if it is a file
        if os.path.isfile(filename):
            check_list_df, check_status, conversion_flags = chksingle(filename, outfolder)
            check_status_df = check_status_df.append({"FileName": f, "CheckStatus": check_status}, ignore_index=True)

    if verbose:
        print("\n\n")
        print("-----------------------------------------")
        print(check_status_df)

    check_status_df.to_csv(os.path.join(outfolder, "check_status_log.csv"), index=None, sep=",", mode="w")
    # df.to_csv("log.csv", index = False)


# ------------------------------------------------------
# -- convert to standardized SWC
def swccorrect(filename, check_list_df, conversion_flags, filename_corrected=None, spider_leg_type=0, verbose=True, write_csv=True):
    """
    Method for implementing the 'correction phase' during standardization.
    Cannot be run independetly without first running chksingle().

    Required arguments:
        filename:          [string]     Path of the SWC file to correct.
        check_list_df:     [pandas.df]  Log of formatting errors detected by chksingle().
        conversion_flags:  [tuple]      Collection of flags that are set by chksingle(), determining what correction actions need to be performed.

    Returns:
        correction_list_df: [pandas.df] Log of formatting errors + corrective actions performed.
    """

    correction_list_df = check_list_df.assign(Correction=" " * len(check_list_df))

    if conversion_flags.stop:
        print("ERROR: Conversion Flags terminated correction operation!")
        correction_list_df.loc[0].Correction = "ERROR: Conversion Flags terminated correction operation!"
        return correction_list_df

    file_extension = os.path.splitext(filename)[1]
    if file_extension.lower() != ".swc":
        print("ERROR: File extension not detected as SWC!")
        correction_list_df.loc[0].Correction = "ERROR: File extension not detected as SWC!"
        return correction_list_df

    if filename_corrected is None:
        outfolder = os.path.dirname(filename)
        filename_corrected = os.path.basename(filename).split(".")[0] + "_standardized.swc"
        filename_corrected = os.path.join(outfolder, filename_corrected)
    else:
        outfolder = os.path.dirname(filename_corrected)

    ftr_msg = list()
    commentline_numbers = []

    with open(filename, "rt") as fileswc:
        Lines = fileswc.readlines()
        for i in range(0, len(Lines)):
            line = Lines[i]
            line = line.strip()
            if line.startswith("#") or line.startswith("*"):
                commentline_numbers.append(i)
        # extract original header
        orig_header = list()
        for i in range(0, len(Lines)):
            line = Lines[i]
            if line.startswith("#") or line == "\n":
                orig_header.append(line)
                continue
            else:
                break
        orig_header = "".join(orig_header).strip()
        # extract original footer
        orig_footer = list()
        for i in reversed(range(0, len(Lines))):
            line = Lines[i]
            if line.startswith("#") or line == "\n":
                orig_footer.append(line)
                continue
            else:
                break
        orig_footer.reverse()
        orig_footer = "".join(orig_footer).strip()

    try:
        swc_matrix = pd.read_table(filename, sep=r"\s+", skiprows=commentline_numbers, header=None)
    except pd.errors.EmptyDataError:
        print("ERROR: Empty File!")
        correction_list_df.loc[0].Correction = "ERROR: Empty File!"
        return correction_list_df
    except Exception:
        print("ERROR: Unable to read file contents!")
        correction_list_df.loc[0].Correction = "ERROR: Unable to read file contents!"
        return correction_list_df
    # swc_matrix = np.loadtxt(filename)

    if len(swc_matrix.columns) != 7:
        try:
            swc_matrix = swc_matrix.iloc[:, :7]
        except Exception:
            print("ERROR: Needs 7 data-field columns to proceed!")
            correction_list_df.loc[0].Correction = "ERROR: Needs 7 data-field columns to proceed!"
            return correction_list_df

    # swc_matrix = swc_matrix.replace({np.nan:None})
    swc_matrix.columns = ["Index", "TypeID", "X", "Y", "Z", "Radius", "ParentIndex"]

    updated_swc_matrix = swc_matrix

    if conversion_flags.radius:
        updated_swc_matrix = set_default_radius.set_default_radius(updated_swc_matrix)
        correction_list_df.loc[8, "Correction"] = "Default Radius set to 0.500."
        ftr_msg.append("NOTE: Invalid (negative and/or NaN) Radius values set to a default value of 0.500")

    if not (conversion_flags.somaID is None) or not (conversion_flags.swcPlusCustom is None):
        updated_swc_matrix = set_standard_somaID.set_standard_somaID(updated_swc_matrix, conversion_flags.somaID, conversion_flags.swcPlusCustom)
        updated_swc_matrix = swc_sort.sort_matrix(updated_swc_matrix.values)
        updated_swc_matrix = pd.DataFrame(updated_swc_matrix, columns=["Index", "TypeID", "X", "Y", "Z", "Radius", "ParentIndex"])
        soma_rows = updated_swc_matrix.loc[updated_swc_matrix["TypeID"] == 1,]
        tree_rows = updated_swc_matrix.loc[updated_swc_matrix["TypeID"] != 1,]
        if len(soma_rows.index) >= 3:
            updated_swc_matrix = contour_convert.contour_convert(updated_swc_matrix, tree_rows, soma_rows)
        correction_list_df.loc[5, "Correction"] = "Set soma sample Type=1."

    if conversion_flags.typeID:
        updated_swc_matrix = set_unspecified_typeID.set_unspecified_typeID(updated_swc_matrix)
        correction_list_df.loc[5, "Correction"] = "Nonstandard Type converted into Type=0 indicating 'Undefined'."
        # temp_string = correction_list_df.loc[5, "Correction"]
        # if temp_string.isspace():
        #     correction_list_df.loc[5, "Correction"] = "TypeID=0 are converted into TypeID=6 indicating Unspecified Neurite."
        # else:
        #     correction_list_df.loc[5, "Correction"] = temp_string + " TypeID=0 are converted into TypeID=6 indicating Unspecified Neurite."

    soma_rows = updated_swc_matrix.loc[updated_swc_matrix["TypeID"] == 1,]
    tree_rows = updated_swc_matrix.loc[updated_swc_matrix["TypeID"] != 1,]

    if conversion_flags.contour:
        updated_swc_matrix = contour_convert.contour_convert(updated_swc_matrix, tree_rows, soma_rows)
        correction_list_df.loc[11, "Correction"] = "Converted single contour to single point soma."

    soma_rows = updated_swc_matrix.loc[updated_swc_matrix["TypeID"] == 1,]
    tree_rows = updated_swc_matrix.loc[updated_swc_matrix["TypeID"] != 1,]

    if conversion_flags.sort != 0 or conversion_flags.root is True:
        updated_swc_matrix = break_loop.break_loop(updated_swc_matrix)
        updated_swc_matrix = swc_sort.sort_matrix(updated_swc_matrix.values)
        updated_swc_matrix = pd.DataFrame(updated_swc_matrix, columns=["Index", "TypeID", "X", "Y", "Z", "Radius", "ParentIndex"])
        if conversion_flags.root:
            correction_list_df.loc[3, "Correction"] = "Reindexed and sorted samples so that Root ParentIndex is -1."
        if conversion_flags.reset:
            correction_list_df.loc[9, "Correction"] = "Reindexed and sorted samples to be sequential."
        if conversion_flags.sort:
            correction_list_df.loc[10, "Correction"] = "Reindexed and sorted samples."

    if conversion_flags.invalidparent is True:
        updated_swc_matrix, nochange_flag = set_invalidparent_to_root.set_invalidparent_to_root(updated_swc_matrix)
        if nochange_flag is True:
            correction_list_df.loc[4, "Correction"] = "Unable to correct. Check file integrity!"
        else:
            correction_list_df.loc[4, "Correction"] = "Set invalid Parent index to -1."

    if conversion_flags.bifucNS:
        updated_swc_matrix = remove_bifuc_neuronstudio.remove_bifuc_neuronstudio(updated_swc_matrix)
        temp_string = correction_list_df.loc[5, "Correction"]
        if temp_string.isspace():
            correction_list_df.loc[5, "Correction"] = "Set bifurcation TypeID to that of branch, and terminal Type to that of parent."
        else:
            correction_list_df.loc[5, "Correction"] = temp_string + " Set bifurcation TypeID to that of branch, and terminal Type to that of parent."

    if (conversion_flags.spiderleg is True) and (spider_leg_type != 0):
        updated_swc_matrix = cut_spiderlegs.cut_spiderlegs(updated_swc_matrix, tree_rows, soma_rows, verbose=True)
        correction_list_df.loc[12, "Correction"] = "Cut spider legs originating from soma."

    if conversion_flags.xyz > 0:
        updated_swc_matrix, tofloat_success = correct_invalid_xyz.correct_invalid_xyz(updated_swc_matrix, conversion_flags.xyz)
        if conversion_flags.xyz == 1 and tofloat_success:
            correction_list_df.loc[7, "Correction"] = "Coerced into type float."
        if conversion_flags.xyz == 1 and (not tofloat_success):
            correction_list_df.loc[7, "Correction"] = "Unable to coerce into type float. Invalid coordinate(s) set to zero."
            ftr_msg.append("NOTE: Invalid XYZ coordinate(s) set to zero.")
        if conversion_flags.xyz == 3 and tofloat_success:
            correction_list_df.loc[7, "Correction"] = "Coerced into type float. Set NaN coordinate(s) to zero."
            ftr_msg.append("NOTE: Invalid XYZ coordinate(s) set to zero.")
        if conversion_flags.xyz == 3 and (not tofloat_success):
            correction_list_df.loc[7, "Correction"] = "Unable to coerce into type float. Invalid coordinate(s) set to zero."
            ftr_msg.append("NOTE: Invalid XYZ coordinate(s) set to zero.")
        if conversion_flags.xyz == 2:
            correction_list_df.loc[7, "Correction"] = "Set NaN coordinate(s) to zero. Check file integrity!"
            ftr_msg.append("NOTE: NaN XYZ coordinate(s) set to zero.")

    if verbose:
        PrintErrorChkList(filename, correction_list_df)

    hdr = (
        "****************************************************************************************"
        "\nStandardized SWC Format File\nCreated by 'xyz2swc' project"
        "\nFor the conversion service, please visit: http://cng-nmo-main.orc.gmu.edu/xyz2swc/ui/"
        "\nStandard specification available at: https://swc-specification.readthedocs.io/en/latest/"
        "\n****************************************************************************************"
        "\n\n\n"
    )
    if orig_header:
        hdr = hdr + orig_header + "\n****************************************************************************************\n\n"
    filename_header = "\n--------------------------------\n" "Index Type X Y Z Radius Parent" "\n--------------------------------"
    hdr = hdr + filename_header

    ftr = ""
    if orig_footer:
        ftr = "\n" + orig_footer
    if ftr_msg:
        ftr = ftr + "\n\n" + "\n".join(ftr_msg)

    # save corrected swc
    np.savetxt(filename_corrected, updated_swc_matrix, fmt="%u %u %f %f %f %f %d", header=hdr, footer=ftr)

    if write_csv:
        csv_outfile = os.path.basename(filename).split(".")[0] + "_correctionlist.csv"
        correction_list_df.to_csv(os.path.join(outfolder, csv_outfile), index=None, sep=",", mode="w")

    # breakpoint()
    return correction_list_df


# -----------------------------------------------------------------------------------
if __name__ == "__main__":
    chkfolder()
