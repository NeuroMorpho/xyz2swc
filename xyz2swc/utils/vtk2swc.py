import pyvista as pv
import numpy as np


def vtk2swc(inputfile, outputfile):
    """
    Function to convert a .vtk file into a .swc file.

    Required arguments:
        inputfile:          [string]    Path of the reconstruction format to convert.
        outputfile:         [string]    Path of converted SWC file.

    Returns:
        conversion_status:  [string]    'SUCCESS' if conversion had no errors; 'FAIL' if there were errors
    """

    try:
        polydata = pv.read(inputfile)
        # pv.is_pyvista_dataset(polydata)
        # extract linesegments from vtk file
        polylines = []
        i, offset = 0, 0
        cc = polydata.lines
        while i < polydata.n_cells:
            nn = cc[offset]
            polylines.append(cc[offset + 1 : offset + 1 + nn])
            offset += nn + 1
            i += 1
        #
        lines = []
        for poly in polylines:
            lines.append(np.column_stack((poly[:-1], poly[1:])))
        lines = np.vstack(lines)
        # cells = np.column_stack((np.full(len(lines), 2), lines))

        # check if lines were extracted
        if lines.shape[1] != 2:
            return "FAIL"
        parentID_list = [-1] * polydata.n_points
        for i in range(0, lines.shape[0]):
            parentID_list[lines[i][1]] = lines[i][0] + 1

        swc = np.empty([polydata.n_points, 7])
        swc[:, 0] = range(1, polydata.n_points + 1)
        swc[:, 1] = [0] * polydata.n_points
        swc[:, 2:5] = polydata.points
        swc[:, 5] = [0.5] * polydata.n_points
        swc[:, 6] = parentID_list

        np.savetxt(outputfile, swc, fmt="%u %u %f %f %f %f %d")

        return "SUCCESS"

    except Exception:
        return "FAIL"

    # return conversion_status
