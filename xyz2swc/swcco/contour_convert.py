import numpy as np
import pandas as pd
import math
import pdb


# ------------------------------------------------------
# -- 3d distance
def distance3d(a, b):

    d = math.sqrt(math.pow(a[0] - b[0], 2) +
                  math.pow(a[1] - b[1], 2) +
                  math.pow(a[2] - b[2], 2) * 1.0)
    # print("Distance is ")
    # print(d)
    return d


# ------------------------------------------------------
# -- Contour center and radius
def somacenter(soma_xyz):

    mean_x = np.mean(soma_xyz['X'])
    mean_y = np.mean(soma_xyz['Y'])
    mean_z = np.mean(soma_xyz['Z'])
    center_xyz = [mean_x, mean_y, mean_z]

    distance_vector = [None] * len(soma_xyz)
    for i in range(0, len(soma_xyz)):
        point_cord = soma_xyz.iloc[i].tolist()
        distance_vector[i] = distance3d(point_cord, center_xyz)

    soma_radius = np.mean(distance_vector)

    return center_xyz, soma_radius


# ------------------------------------------------------
# -- Convert contour to single point soma
def contour_convert(swc_matrix, tree_rows, soma_rows):

    soma_start_dfindex = soma_rows.index[0]
    soma_start_Index = soma_rows.iloc[0]['Index']
    soma_end_dfindex = soma_rows.index[len(soma_rows) - 1]
    soma_end_Index = soma_rows.iloc[len(soma_rows) - 1]['Index']

    soma_xyz = soma_rows.loc[:, ['X', 'Y', 'Z']]  # swc_matrix.loc[swc_matrix['TypeID']==1,['X','Y','Z']]
    soma_centerxyz, soma_radius = somacenter(soma_xyz)
    single_point_soma = [soma_start_Index, 1, soma_centerxyz[0], soma_centerxyz[1], soma_centerxyz[2], soma_radius, soma_rows.iloc[0]['ParentIndex']]
    single_point_soma_df = pd.DataFrame([single_point_soma], columns=list(swc_matrix.columns))

    # sample points that have soma as parent
    soma_root_indicies = tree_rows.index[tree_rows['ParentIndex'].isin(soma_rows['Index'])]
    # sample points that have root (-1) for parent
    root_indicies = tree_rows.index[tree_rows['ParentIndex'] == -1]

    tree_rows_before = tree_rows.loc[tree_rows['Index'] < soma_start_Index]
    tree_rows_after = tree_rows.loc[tree_rows['Index'] > soma_end_Index]

    tree_rows_after[:]['Index'] = tree_rows_after['Index'] - len(soma_rows.index) + len(single_point_soma_df)
    tree_rows_after[:]['ParentIndex'] = tree_rows_after['ParentIndex'] - len(soma_rows.index) + len(single_point_soma_df)

    if len(soma_root_indicies[soma_root_indicies < soma_start_dfindex]) > 0:
        tree_rows_before.loc[soma_root_indicies[soma_root_indicies < soma_start_dfindex], 'ParentIndex'] = soma_start_Index

    if len(soma_root_indicies[soma_root_indicies > soma_end_dfindex]) > 0:
        tree_rows_after.loc[soma_root_indicies[soma_root_indicies > soma_end_dfindex], 'ParentIndex'] = soma_start_Index

    if len(root_indicies[root_indicies > soma_end_dfindex]) > 0:
        tree_rows_after.loc[root_indicies[root_indicies > soma_end_dfindex], 'ParentIndex'] = -1
    # tree_rows.at[tree_rows.first_valid_index(), 'ParentIndex'] = len(single_point_soma_df)

    updated_swc_matrix = tree_rows_before.append(single_point_soma_df, ignore_index=True)
    updated_swc_matrix = updated_swc_matrix.append(tree_rows_after, ignore_index=True)
    # updated_swc_matrix = pd.concat([single_point_soma_df, tree_rows], ignore_index = True)
    # df = df.sort_index().reset_index(drop=True) # insert at 0.5 location
    # df.loc[len(df)] = list

    return updated_swc_matrix
