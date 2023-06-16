# *xyz2swc*

This repository contains the source code for the *xyz2swc* software --- a universal online service for converting digital reconstructions of neural morphologies into standardized SWC format.

---
<br/>


## Running *xyz2swc* online [Preferred Method] 

***Accessing the service:*** The *xyz2swc* software is available as a free and ready-to-use online service. It is accessible through any common internet browser via a user friendly web-based graphical interface at https://neuromorpho.org/xyz2swc/ui/.

***Prerequisites:*** The online service is operating system and programming language independent, and requires no local package installation. 

***Instructions for use:*** To convert your digital tracings, simply upload the reconstruction files (either individually or as a zipped archive) and select the “Convert/Standardize” option. The service automatically detects the format of the uploaded files, performs the data conversion, and provides the converted standardized SWC files for download.


***Demo Samples:*** For those wanting to quickly test out the service, the `input` folder contains sample files of different types of reconstruction formats, while the `output` folder contains the corresponding standardized SWC files.

The average runtime to convert and standardize an input file is approximately 15 seconds, but can be as long as 5-7 minutes for large (>5 MB) files and/or complex format variations. The graphical user interface clearly indicates the progress and conversion status of each input file.

***Results in the manuscript can be replicated:*** by downloading the 'original' digital reconstruction files available on the NeuroMorpho.Org (version 8.4) repository, and converting them using the online service. The generated log files can be used to determine the number (percentage) of files successfully converted.  

***API:*** Instructions to programmatically access the online service are available at https://neuromorpho.org/xyz2swc/docs.

---
<br/>


## Running *xyz2swc* locally

**NOTE:** Local installation of `xyz2swc` requires considerable experience and effort, and is therefore recommended only for those who might want to go beyond simply using the software (interested developers and code contributors).

### *System Requirements*
- Hardware: A standard computer with enough RAM to support the in-memory operations.
- Operating System: The package is currently only supported for Linux and has been tested on Ubuntu 18.04 LTS. 

Refer to the detailed installation instructions below for other specific software requirements for each of the *xyz2swc* modules.

### *Option 1: Local installation using Docker*

If you need to install and run the service locally (e.g., on a private server) we recommend making use of the published Docker image https://hub.docker.com/r/neuromorpho/xyz2swc - which which contains the latest stable version of the source code, libraries, modules, and all other needed dependencies.

- Clone this repository: `$ git clone https://github.com/NeuroMorpho/xyz2swc.git`
- Install [docker](https://docs.docker.com/get-docker/)
- Download MATLAB Runtime from [here](https://www.mathworks.com/products/compiler/matlab-runtime.html) and save into the file lib/matlab_rt.zip
- Download Fiji from [here](https://downloads.imagej.net/fiji/latest/fiji-linux64.zip) and save into the file modules/snt/fiji-linux64.zip
- Build the docker image using docker compose: `$ docker compose up`
- Wait for build to complete, this usually takes 5-10 minutes
- Once build is completed and docker image is deployed, UI will be accessible at port 8001 of localhost: http://localhost:8001

**Total estimated time for local installation using Docker:** 15-30 minutes, excluding download time for MATLAB Runtime and FIJI.

### *Option 2: Local installation from scratch*

1. **Install Neuronland module:** Detailed install instructions for compiling and building the Neuronland app using `g++/gcc` are available [here](https://github.com/NeuroMorpho/xyz2swc/tree/main/modules/neuronland/release_docs).

2. **Install MATLAB Runtime Compiler:** Conversion of NeuronJ `.ndf`, and TreesToolBox `.mtr` format files require the MATLAB runtime compiler (mcr). Instructions for installing and deploying the stand alone MATLAB application can be found [here](https://github.com/NeuroMorpho/xyz2swc/tree/main/modules/ndf/release_docs/Install_Instructions_Matlab.md). 

3. **Install R:** Conversion of Amira `.am` format files require the use of R packages and libraries. Install instructions can be found [here](https://github.com/NeuroMorpho/xyz2swc/tree/main/modules/am/release_docs/Install_Instructions_R.md).

4. **Setting up FIJI:** Conversion of SNT `.traces` format files require the FIJI (FIJI Is Just ImageJ) application. Instructions to download and setup the portable version of FIJI can be found [here](https://github.com/NeuroMorpho/xyz2swc/tree/main/modules/snt/release_docs/Install_Instructions_FIJI.md).

5. **Setting up HBP Morphology Viewer:**
The HBP converter module requires Node.js and npm: 
`$ apt-get -y install nodejs npm`

6. **Install Python:** 
To install the latest version of Python:
`$ sudo apt install python3`
Use pip3 to install the necessary package modules:
`$ sudo apt install python3-pip`
`$ pip3 install os pdb numpy pandas sys math statistics`

### Demo 
Navigate to the `xyz2swc` folder and run the converter by executing the python script:
`$ cd ./xyz2swc` 
`$ python convert.py`
 
- No optional arguments need to be specified.
- The program automatically searches and imports the reconstruction files from `./input/to_convert/` folder. Demo sample file are provided in this folder.
- On successful conversion the SWC files are saved into the `./output/converted` folder. The folder also contains a `converted_checklist.csv` for a quick inspection of the conversion status of each file.

### Instructions for use
Simply replace the demo example files in the `./input/to_convert/` folder with the files you want to convert (or standardize), and execute the `convert.py` script as described in the [Demo](#Demo) above. 

---
