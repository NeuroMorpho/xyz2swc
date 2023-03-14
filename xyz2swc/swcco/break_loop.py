import numpy as np
import math
import pdb

# ------------------------------------------------------
# -- set invalid ParentIndex to -1 (make it a root)


def break_loop(swc_matrix):

    updated_swc_matrix = swc_matrix

    for row_index, row_data in swc_matrix.iterrows():
        p_id = row_data.ParentIndex
        if p_id != -1:
            loc_of_parent = swc_matrix[(swc_matrix.Index == p_id)].index.to_list()[0]
            if (swc_matrix.index.get_loc(loc_of_parent) >= swc_matrix.index.get_loc(row_index)):
                parent_not_defined = True
                row_of_interest = swc_matrix.index.get_loc(row_index)
                start_chain_at_parent = swc_matrix.index.get_loc(loc_of_parent)
                loop_exists = check_for_loop(swc_matrix, row_of_interest, start_chain_at_parent)
                if loop_exists:
                    swc_matrix.loc[row_of_interest, 'ParentIndex'] = [-1]

    return updated_swc_matrix


def check_for_loop(swc_matrix, row_of_interest, start_chain_at_parent):

    if row_of_interest == start_chain_at_parent:
        return True
    else:
        chain_elements = [row_of_interest, start_chain_at_parent]
        Parent_exists = True

    i = start_chain_at_parent
    while Parent_exists is True:
        next_point = swc_matrix.loc[i].ParentIndex
        try:
            i = swc_matrix[(swc_matrix.Index == next_point)].index.to_list()[0]
            if i in chain_elements:
                return True
            else:
                chain_elements.append(i)
                Parent_exists = True
        except IndexError:
            Parent_exists = False
            return False
