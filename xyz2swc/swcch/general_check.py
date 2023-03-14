import numpy as np
import math
import pdb

## ------------------------------------------------------
##-- general check

def general_check(swc_matrix, soma_rows, tree_rows, check_list_df):

    #-- check if they are 7 field columns
    if (len(swc_matrix.columns)!=7):
        check_list_df.loc[0].Value = True
        check_list_df.loc[0].Status = "FAIL"
        check_list_df.loc[0].ErrorMsg = "ERROR: SWC File requires a points matrix with 7 columns; Only " + str(len(swc_matrix.columns)) + " columns detected"
        stop_check_flag = True
        return check_list_df, stop_check_flag
    else:
        #-- find indicies of any rows which do not have 7 values
        invalid_samples = swc_matrix.isna().any(axis=1)
        invalid_sample_lineno, = np.where(invalid_samples==True) #np.where(invalid_samples)[0].shape[0]
        # print("\nNumber of invalid samples: %d" %(len(invalid_sample_lineno)))
        if(len(invalid_sample_lineno)>0):
            check_list_df.loc[0].Value = False
            check_list_df.loc[0].Status = "PASS"
            check_list_df.loc[0].ErrorMsg = "Warning! Some samples have NA values. Please check file integrity."
        else:
            check_list_df.loc[0].Value = False
            check_list_df.loc[0].Status = "PASS"


    #-- check number of tree and soma samples
    if(len(swc_matrix.index) == 0):
        check_list_df.loc[1,["Value","Status","ErrorMsg"]] = [0,"FAIL","ERROR: Empty File! No samples found."]
        stop_check_flag = True
        return check_list_df, stop_check_flag

    if(len(tree_rows.index)<=20):
        if(len(tree_rows.index)==0):
            check_list_df.loc[1,["Value","Status","ErrorMsg"]] = [len(tree_rows.index),"PASS","Warning! No tree samples found."]
        else:
            check_list_df.loc[1,["Value","Status","ErrorMsg"]] = [len(tree_rows.index),"PASS","Warning! Too few samples. Please check file integrity."]
    else:
        check_list_df.loc[1,["Value","Status","ErrorMsg"]] = [len(tree_rows.index),"PASS"," "]

    if(len(soma_rows.index)==0):
        check_list_df.loc[2,["Value","Status","ErrorMsg"]] = [len(soma_rows.index),"PASS","Warning! No soma samples found."]
    else:
        check_list_df.loc[2,["Value","Status","ErrorMsg"]] = [len(soma_rows.index),"PASS"," "]

    stop_check_flag = False

    return check_list_df, stop_check_flag
