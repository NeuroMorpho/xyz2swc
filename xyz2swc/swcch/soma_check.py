import numpy as np
import math
import pdb


# -----------------------------------------------------
def checkConsecutive(index_list):
    return (sorted(index_list) == list(range(min(index_list), max(index_list) + 1))) and (sorted(index_list) == index_list)


# ------------------------------------------------------
# --Angle
def angle3d(a, b, c):

    v1 = np.array([ a[0] - b[0], a[1] - b[1], a[2] - b[2] ])
    v2 = np.array([ c[0] - b[0], c[1] - b[1], c[2] - b[2] ])

    v1mag = np.sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2])
    v2mag = np.sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2])

    dot_product = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]

    angle_rad = np.arccos( dot_product/(v1mag*v2mag) )

    return math.degrees(angle_rad)


# ------------------------------------------------------
# --Curvature
def curvature3d(a, b, c):

    v1 = np.array([a[0] - b[0], a[1] - b[1], a[2] - b[2]])
    v2 = np.array([c[0] - b[0], c[1] - b[1], c[2] - b[2]])

    v1mag = np.sqrt([v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]])
    v2mag = np.sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2])

    dot_product = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]

    angle_rad = np.arccos( dot_product/(v1mag*v2mag) )

    v3 =  np.array([ a[0] - c[0], a[1] - c[1], a[2] - c[2] ])
    v3mag = np.sqrt(v3[0] * v3[0] + v3[1] * v3[1] + v3[2] * v3[2])

    curva = (2*np.sin(angle_rad)/v3mag)

    return curva


# ------------------------------------------------------
# --3d distance
def distance3d(a, b):

    d = math.sqrt(math.pow(a[0] - b[0], 2) +
                  math.pow(a[1] - b[1], 2) +
                  math.pow(a[2] - b[2], 2) * 1.0)
    # print("Distance is ")
    # print(d)
    return d


# ------------------------------------------------------
# --3d distance
def distance_sum_3points(a, b, c):

    return (distance3d(a, b) + distance3d(b, c))


# ------------------------------------------------------
# --Contour center and radius
def somacenter(soma_xyz):

    mean_x = np.mean(soma_xyz['X'])
    mean_y = np.mean(soma_xyz['Y'])
    mean_z = np.mean(soma_xyz['Z'])
    center_xyz = [mean_x, mean_y, mean_z]

    distance_vector = [None]*len(soma_xyz)
    for i in range(0, len(soma_xyz)):
        point_cord = soma_xyz.iloc[i].tolist()
        distance_vector[i] = distance3d(point_cord, center_xyz)

    soma_radius = np.mean(distance_vector)

    return center_xyz, soma_radius


# ------------------------------------------------------
# --Contour center and radius
def contour_or_cylinder(soma_xyz):

    angle_vector = [None]*(len(soma_xyz)-2)
    curvature_vector = [None]*(len(soma_xyz)-2)
    distance_vector = [None]*(len(soma_xyz)-2)

    for i in range(0, (len(soma_xyz)-2)):
        a = soma_xyz.iloc[0].tolist()
        b = soma_xyz.iloc[i+1].tolist()
        c = soma_xyz.iloc[len(soma_xyz)-1].tolist()
        angle_vector[i] = angle3d(a, b, c)
        curvature_vector[i] = curvature3d(a, b, c)
        distance_vector[i] = distance_sum_3points(a, b, c)

    distance_vector = np.array(distance_vector)
    max_ind, = np.where(distance_vector == distance_vector.max())

    angle_vector = np.array(angle_vector)

    angles_to_check = angle_vector[max_ind]

    return angle_vector.tolist(), curvature_vector, angles_to_check


# ------------------------------------------------------
# --soma check
def soma_check(swc_matrix, soma_rows, tree_rows, check_list_df, verbose=False):

    stop_check_flag = False
    convert_contour_flag = False
    if(check_list_df.loc[2,"Value"]==0):
        check_list_df.loc[11,["Value","Status","ErrorMsg"]] = [False,"PASS","Warning! No soma samples found."]
        soma_centerxyz = None
        soma_radius = None
        return check_list_df, stop_check_flag, soma_centerxyz, soma_radius, convert_contour_flag
    elif(check_list_df.loc[2,"Value"]==1 or check_list_df.loc[2,"Value"]==2):
        check_list_df.loc[11,["Value","Status","ErrorMsg"]] = [False,"PASS","Warning! Need 3 or more soma smaples to detect contour."]
        soma_centerxyz = None
        soma_radius = None
        return check_list_df, stop_check_flag, soma_centerxyz, soma_radius, convert_contour_flag
    else:
        if (len(set(swc_matrix.ParentIndex[swc_matrix['TypeID'] == 1])) != len(swc_matrix.ParentIndex[swc_matrix['TypeID'] == 1])) or not(checkConsecutive(soma_rows['Index'].tolist())):
            check_list_df.loc[11,["Value","Status","ErrorMsg"]] = [False,"PASS"," "]
            soma_centerxyz = None
            soma_radius = None
            return check_list_df, stop_check_flag, soma_centerxyz, soma_radius, convert_contour_flag
        else:
            soma_xyz = swc_matrix.loc[swc_matrix['TypeID'] == 1, ['X', 'Y', 'Z']]
            soma_centerxyz, soma_radius = somacenter(soma_xyz)
            angle_vector, curvature_vector, angles_to_check = contour_or_cylinder(soma_xyz)
            all_acute = np.all(angles_to_check<=90)
            if(all_acute):
                check_list_df.loc[11,["Value","Status","ErrorMsg"]] = [True,"FAIL","ERROR: Contours not permitted in standardized SWC."]
                convert_contour_flag = True
            else:
                check_list_df.loc[11,["Value","Status","ErrorMsg"]] = [False,"PASS"," "]

    if(verbose):
        print("\n\nSoma center: X=%0.3f Y=%0.3f Z=%0.3f" %(soma_centerxyz[0],soma_centerxyz[1],soma_centerxyz[2]))
        print("Soma raidus is: %0.3f" %(soma_radius))
        print("3d angles are:")
        print(['%.3f' % elem for elem in angle_vector])
        print("Curvatures are:")
        print(['%.3f' % elem for elem in curvature_vector])
        print("Angles to check:")
        print(['%.3f' % elem for elem in angles_to_check])
        print("Curvature angle is acute? %r" %(all_acute))
        print("Soma check flag is ", convert_contour_flag)
        print("\n\n-----------------------------------------\n\n")

    return check_list_df, stop_check_flag, soma_centerxyz, soma_radius, convert_contour_flag
