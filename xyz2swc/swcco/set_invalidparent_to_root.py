import numpy as np
import math
import pdb

# ------------------------------------------------------
# -- set invalid ParentIndex to -1 (make it a root)


def set_invalidparent_to_root(swc_matrix):

    nochange_flag = False

    updated_swc_matrix = swc_matrix

    if (swc_matrix.loc[:, "ParentIndex"].isnull().values.any()):
        updated_swc_matrix["ParentIndex"] = updated_swc_matrix["ParentIndex"].replace({np.nan: -1})

    p_list = swc_matrix.ParentIndex.tolist()
    i_list = swc_matrix.Index.tolist()
    i_list.append(-1)

    invalid_pids = np.setdiff1d(p_list, i_list).tolist()
    invalid_pid_indicies = []

    for i in invalid_pids:
        temp = swc_matrix.index[swc_matrix['ParentIndex'] == i].tolist()
        invalid_pid_indicies.extend(temp)

    if (len(invalid_pid_indicies) > 0):
        swc_matrix.loc[invalid_pid_indicies, 'ParentIndex'] = [-1] * len(invalid_pid_indicies)
    else:
        nochange_flag = True

    return updated_swc_matrix, nochange_flag
