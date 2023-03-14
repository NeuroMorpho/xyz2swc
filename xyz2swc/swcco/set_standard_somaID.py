import numpy as np
import pandas as pd
import math
import pdb

# ------------------------------------------------------
# Correct non standard TypeIDs for soma


def set_standard_somaID(swc_matrix, nonstandard_somaID=None, swcPlus_remove=None):

    updated_swc_matrix = swc_matrix

    if not(nonstandard_somaID is None):
        for i in range(0, len(updated_swc_matrix)):
            if (updated_swc_matrix.loc[i, "TypeID"] == nonstandard_somaID):
                updated_swc_matrix.loc[i, "TypeID"] = 1

    if not(swcPlus_remove is None):
        temp_list = []
        for i in range(0, len(updated_swc_matrix)):
            if (updated_swc_matrix.loc[i, "TypeID"] in swcPlus_remove):
                temp_list.append(i)
            if (updated_swc_matrix.loc[i, "TypeID"] == 0) or (updated_swc_matrix.loc[i, "TypeID"] > 7) or pd.isna(updated_swc_matrix.loc[i, "TypeID"]):
                updated_swc_matrix.loc[i, "TypeID"] = 6
        updated_swc_matrix = updated_swc_matrix.drop(temp_list)
        updated_swc_matrix = updated_swc_matrix.reset_index(drop=True)

    return updated_swc_matrix
