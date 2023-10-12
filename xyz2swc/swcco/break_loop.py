def break_loop(swc_matrix):
    """
    removes any loops in the SWC tree
    """
    updated_swc_matrix = swc_matrix
    tree_traversed = []
    for row_index, row_data in swc_matrix.iterrows():
        p_id = row_data.ParentIndex
        if p_id == -1:
            tree_traversed.append(row_index)
            continue
        if p_id not in tree_traversed:
            loc_of_parent = swc_matrix[(swc_matrix.Index == p_id)].index.to_list()[0]
            if swc_matrix.index.get_loc(loc_of_parent) >= swc_matrix.index.get_loc(row_index):
                row_of_interest = swc_matrix.index.get_loc(row_index)
                start_chain_at_parent = swc_matrix.index.get_loc(loc_of_parent)
                loop_exists, chain_elements = check_for_loop(swc_matrix, row_of_interest, start_chain_at_parent, tree_traversed)
                tree_traversed = tree_traversed + chain_elements
                if loop_exists:
                    swc_matrix.loc[row_of_interest, "ParentIndex"] = [-1]
    return updated_swc_matrix


def check_for_loop(swc_matrix, row_of_interest, start_chain_at_parent, tree_traversed):
    if row_of_interest == start_chain_at_parent:
        return True, [row_of_interest]
    else:
        chain_elements = [row_of_interest, start_chain_at_parent]
        Parent_exists = True

    i = start_chain_at_parent
    while Parent_exists is True:
        next_point = swc_matrix.loc[i].ParentIndex
        try:
            i = swc_matrix[(swc_matrix.Index == next_point)].index.to_list()[0]
            if i in chain_elements:
                return True, chain_elements
            elif i in tree_traversed:
                return False, chain_elements
            else:
                chain_elements.append(i)
                Parent_exists = True
        except IndexError:
            Parent_exists = False
            return False, chain_elements
