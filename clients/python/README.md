# NeuroMorphoAPI Python Wrapper

## Overview

This repository contains a Python wrapper for the NeuroMorpho API. The wrapper allows users to interact with NeuroMorpho's XYZ to SWC conversion services. Users can upload files for conversion, check file status, and download the converted files or associated log files.

## Features

- File Conversion: Convert neuron structure files to SWC format.
- Check Status: Check the status of your file conversions.
- Download Logs: Download logs associated with the file conversion.
- Download Zipped Files: Download the converted files in a ZIP format.

## Requirements

- Python 3.x
- `requests` package
- `os` and `tempfile` from Python's standard library
- `zipfile` for handling ZIP files

## Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/NeuroMorphoAPI-wrapper.git
    ```

2. Navigate to the cloned directory:
    ```bash
    cd NeuroMorphoAPI-wrapper
    ```

3. Install the required Python packages:
    ```bash
    pip install -r requirements.txt
    ```

## Usage

1. Import the `NeuroMorphoAPI` class from `xyz2swc-api.py`.
    ```python
    from xyz2swc-api import NeuroMorphoAPI
    ```

2. Create an instance of the API:
    ```python
    api = NeuroMorphoAPI()
    ```

3. Convert a file:
    ```python
    api.convert(open('input/to_convert/neuron_Neurolucida_DAT.dat', 'rb'))
    ```

4. Download the converted files:
    ```python
    folderpath = api.getzipped()
    ```

5. Unzip the files:
    ```python
    with ZipFile(f'{folderpath}/converted_files.zip', 'r') as zipObj:
        zipObj.extractall(folderpath)
    ```

6. To see the first 10 lines of each file in the directory:
    ```python
    files = os.listdir(folderpath)
    for file in files:
        with open(f'{folderpath}/{file}', 'r', encoding='latin-1') as f:
            print(f'File: {file}')
            for i in range(10):
                print(f.readline())
            print('\n')
    ```
