import numpy as np
import math
import pdb
import re


# ------------------------------------------------------
# Neuronstudio check
def neuronstudio_check(swc_matrix):

    # for i in commentline_numbers:
    #     reurn_value = Lines[i].lower().find("neuronstudio")
    #     if(return_value!=-1):
    #         bifurc_check()

    bifuc_5_flag = False
    bifuc_6_flag = False
    bifuc_flag = False

    all_TypeIDs = swc_matrix['TypeID'].tolist()
    if (5 in all_TypeIDs and 6 in all_TypeIDs):
        index_of_typeid5 = swc_matrix[(swc_matrix.TypeID == 5)].Index.to_list()
        index_of_typeid6 = swc_matrix[(swc_matrix.TypeID == 6)].Index.to_list()[0]
        for ii in index_of_typeid5:
            if len(swc_matrix[swc_matrix.ParentIndex == ii]) >= 2:
                bifuc_5_flag = True
                break
        if len(swc_matrix[swc_matrix.ParentIndex == index_of_typeid6]) == 0:
            bifuc_6_flag = True

    if bifuc_5_flag and bifuc_6_flag:
        bifuc_flag = True

    return bifuc_flag


# ------------------------------------------------------
# swcPlus check
def swcPlus_check(swc_matrix, Lines, commentline_numbers):

    nonstandard_somaID = None
    swcPlus_remove = None
    is_swcPlus_flag = False

    for i in commentline_numbers:
        line = Lines[i]
        line = line.strip()
        return_value = line.lower().find("swcplus")
        if(return_value != -1):
            is_swcPlus_flag = True

        matched_string = re.findall('(?<= id=\').*\'', line)
        if len(matched_string) != 0:
            soma_matched_string = re.findall('(?<=somaContour id=\').*\'', line)
            if len(soma_matched_string) != 0:
                nonstandard_somaID = int(matched_string[0][:-1])
            else:
                temp_id = int(matched_string[0][:-1])
                if (temp_id < 1) or (temp_id > 7):
                    if swcPlus_remove is None:
                        swcPlus_remove = list([temp_id])
                    else:
                        swcPlus_remove.append(temp_id)

    return is_swcPlus_flag, nonstandard_somaID, swcPlus_remove


# ------------------------------------------------------
# TypeID check

def typeid_check(swc_matrix, soma_rows, tree_rows, check_list_df, Lines, commentline_numbers,
                 do_NStudio=True, do_swcPlus=True):

    nonstandard_somaID = None
    swcPlus_remove = None
    unspecified_typeid_flag = False
    remove_bifucNS_flag = False

    # NonStandard TypeID
    if (all(swc_matrix["TypeID"] >= 1) and all(swc_matrix["TypeID"] <= 7)):

        if do_NStudio:
            bifuc_flag = neuronstudio_check(swc_matrix)
        else:
            bifuc_flag = False

        if bifuc_flag:
            check_list_df.loc[5, ["Value", "Status", "ErrorMsg"]] = [True, "FAIL", "ERROR: Bifucartions tagged as 5 and Terminals tagged as 6"]
            remove_bifucNS_flag = True
        else:
            check_list_df.loc[5, ["Value", "Status", "ErrorMsg"]] = [False, "PASS", " "]

    else:
        if do_swcPlus:
            is_swcPlus_flag, nonstandard_somaID, swcPlus_remove = swcPlus_check(swc_matrix, Lines, commentline_numbers)
        else:
            nonstandard_somaID = None
            swcPlus_remove = None
            is_swcPlus_flag = False

        if not(nonstandard_somaID is None):
            temp_comment = "ERROR: Detected Soma TypeID=" + str(nonstandard_somaID) + ", must be 1."
            check_list_df.loc[5, ["Value", "Status", "ErrorMsg"]] = [True, "FAIL", temp_comment]
        else:
            if is_swcPlus_flag:
                check_list_df.loc[5, ["Value", "Status", "ErrorMsg"]] = [True, "FAIL", "ERROR: TypeID must be an integer in the range 1-7. SWC+ File Detected"]
            elif any(swc_matrix["TypeID"] == 0):  # Unspecified TypeID 0
                check_list_df.loc[5, ["Value", "Status", "ErrorMsg"]] = [True, "FAIL", "ERROR: TypeID=0 is undefined."]
                unspecified_typeid_flag = True
            else:
                check_list_df.loc[5, ["Value", "Status", "ErrorMsg"]] = [True, "FAIL", "ERROR: TypeID must be an integer in the range 1-7"]
                unspecified_typeid_flag = True

    # --Unspecified TypeID
    # if ( any(swc_matrix["TypeID"]==0) ):
    #     check_list_df.loc[6,["Value","Status","ErrorMsg"]] = [True, "FAIL", "ERROR: TypeID=0 is undefined."]
    # else:
    #     check_list_df.loc[6,["Value","Status","ErrorMsg"]] = [False,"PASS"," "]

    stop_check_flag = False

    return check_list_df, stop_check_flag, unspecified_typeid_flag, nonstandard_somaID, swcPlus_remove, remove_bifucNS_flag
