import numpy as np
import math
import pdb


# -----------------------------------------------------
def checkConsecutive(index_list):
    return (sorted(index_list) == list(range(min(index_list), max(index_list) + 1))) and (sorted(index_list) == index_list)


# ------------------------------------------------------
# Index Check
def index_check(swc_matrix, soma_rows, tree_rows, check_list_df):

    reset_index_flag = False
    do_sort_flag = False

    if(check_list_df.loc[6, "Value"] is True):

        # Indexes are Sequential
        index_list = swc_matrix["Index"].tolist()

        if(checkConsecutive(index_list) is True):
            check_list_df.loc[9, ["Value", "Status", "ErrorMsg"]] = [True, "PASS", " "]
        else:
            check_list_df.loc[9, ["Value", "Status", "ErrorMsg"]] = [False, "FAIL", "ERROR: Indexes must be sequential."]
            reset_index_flag = True
            do_sort_flag = True

        # Sorted Index Order
        if(check_list_df.loc[4, "Value"] is False):
            not_sorted_flag = False
            for row_index, row_data in swc_matrix.iterrows():
                p_id = row_data.ParentIndex
                if p_id != -1:
                    loc_of_parent = swc_matrix[(swc_matrix.Index == p_id)].index.to_list()[0]
                    if (swc_matrix.index.get_loc(loc_of_parent) >= swc_matrix.index.get_loc(row_index)):
                        not_sorted_flag = True
                        break
            if (not_sorted_flag):
                check_list_df.loc[10, ["Value", "Status", "ErrorMsg"]] = [False, "FAIL", "ERROR: Parent samples must be defined before they are being referred to."]
                do_sort_flag = True
            else:
                check_list_df.loc[10, ["Value", "Status", "ErrorMsg"]] = [True, "PASS", " "]
        else:
            check_list_df.loc[10, ["Value", "Status", "ErrorMsg"]] = ["---", "---", "Invalid ParentIDs present"]

    else:
        check_list_df.loc[9, ["Value", "Status", "ErrorMsg"]] = ["---", "---", "ITP int check failed"]

    stop_check_flag = False

    return check_list_df, stop_check_flag, reset_index_flag, do_sort_flag
