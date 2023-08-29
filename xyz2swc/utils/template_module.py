# -- import required Python modules
import numpy


def converter_func(inputfile, outputfile):
    """
    Function for converting a file to SWC format.

    Required arguments:
        inputfile:          [string]    Path of the reconstruction format to convert.
        outputfile:         [string]    Path of converted SWC file.

    Returns:
        conversion_status:  [string]    'SUCCESS' if conversion had no errors;
                                        'FAIL' if there were errors;
    """

    # -- Basic Sturcture
    try:
        """
        1. Read 'inputfile'
                -- decode input file structure
                -- extract the reconstruction data in inputfile

        2. Conversion operation
                -- translate reconstruction data to build an equivalent SWC points matrix
                -- SWC need not meet standard specifications at this stage,
                   as 'outputfile' is later verified and corrected by the standardization
                   [./xyz2sc/standard.py] module.

        3. Write (create new) 'outputfile'
                -- save the data matrix as a .swc format file
                -- can be easily done with the NumPy package:
                   numpy.savetxt(outputfile, swc_matrix, fmt="%u %u %f %f %f %f %d")
        """
        return "SUCCESS"

    except Exception:
        return "FAIL"
