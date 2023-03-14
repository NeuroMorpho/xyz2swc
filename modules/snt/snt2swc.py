#@String (label="Input File", description="Input File") traces_file

import os
import re
from tracing import PathAndFillManager
from ij import IJ
import shutil


traces_filename = os.path.split(traces_file)[-1]
print('file is: ' + traces_file)
print('filename is: ' + traces_filename)

logfiles = "/app/modules/snt/logfiles/"

# d = IJ.getDirectory("Choose your directory of .traces files...")

if traces_file.endswith(".traces"):
    swc_filename_prefix = re.sub(r'\.traces', '-exported', traces_filename)
    swc_filename_prefix = logfiles + swc_filename_prefix
    print(swc_filename_prefix)
    if os.path.exists(logfiles):
        shutil.rmtree(logfiles)
    os.mkdir(logfiles)

    # IJ.log("Converting %s to %s-*.swc" % (traces_file, swc_filename_prefix))
    pafm = PathAndFillManager()
    pafm.loadGuessingType(traces_file)
    if pafm.checkOKToWriteAllAsSWC(swc_filename_prefix):
        pafm.exportAllAsSWC(swc_filename_prefix)
else:
    print('Input file needs to be of type .traces')

######################################################################
