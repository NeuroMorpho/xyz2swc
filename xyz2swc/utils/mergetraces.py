import os
import sys
import glob
import pdb
try:
    import cPickle as pickle
except:
    import pickle
import numpy as np
# import matplotlib.pyplot as plt

def mergetraces(ftr, outswcfile, logfiles=None):

    ftr_minus_traces = os.path.split(ftr)[-1]
    ftr_minus_traces = ftr_minus_traces[:-7]

    if outswcfile is None:
        outswcfile = logfiles + ftr_minus_traces + ".swc"

    if logfiles is None:
        logfiles = "../modules/snt/logfiles/"

    namswc = logfiles + ftr_minus_traces + "-exported*.swc"
    all_swc_tr = glob.glob(namswc)
    allfl=np.empty((0,7),float)
    for swc_tr in all_swc_tr:
        fl = np.loadtxt(swc_tr, dtype=float, comments="#",skiprows=1)
        if len(fl.shape) == 1:
            try:
                fl = fl.reshape(1, 7)
            except ValueError:
                continue
        else:
            if (fl.shape[1] != 7):
                try:
                    fl = fl.reshape(1, 7)
                except ValueError:
                    continue
        fl[:,0] =  fl[:,0] + len(allfl)
        fl[1:,6] =  fl[1:,6] + len(allfl)
        allfl = np.append (allfl,fl,axis=0)
    # if not os.path.exists('./out'):
    #     os.mkdir('./out')
    # os.chdir("./out")
    try:
        np.savetxt(outswcfile,allfl,fmt="%u %u %f %f %f %f %d")
        return 'SUCCESS'
    except:
        return 'FAIL'
    # os.chdir("..")
