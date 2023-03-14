# `xyz2swc` - A Universal Conversion Service of Digital Reconstructions into the SWC Standard

###### tags: `xyz2swc` `documentation`
<br/>
<br/>


### 1. Install Neuronland module
Detailed install instructions for compiling and building the Neuronland app using `g++/gcc` are available [here](https://gitlab.orc.gmu.edu/nmo/xyz2swc/-/tree/main/modules/neuronland/release_docs).




---
<br/>

### 2. Install MATLAB Runtime Compiler
Conversion of NeuronJ `.ndf`, and TreesToolBox `.mtr` format files require the MATLAB runtime compiler (mcr). Instructions for installing and deploying the stand alone MATLAB application can be found [here](https://gitlab.orc.gmu.edu/nmo/xyz2swc/-/blob/main/modules/ndf/release_docs/Install_Instructions_Matlab.md). 



---
<br/>

### 3. Install R
Conversion of Amira `.am` format files require the use of R packages and libraries. Install instructions can be found [here](https://gitlab.orc.gmu.edu/nmo/xyz2swc/-/blob/main/modules/am/release_docs/Install_Instructions_R.md).




---
<br/>

### 4. Setting up FIJI
Conversion of SNT `.traces` format files require the FIJI (FIJI Is Just ImageJ) application. Instructions to download and setup the portable version of FIJI can be found [here](https://gitlab.orc.gmu.edu/nmo/xyz2swc/-/blob/main/modules/snt/release_docs/Install_Instructions_FIJI.md).



---
<br/>

### 5. Setting up Morph-Tool by BBP
` pip install morph-tool[all]`




---
<br/>

### 6. Run `xyz2swc` converter

Converter requires Python. To check (default) installed version.
```
$ python --version
```
To install latest version:
```
$ sudo apt install python3
```
Use pip3 to install needed package modules:
```
$ sudo apt install python3-pip
$ pip3 install os pdb numpy pandas sys math statistics
```
To run the converter:
```
$ cd xyz2swc
$ python xyz2swc.py
```

**Important:**
- No optional arguments need to be specified.
- The program automatically searches and imports the reconstruction files from `./input/to_convert/` folder.
- If you only want to check if a SWC file meets INCF standard specification, then place them in `./input/swc_check_only/` folder.
- On successful conversion the SWC files are saved into the `./output/converted` folder. The folder also contains a `converted_checklist.csv` for a quick inspection of the conversion status of each file.


---
<br/>
