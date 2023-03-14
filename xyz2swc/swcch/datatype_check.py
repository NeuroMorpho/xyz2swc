import numpy as np
import math
import pdb

## ------------------------------------------------------
##-- datatype_check

def datatype_check(swc_matrix, soma_rows, tree_rows, check_list_df):

    set_defaultradius_flag = False
    invalid_XYZ_flag = 0

    #-- check if ITP is int
    if ( np.issubdtype(swc_matrix["Index"].dtypes, int) == False ):
        check_list_df.loc[6,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: Index should be a positive integer."]
        index_int = False
    else:
        if( any(swc_matrix["Index"]==0) ):
            check_list_df.loc[6,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: Index cannot be zero."]
            index_int = False
        else:
            index_int = True

    if ( np.issubdtype(swc_matrix["TypeID"].dtypes, int) == False ):
        check_list_df.loc[6,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: TypeID should be an integer."]
        type_int = False
    else:
        type_int = True

    if ( np.issubdtype(swc_matrix["ParentIndex"].dtypes, int) == False ):
        check_list_df.loc[6,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: ParentIndex should be an integer."]
        parent_int = False
    else:
        parent_int = True

    if(index_int and type_int and parent_int):
        check_list_df.loc[6,["Value","Status","ErrorMsg"]] = [True,"PASS"," "]



    #-- check if XYZ is double
    XYZ_not_float = False
    XYZ_nan = False
    if (np.issubdtype(swc_matrix["X"].dtypes, float) == False or np.issubdtype(swc_matrix["Y"].dtypes, float) == False or np.issubdtype(swc_matrix["Z"].dtypes, float) == False):
        XYZ_not_float = True
    if (swc_matrix.loc[:, "X"].isnull().values.any()) or (swc_matrix.loc[:, "Y"].isnull().values.any()) or (swc_matrix.loc[:, "Z"].isnull().values.any()):
        XYZ_nan = True

    if XYZ_not_float and XYZ_nan:
        check_list_df.loc[7,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: XYZ coordinates need to be of type float/double."]
        invalid_XYZ_flag = 3
    elif (not XYZ_not_float) and XYZ_nan:
        check_list_df.loc[7,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: XYZ coordinates cannot be NaN."]
        invalid_XYZ_flag = 2
    elif XYZ_not_float and (not XYZ_nan):
        check_list_df.loc[7,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: XYZ coordinates need to be of type float/double."]
        invalid_XYZ_flag = 1
    else:
        check_list_df.loc[7,["Value","Status","ErrorMsg"]] = [True,"PASS"," "]
        invalid_XYZ_flag = 0

    #-- check if radius is positive double
    rad_lst = swc_matrix.loc[:,'Radius'].tolist()
    if ( len(set(rad_lst))==1 ):
        set_defaultradius_flag = True

    if (swc_matrix.loc[:,"Radius"].isnull().values.any()):
        check_list_df.loc[8,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: Radius needs to be a positive float/double."]
        set_defaultradius_flag = True
    else:
        if ( np.issubdtype(swc_matrix["Radius"].dtypes, float) == True ):
            if ( any(swc_matrix["Radius"]<0) ):
                check_list_df.loc[8,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: Radius cannot be negative."]
                set_defaultradius_flag = True
            elif ( any(swc_matrix["Radius"]==0) ):
                check_list_df.loc[8,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: Radius cannot be zero."]
                set_defaultradius_flag = True
            else:
                check_list_df.loc[8,["Value","Status","ErrorMsg"]] = [True,"PASS"," "]
        else:
            check_list_df.loc[8,["Value","Status","ErrorMsg"]] = [False,"FAIL","ERROR: Radius needs to be a positive float/double."]


    stop_check_flag = False

    return check_list_df, stop_check_flag, set_defaultradius_flag, invalid_XYZ_flag
