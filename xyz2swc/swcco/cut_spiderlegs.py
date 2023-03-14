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
# Cut Spider Legs
def cut_spiderlegs(swc_matrix, tree_rows, soma_rows, verbose):

    updated_swc_matrix = swc_matrix

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
    res_sd = statistics.pstdev(nz_lengths)

    spiderleg_loc, = np.where(compartment_lengths > (5 * res_sd))

    if (len(spiderleg_loc) > 0):

        for i in spiderleg_loc:
            p_id = swc_matrix.loc[i, "ParentIndex"]
            try:
                loc_of_parent = swc_matrix[(swc_matrix.Index == p_id)].index.to_list()[0]
            except IndexError:
                continue
            # cut spider legs originating from soma
            if swc_matrix.loc[loc_of_parent, "TypeID"] == 1:
                if verbose:
                    print("Making index", i, "a root")
                updated_swc_matrix.loc[i, 'ParentIndex'] = -1

    return updated_swc_matrix
