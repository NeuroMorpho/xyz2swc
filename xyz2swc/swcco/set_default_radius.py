import numpy as np
import math
import pdb

# ------------------------------------------------------
# Set default radius to 0.5


def set_default_radius(swc_matrix):

    updated_swc_matrix = swc_matrix

    if (swc_matrix.loc[:, "Radius"].isnull().values.any()):
        updated_swc_matrix["Radius"] = updated_swc_matrix["Radius"].replace({np.nan: 0.5})

    rad_lst = updated_swc_matrix.loc[:, 'Radius'].tolist()
    if (len(set(rad_lst)) == 1):
        updated_swc_matrix.loc[:, "Radius"] = 0.5
    else:
        for i in range(0, len(updated_swc_matrix)):
            if (updated_swc_matrix.loc[i, "Radius"] <= 0):
                updated_swc_matrix.loc[i, 'Radius'] = 0.5

    return updated_swc_matrix
