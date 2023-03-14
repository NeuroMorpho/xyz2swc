import numpy as np
import math
import pdb

## ------------------------------------------------------
##-- remove bifurication TypeIDs 5 and 6 from NeuronStudio SWCs

def remove_bifuc_neuronstudio(swc_matrix):

    updated_swc_matrix = swc_matrix

    for i in range(0,len(updated_swc_matrix)):
        if updated_swc_matrix.loc[i,'TypeID'] == 5:
            index_of_interest = updated_swc_matrix.loc[i,'Index']
            branches = updated_swc_matrix[updated_swc_matrix.ParentIndex==index_of_interest]
            while (len(branches)!=0):
                branch_typeIDs = branches.TypeID.tolist()
                if min(branch_typeIDs)<=5:
                    updated_swc_matrix.loc[i,'TypeID'] = min(branch_typeIDs)
                    break
                elif max(branch_typeIDs)>=6:
                    updated_swc_matrix.loc[i,'TypeID'] = max(branch_typeIDs)
                    break
                else:
                    index_of_interest = branches.loc[i,'Index']
                    branches = updated_swc_matrix[updated_swc_matrix.ParentIndex==index_of_interest]

        if updated_swc_matrix.loc[i,'TypeID'] == 6:
            index_of_interest = updated_swc_matrix.loc[i,'ParentIndex']
            if (index_of_interest in updated_swc_matrix.Index):
                updated_swc_matrix.loc[i,'TypeID'] = updated_swc_matrix[updated_swc_matrix.Index==index_of_interest].TypeID.tolist()[0]

    return updated_swc_matrix
