import numpy as np
import math
import pdb
import pandas as pd

## ------------------------------------------------------
##-- correct non-standard TypeID=0

def set_unspecified_typeID(swc_matrix):

    updated_swc_matrix = swc_matrix

    for i in range(0,len(updated_swc_matrix)):
        if (updated_swc_matrix.loc[i,"TypeID"]==0) or (updated_swc_matrix.loc[i,"TypeID"]>7) or pd.isna(updated_swc_matrix.loc[i,"TypeID"]):
            updated_swc_matrix.loc[i,"TypeID"]=6

    return updated_swc_matrix
