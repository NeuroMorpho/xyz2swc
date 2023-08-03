import pyvista as pv
import trimesh as tm
import os
from .mesh2swc import mesh2swc

# import pdb


def vtk2swc(inputfile, outputfile, logdir="./logs", mesh_configfile=None):
    # intermediate mesh file
    if not os.path.exists(logdir):
        os.mkdir(logdir)
    f = os.path.splitext(os.path.basename(inputfile))[0]
    meshfile = os.path.join(logdir, f + ".stl")

    # convert vtk to mesh
    try:
        polydata = pv.read(inputfile)
        mesh = polydata.extract_surface().triangulate()
        faces_as_array = mesh.faces.reshape((mesh.n_faces, 4))[:, 1:]
        tmesh = tm.Trimesh(mesh.points, faces_as_array)
        dump = tm.exchange.export.export_mesh(tmesh, meshfile)
    except Exception:
        return "FAIL"

    # convert stl to swc
    conversion_status = mesh2swc(meshfile, outputfile, mesh_configfile)

    return conversion_status
