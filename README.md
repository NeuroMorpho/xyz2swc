# *xyz2swc*

This repository contains the source code for the *xyz2swc* software --- a universal online service for converting digital reconstructions of neural morphologies into standardized SWC format.

---
<br/>


## Running *xyz2swc* online [Preferred Method] 

***Accessing the service:*** The *xyz2swc* software is available as a free and ready-to-use online service. It is accessible through any common internet browser via a user friendly web-based graphical interface at https://neuromorpho.org/xyz2swc/ui/. Demonstration video of the tool is available at https://vimeo.com/853219729?share=copy.

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

If you need to install and run the service locally (e.g., on a private server) we recommend making use of the published Docker image https://hub.docker.com/r/neuromorpho/xyz2swc - which which contains the latest stable version of the source code, libraries, modules, and all other needed dependencies. You can pull the docker image using the command below:

- `$ docker pull neuromorpho/xyz2swc:latest`

You can also build the docker image from scratch using the instructions below. Please note that the system is built and tested for Ubuntu Linux. Other platforms may need adaptions to work.

- Clone this repository: `$ git clone https://github.com/NeuroMorpho/xyz2swc.git`
- Install [docker](https://docs.docker.com/get-docker/)
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

The following checks are done, both for checked files as well as for the converted files:

| **Check**  | **Action/Correction**  |
|---|---|
| Missing Field  | If the SWC points matrix does not have seven columns, then return an error. All further checks are omitted.  |
| Number of Lines  | - Generate an error if no samples are detected. All further checks are omitted.  - If fewer than 20 lines, generate a warning to check file integrity.   |
| Number of soma Samples  | Generate warning if no soma samples detected.  |
| Invalid Parent  | If the Parent points to an Index value that does not exist, then make the sample with the invalid Parent a root point, and generate a warning to check file integrity.  |
| Index/Parent Integer  | If Index and/or Parent are float-formatted integer (e.g., “1.00”), format them as integers. If they are non-integer values (e.g., “1.34”) or non-numerical entries (e.g, “abc”), generate an error.  |
| XYZ Double  | Ensure X, Y, and Z coordinates are float/double values. Any NaN or NA values detected in the ASCII text file are treated as 0.0, and a warning is issued to check file integrity.   |
| Radius Positive Double  | - Ensure sample Radius is a double/float value.  - If radius is negative or zero, set to 0.5.  - Set any NaN or NA Radius values to 0.5 and generate a warning to check file integrity.  |
| Non-Standard Type  | - If Type is float-formatted integer, format as integer. If it is non-integer value or non-numerical entry, change to Type 6 indicating 'unspecified neurite'.  - If Type is 0 or an integer greater than 7, set to Type 6.  - If bifurcation and terminal points have non-standard Types, set them to that of parent.   |
| Sequential Index  | If the Index values are not in sequential order (starting from 1), then sort and reset Index and Parent numbering.   |
| Sorted Order  | - If parent samples are referred to before being defined, then sort and reset Index and Parent numbering.  - Sort indices to ensure that the first sample in the file is a root point. If no sample point is a root, generate an error.  |
| Soma Contours  | Detect soma contour(s), and replace each with a single point. |



### Instructions for use
Simply replace the demo example files in the `./input/to_convert/` folder with the files you want to convert (or standardize), and execute the `convert.py` script as described in the [Demo](#Demo) above. 

---

## Supported tools and formats

| **File Format** | **Converter Module(s)** | **Programming Language(s)** | **No. of Variations Supported** |
|---|---|---|---|
| Amira AM | natverse | R | 3 |
| Arbor | NeuronLand | C++ | 1 |
| ESWC | Custom | Python | 2 |
| Eutectics NTS | NeuronLand; Custom | C++; Python | 4 |
| Genesis | NeuronLand | C++ | 2 |
| Gulyas | NeuronLand | C++ | 1 |
| HBP Morphology Viewer SWC+ | Custom | Python | 1 |
| Imaris IMS | NeuronLand (HDF5 Library) | C++ | 1 |
| KNOSSOS NML | Custom | Python | 1 |
| Neurolucida ASC | NeuronLand; HBP (Customized) | C++; Node.js | 7 |
| Neurolucida DAT | NeuronLand; HBP (Customized) | C++; Node.js | 3 |
| Neurolucida NRX | NeuronLand; HBP (Customized) | C++; Node.js | 1 |
| Neurolucida XML | NeuronLand; HBP (Customized) | C++; Node.js | 1 |
| NeuroML | NeuronLand; Custom | C++; Python | 15 |
| NeuronJ NDF | Bonfire (Customized) | Matlab | 4 |
| Neuron HOC | Custom | Python | 11 |
| NeuroZoom | NeuronLand | C++ | 2 |
| NINDS3D | NeuronLand | C++ | 1 |
| PSICS | NeuronLand | C++ | 1 |
| PyKNOSSOS NMX | Custom | Python | 1 |
| SNT TRACES | FIJI (SNT plugin); Custom | Java; Python | 2 |
| TREES Toolbox MTR | TREES Toolbox (Customized) | Matlab | 1 |
| TREES Toolbox MAT | TREES Toolbox (Customized) | Matlab | 2 |


---
