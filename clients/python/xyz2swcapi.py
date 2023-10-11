import requests
import os
import tempfile
from zipfile import ZipFile



class NeuroMorphoAPI:
    def __init__(self, base_url="https://neuromorpho.org/xyz2swc/"):
        self.base_url = base_url
        self.foldername = None
        requests.packages.urllib3.util.ssl_.DEFAULT_CIPHERS += 'HIGH:!DH:!aNULL'

    def convert(self, input_files):
        url = f"{self.base_url}convertfiles"
        files = {'files': input_files}
        response = requests.post(url, files=files, verify=False)
        if response.status_code == 200:
            data = response.json().get('data')
            self.foldername = data['folder']
        else:
            print(f'Failed to convert files: {response.text}')

    def check(self, input_files):
        url = f"{self.base_url}checkfiles"
        files = {'files': input_files}
        response = requests.post(url, files=files, verify=False)
        if response.status_code == 200:
            data = response.json().get('data')
            self.foldername = data['folder']
        else:
            print(f'Failed to check files: {response.text}')

    def getlog(self):
        """
        Downloads the log file and saves it to a temporary folder.
        :return: path to the folder where the log file is stored."""
        if self.foldername is None:
            print("No folder associated. Please convert or check files first.")
            return
        url = f"{self.base_url}getlogs/{self.foldername}"
        response = requests.get(url,verify=False)
        if response.status_code == 200:
            temp_dir = tempfile.mkdtemp()
            with open(f"{temp_dir}/log.zip", "wb") as log_file:
                log_file.write(response.content)
            return temp_dir
        else:
            print(f'Failed to download log file: {response.text}')
            return None
        

    def getzipped(self) -> str:
        """
        Downloads the zipped files and returns the path to the folder where the files are stored.
        :return: path to the folder where the files are stored."""
        if self.foldername is None:
            print("No folder associated. Please convert or check files first.")
            return
        url = f"{self.base_url}getzipped/{self.foldername}"
        response = requests.get(url,verify=False)
        if response.status_code == 200:
            temp_dir = tempfile.mkdtemp()
            with open(f"{temp_dir}/converted_files.zip", "wb") as zip_file:
                zip_file.write(response.content)
            return temp_dir
        else:
            print(f'Failed to download zipped files: {response.text}')
            return None

    def newsession(self):
        self.foldername = None

if __name__ == "__main__":
    # Create an instance of the API
    api = NeuroMorphoAPI()
    # Convert a file
    api.convert(open('input/to_convert/neuron_Neurolucida_DAT.dat', 'rb'))
    # get the converted files
    folderpath = api.getzipped()
    # unzip the files
    with ZipFile(f'{folderpath}/converted_files.zip', 'r') as zipObj:
        zipObj.extractall(folderpath)
    # print the files to the console
    files = os.listdir(folderpath)
    for file in files:
        # open file and print the first 10 lines
        with open(f'{folderpath}/{file}', 'r', encoding='latin-1') as f:
            print(f'File: {file}')
            for i in range(10):
                print(f.readline())
            print('\n')
            



