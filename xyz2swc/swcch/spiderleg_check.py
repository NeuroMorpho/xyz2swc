import numpy as np
import math
import statistics
import pdb


# ------------------------------------------------------
# 3d distance
def distance3d(a, b):

    d = math.sqrt(math.pow(a[0] - b[0], 2) +
                  math.pow(a[1] - b[1], 2) +
                  math.pow(a[2] - b[2], 2) * 1.0)
    # print("Distance is ")
    # print(d)
    return d


# ------------------------------------------------------
# spiderleg check
def spiderleg_check(swc_matrix, soma_rows, tree_rows, check_list_df):

    cut_spiderlegs_flag = False

    if check_list_df.loc[7, "Value"] is True:

        # ls.pop(-1)
        compartment_lengths = []

        for i in swc_matrix.index.tolist():
            x1 = swc_matrix.loc[i, ["X", "Y", "Z"]].tolist()
            p_id = swc_matrix.loc[i, "ParentIndex"]
            if(p_id == -1):
                d = 0
                compartment_lengths.append(d)
                continue
            try:
                loc_of_parent = swc_matrix[(swc_matrix.Index == p_id)].index.to_list()[0]
            except IndexError:
                continue
            x2 = swc_matrix.loc[loc_of_parent, ["X", "Y", "Z"]].tolist()
            d = distance3d(x1, x2)
            compartment_lengths.append(d)

        compartment_lengths = np.asarray(compartment_lengths)
        nz_lengths = compartment_lengths[np.nonzero(compartment_lengths)]

        if len(nz_lengths) == 0:
            check_list_df.loc[12, ["Value", "Status", "ErrorMsg"]] = [False, "PASS", " "]
        else:
            res_sd = statistics.pstdev(nz_lengths)
            spiderleg_loc, = np.where(compartment_lengths > (5 * res_sd))

            if (len(spiderleg_loc) == 0):
                check_list_df.loc[12, ["Value", "Status", "ErrorMsg"]] = [False, "PASS", " "]
            else:
                check_list_df.loc[12, ["Value", "Status", "ErrorMsg"]] = [True, "PASS", "Warning! Abnormal compartments detected with very long lengths."]
                cut_spiderlegs_flag = True

    else:
        check_list_df.loc[12, ["Value", "Status", "ErrorMsg"]] = ["---", "---", "XYZ double check failed."]

    return check_list_df, cut_spiderlegs_flag
