import os
import sys
import glob
import pdb
import numpy as np
import pandas as pd


def eswc2swc(infile, outswcfile=None):

    if outswcfile is None:
        outswcfile = os.path.splitext(infile)[0] + '.swc'

    try:
        swc_matrix = pd.read_table(infile, comment='#', sep=r'\s+', header=None)
    except pd.errors.EmptyDataError:
        print("EmptyData ERROR: Unable to read and parse SWC File.")
        return 'FAIL'
    except Exception:
        print("ERROR: Unable to read and parse SWC File.")
        return 'FAIL'

    if (len(swc_matrix.columns) != 7):
        try:
            swc_matrix = swc_matrix.iloc[:, :7]
        except Exception:
            print("ERROR: Points matrix requires atleast 7 columns; Only " + str(len(swc_matrix.columns)) + " columns detected")
            return 'FAIL'

    try:
        np.savetxt(outswcfile, swc_matrix, fmt="%u %u %f %f %f %f %d")
        return 'SUCCESS'
    except Exception:
        return 'FAIL'
