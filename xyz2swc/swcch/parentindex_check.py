import numpy as np
import math
import pdb


# ------------------------------------------------------
# ParentIndex Check


def parentindex_check(swc_matrix, soma_rows, tree_rows, check_list_df):

    set_root_flag = False
    invalidparent_to_root_flag = False

    # check root ParentIndex is -1
    check_list_df.loc[3].Value = swc_matrix.loc[0].ParentIndex
    if (check_list_df.loc[3].Value == -1):
        check_list_df.loc[3].Status = "PASS"
    else:
        pid_lst = swc_matrix.loc[:, 'ParentIndex'].tolist()
        if (-1 in pid_lst):
            check_list_df.loc[3].Status = "FAIL"
            check_list_df.loc[3].ErrorMsg = "ERROR: Root ParentIndex should be -1."
            set_root_flag = True
        else:
            check_list_df.loc[3].Status = "FAIL"
            check_list_df.loc[3].ErrorMsg = "ERROR: No Root detected (sample with ParentIndex=-1)."

    # check ParentIndex is valid
    p_list = swc_matrix.ParentIndex.tolist()
    i_list = swc_matrix.Index.tolist()
    i_list.append(-1)
    if(all(x in i_list for x in p_list)):
        check_list_df.loc[4].Value = False
        check_list_df.loc[4].Status = "PASS"
    else:
        # breakpoint()
        check_list_df.loc[4].Value = True
        check_list_df.loc[4].Status = "FAIL"
        check_list_df.loc[4].ErrorMsg = "ERROR: ParentIndex of sample does not match any Index."
        invalidparent_to_root_flag = True

    stop_check_flag = False

    return check_list_df, stop_check_flag, set_root_flag, invalidparent_to_root_flag
