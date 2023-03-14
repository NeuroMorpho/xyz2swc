import numpy as np
import math
import pdb

# ------------------------------------------------------
# -- set invalid ParentIndex to -1 (make it a root)


def correct_invalid_xyz(swc_matrix, invalid_XYZ_flag):

    tofloat_success = True
    updated_swc_matrix = swc_matrix

    x = np.array(swc_matrix.X)
    y = np.array(swc_matrix.Y)
    z = np.array(swc_matrix.Z)

    # -- Float
    if (invalid_XYZ_flag == 1) or (invalid_XYZ_flag == 3):

        x_is_float = True if np.issubdtype(x.dtype, float) else False
        y_is_float = True if np.issubdtype(y.dtype, float) else False
        z_is_float = True if np.issubdtype(z.dtype, float) else False

        if (x_is_float and y_is_float and z_is_float):
            tofloat_success = True
            return updated_swc_matrix, tofloat_success
        else:
            # Try forcing to Float
            try:
                np.float_(x_list)
                np.float_(y_list)
                np.float_(z_list)
                tofloat_success = True
                updated_swc_matrix.loc[:, 'X'] = x
                updated_swc_matrix.loc[:, 'Y'] = y
                updated_swc_matrix.loc[:, 'Z'] = z
                return updated_swc_matrix, tofloat_success
            except Exception:
                tofloat_success = False

        for i in range(0, len(updated_swc_matrix)):

            if not x_is_float:
                try:
                    x[i] = np.float(x[i])
                except Exception:
                    x[i] = 0.0

            if not y_is_float:
                try:
                    y[i] = np.float(y[i])
                except Exception:
                    y[i] = 0.0

            if not z_is_float:
                try:
                    z[i] = np.float(z[i])
                except Exception:
                    z[i] = 0.0

        updated_swc_matrix.loc[:, 'X'] = x
        updated_swc_matrix.loc[:, 'Y'] = y
        updated_swc_matrix.loc[:, 'Z'] = z

    # -- NaN
    if (invalid_XYZ_flag == 2) or (invalid_XYZ_flag == 3):

        if (swc_matrix.loc[:, "X"].isnull().values.any()):
            updated_swc_matrix["X"] = updated_swc_matrix["X"].replace({np.nan: 0.0})

        if (swc_matrix.loc[:, "Y"].isnull().values.any()):
            updated_swc_matrix["Y"] = updated_swc_matrix["Y"].replace({np.nan: 0.0})

        if (swc_matrix.loc[:, "Z"].isnull().values.any()):
            updated_swc_matrix["Z"] = updated_swc_matrix["Z"].replace({np.nan: 0.0})

    return updated_swc_matrix, tofloat_success
