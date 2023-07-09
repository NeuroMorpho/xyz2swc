# Example how to use the xyz2swc API
# 1 ) upload and convert two files to the server
# 2 ) download the swc files

import requests
import os

# set the cipher list to avoid SSL errors, needed temporarily
requests.packages.urllib3.util.ssl_.DEFAULT_CIPHERS += 'HIGH:!DH:!aNULL'

# define base url
base_url = 'https://neuromorpho.org/xyz2swc/'

# define the files to upload
file1path = 'input/to_convert/neuron_Neurolucida_DAT.dat'
file2path = 'input/to_convert/neuron_Neurolucida_ASC.asc'
files = [('files', (os.path.basename(file1path), open(file1path, 'rb'))),
         ('files', (os.path.basename(file2path), open(file2path, 'rb')))]

# upload and convert the files
print(files)
r = requests.post(base_url + 'convertfiles',
                  files=files,
                  verify=False)
folder = r.json()['data']['folder']

# download the swc files
r = requests.get(base_url + 'getzipped/' + folder,
                 stream=True,
                 verify=False)
with open('output/converted.zip', 'wb') as f:
    for chunk in r.iter_content(chunk_size=1024):
        if chunk:
            f.write(chunk)

# download the logs
r = requests.get(base_url + 'getlogs/' + folder, stream=True)
with open('output/logs.zip', 'wb') as f:
    for chunk in r.iter_content(chunk_size=1024):
        if chunk:
            f.write(chunk)
