import os
import tempfile
from zipfile import ZipFile
from xyz2swcapi import NeuroMorphoAPI 

def test_convert():
    api = NeuroMorphoAPI()
    api.convert(open('../../input/to_convert/neuron_Neurolucida_DAT.dat', 'rb'))
    assert api.foldername is not None, "Folder name should be set after conversion."

def test_check():
    api = NeuroMorphoAPI()
    api.check(open('../../input/to_convert/neuron_Neurolucida_DAT.dat', 'rb'))
    assert api.foldername is not None, "Folder name should be set after checking."

def test_getlog():
    api = NeuroMorphoAPI()
    api.convert(open('../../input/to_convert/neuron_Neurolucida_DAT.dat', 'rb'))
    temp_dir = api.getlog()
    assert temp_dir is not None, "Should return a temporary directory."
    assert os.path.exists(f"{temp_dir}/log.zip"), "Log file should exist in temporary directory."

def test_getzipped():
    api = NeuroMorphoAPI()
    api.convert(open('../../input/to_convert/neuron_Neurolucida_DAT.dat', 'rb'))
    temp_dir = api.getzipped()
    assert temp_dir is not None, "Should return a temporary directory."
    zip_file_path = f"{temp_dir}/converted_files.zip"
    assert os.path.exists(zip_file_path), "ZIP file should exist in temporary directory."
    with ZipFile(zip_file_path, 'r') as zipObj:
        assert len(zipObj.namelist()) > 0, "ZIP file should contain files."
