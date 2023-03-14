import os
import sys
import glob
import numpy as np

def sort_matrix(swcmatrix):

    updated_swcmatrix = swcmatrix
    sRe = updated_swcmatrix[:,6]
    Li = list(range(1,(len(updated_swcmatrix[:,1])+1)))
    Li1 = Li[:-1]
    for i in Li1:
        if updated_swcmatrix[i,6] != -1:
            pids= np.where(updated_swcmatrix[:,0]==updated_swcmatrix[i,6])
            try:
                pids = float(pids[0])
            except TypeError:
                try:
                    pids = float(pids[0][0])
                except:
                    sRe[i] = -1
                    continue
            sRe[i] = pids+1
    updated_swcmatrix[:,6] = sRe
    updated_swcmatrix[:,0]= Li
    # breakpoint()
    swcmatrix = updated_swcmatrix
    updated_swcmatrix = np.empty((0,7),float)
    Px= np.where(swcmatrix[:,6]==-1)
    Px=list(Px[0])
    while len(Px)>0:
        P = Px[0]
        Px = Px[1:]
        while P.size>0:
            P=int(P)
            updated_swcmatrix = np.vstack((updated_swcmatrix,swcmatrix[P,:]))
            Child= np.where(swcmatrix[:,6]==swcmatrix[P,0])
            Child = list(Child[0])
            if len(Child)==0:
                break
            if len(Child)>1:
                Px= np.append(Child[1:],Px)
            P = Child[0]

    sRe = updated_swcmatrix[:,6]
    Li = list(range(1,(len(updated_swcmatrix[:,1])+1)))
    Li1 = Li[:-1]
    for i in Li1:
        if updated_swcmatrix[i,6] != -1:
            pids= np.where(updated_swcmatrix[:,0]==updated_swcmatrix[i,6])
            pids = float(pids[0])
            sRe[i] = pids+1
    updated_swcmatrix[:,6] = sRe
    updated_swcmatrix[:,0]= Li

    return updated_swcmatrix


def swc_sort(filename, filename_sorted=None):
    swcmatrix = []
    swcmatrix = np.loadtxt(filename, dtype=float, comments="#")

    updated_swcmatrix = sort_matrix(swcmatrix)

    if(filename_sorted is None):
        filename_sorted = os.path.basename(filename).split('.')[0]+'_sorted.swc'
        filename_sorted = os.path.join(os.path.dirname(filename),filename_sorted)

    np.savetxt(filename_sorted,updated_swcmatrix,fmt="%u %u %f %f %f %f %d")


if __name__ == '__main__':
    try:
        if(len(sys.argv)==3):
            swc_sort(sys.argv[1], sys.argv[2])
        else:
            swc_sort(sys.argv[1])
    except IndexError:
        print("At least one argument [input file] must be supplied!")
