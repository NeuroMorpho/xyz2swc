# *xyz2swc*

This repository contains the source code for the *xyz2swc* software --- a universal online service for converting digital reconstructions of neural morphologies into standardized SWC format.

---
<br/>

## Contents
[Running online](#Running-xyz2swc-online-Preferred-Method)  
[Running locally](#Running-xyz2swc-locally)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[System Requirements](#System-Requirements)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Installation using Docker](#Option-1-Local-installation-using-Docker)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Installation from scratch](#Option-2-Local-installation-from-scratch)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Instructions for use](#Instructions-for-use)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Module management](#Module-management)  
[Standardization checklist](#Standardization-checklist)  
[Supported tools and formats](#Supported-tools-and-formats)  

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

<br/>

### *Option 1: Local installation using Docker*

If you need to install and run the service locally (e.g., on a private server) we recommend making use of the published Docker image https://hub.docker.com/r/neuromorpho/xyz2swc - which which contains the latest stable version of the source code, libraries, modules, and all other needed dependencies. You can pull the docker image using the command below:

- `$ docker pull neuromorpho/xyz2swc:latest`

You can also build the docker image from scratch using the instructions below. Please note that the system is built and tested for Ubuntu Linux. Other platforms may need adaptions to work.

- Clone this repository: `$ git clone https://github.com/NeuroMorpho/xyz2swc.git`
- Install [docker](https://docs.docker.com/get-docker/)
- Build the docker image using docker compose: `$ docker compose up`
- Wait for build to complete, this usually takes 5-10 minutes
- Once build is completed and docker image is deployed, UI will be accessible at port 8001 of localhost: http://localhost:8001

**Total estimated time for local installation using Docker:** 15-30 minutes, excluding download time for FIJI.

<br/>

### *Option 2: Local installation from scratch*

- *Install Python:* ```$ apt install -y python3```
Use pip3 to install all necessary Python packages (listed [here](https://github.com/NeuroMorpho/xyz2swc/blob/main/requirements.txt)).
```$ apt install -y python3-pip```
```$ pip3 install <package-name>```


- *Install Neuronland module:* Detailed install instructions for compiling and building the Neuronland app using `g++/gcc` are available [here](https://github.com/NeuroMorpho/xyz2swc/tree/main/modules/neuronland/release_docs).
- *Install R:* Required only<sup>‡</sup> for converting Amira `.am` format files. Install instructions can be found [here](https://github.com/NeuroMorpho/xyz2swc/tree/main/modules/am/release_docs/Install_Instructions_R.md).

- *Setting up FIJI:* Required only<sup>‡</sup> for converting SNT `.traces` format files. Instructions to download and setup the portable version of FIJI can be found [here](https://github.com/NeuroMorpho/xyz2swc/tree/main/modules/snt/release_docs/Install_Instructions_FIJI.md).

- *Install Node.js:* ``` $ apt install -y nodejs npm ```
Required for the HBP converter module.


- *Install Octave:* ``` $ apt install -y octave ``` 
Required only<sup>‡</sup> for converting NeuronJ `.ndf`, and TREES Toolbox `.mat`,`.mtr` format files.

---
<sup>‡</sup> Installation required only for converting the specified format(s), and does not impact other *xyz2swc* operations.

<br/>

### *Instructions for use*

Navigate to the `xyz2swc` folder and run the converter by executing the python script:
`$ cd ./xyz2swc` 
`$ python convert.py`
 
- Simply replace the demo example files in the `./input/to_convert/` folder with the files you want to convert (or standardize)
- No optional arguments need to be specified.
- The program automatically searches and imports the reconstruction files from `./input/to_convert/` folder. Demo sample file are provided in this folder.
- On successful conversion the SWC files are saved into the `./output/converted/` folder. The folder also contains a `converted_checklist.csv` for a quick inspection of the conversion status of each file.

<br/>

### *Module management*
- Python converter modules are located in `./xyz2swc/utils/` folder.
- Non-python converter modules are located in `./modules/` folder.
- Use [`convert.single()`](https://github.com/NeuroMorpho/xyz2swc/blob/main/xyz2swc/convert.py#L23) for convenient top-level to include/exclude individual converter modules without impacting overall service operation.
- To add support for a new morphology format, it is recommend to create a new Python converter module and simply import it via `convert.single()`.
    - Details of how and where to import the new module can be found [here](https://github.com/NeuroMorpho/xyz2swc/blob/main/xyz2swc/convert.py#L185).
    - A reference template for the new module is provided [here](https://github.com/NeuroMorpho/xyz2swc/blob/main/xyz2swc/utils/template_module.py).





---
<br/>

## Standardization checklist
The following checks are done, both for checked files as well as for the converted files:

| **Check**  | **Action/Correction**  |
|---|---|
| Missing Field  | If the SWC points matrix does not have seven columns, then return an error. All further checks are omitted.  |
| Number of Lines  | Generate an error if no samples are detected. All further checks are omitted.  If fewer than 20 lines, generate a warning to check file integrity.   |
| Number of soma Samples  | Generate warning if no soma samples detected.  |
| Invalid Parent  | If the Parent points to an Index value that does not exist, then make the sample with the invalid Parent a root point, and generate a warning to check file integrity.  |
| Index/Parent Integer  | If Index and/or Parent are float-formatted integer (e.g., “1.00”), format them as integers. If they are non-integer values (e.g., “1.34”) or non-numerical entries (e.g, “abc”), generate an error.  |
| XYZ Double  | Ensure X, Y, and Z coordinates are float/double values. Any NaN or N/A values detected in the ASCII text file are treated as 0.0. Generate a warning to check file integrity, and add a footer to the file to note inserted values. |
| Radius Positive Double  | Ensure sample Radius is a double/float value. If radius is negative, zero, NaN, or N/A, then set to 0.5. Generate a warning to check file integrity, and add a footer to the file to note inserted values. |
| Non-Standard Type  | If Type is float-formatted integer, format as integer. If it is non-integer value or non-numerical entry, change to Type 0 indicating 'undefined'. If bifurcation and terminal points have non-standard Types, set them to that of parent.  |
| Sequential Index  | If the Index values are not in sequential order (starting from 1), then sort and reset Index and Parent numbering. |
| Sorted Order  | If parent samples are referred to before being defined, then sort and reset Index and Parent numbering. Sort indices to ensure that the first sample in the file is a root point. If no sample point is a root, generate an error.  |
| Soma Contours  | Detect soma contour(s), and replace each with a single point. |



---
<br/>

## Supported tools and formats
| **Software Application** | **File Format** | **Test Data Source** | **Converter Module(s)** | **Programming Language(s)** | **No. of Variations Supported** |
| --- | --- | --- | --- | --- | --- |
| Amira<sup>2,22</sup> *(ThermoFisher, RRID:SCR\_007353)* | .am | Neuromorpho.Org | natverse<sup>16</sup> | R | 3 |
| Arbor<sup>20</sup> | .swc | Neuromorpho.Org | NeuronLand | C++ | 1 |
| ESWC<sup>18</sup> | .eswc | Neuromorpho.Org | Custom | Python | 2 |
| Eutectics<sup>4</sup> | .nts | Neuromorpho.Org | NeuronLand; Custom | C++; Python | 4 |
| Genesis<sup>12</sup> | .p | senselab.med.yale.edu/ModelDB/ | NeuronLand | C++ | 2 |
| HBP Morphology Viewer SWC+<sup>8</sup> | .swc | Neuromorpho.Org | Custom | Python | 1 |
| Imaris *(Oxford Instruments,  RRID:SCR\_007370)* | .ims | Neuromorpho.Org | NeuronLand (HDF5 Library) | C++ | 1 |
| KNOSSOS<sup>3</sup> | .nml\* | Neuromorpho.Org | Custom | Python | 1 |
| Neuroglancer  (RRID:SCR_015631) | .stl .obj .ply | http://fafb-ffn1.storage.googleapis.com/data.html | skeletor<sup>23</sup> | Python | 3<sup>#</sup> |
| Neurolucida<sup>1,13</sup> | .asc | Neuromorpho.Org | NeuronLand;HBP<sup>8,†</sup> | C++; Node.js | 7 |
| Neurolucida | .dat | Neuromorpho.Org | NeuronLand; HBP<sup>†</sup> | C++; Node.js | 3 |
| Neurolucida | .nrx | Neuromorpho.Org | NeuronLand; HBP<sup>†</sup> | C++; Node.js | 1 |
| Neurolucida | .xml | Neuromorpho.Org | NeuronLand; HBP<sup>†</sup> | C++; Node.js | 1 |
| NeuroML<sup>14,15</sup> | .nml\*\* | Neuromorpho.Org | NeuronLand; Custom | C++; Python | 15 |
| NeuronJ<sup>7</sup> | .ndf | Neuromorpho.Org | Bonfire<sup>28,†</sup> | Octave | 4 |
| NEURON<sup>10,11</sup> | .hoc | Neuromorpho.Org | Custom | Python | 11 |
| NeuroZoom<sup>24</sup> | .swc | Neuromorpho.Org | NeuronLand | C++ | 2 |
| NINDS3D<sup>25</sup> | .anat | Neuromorpho.Org | NeuronLand | C++ | 1 |
| PSICS<sup>19</sup> *(RRID: SCR\_014159)* | .xml | psics.org/examples.html | NeuronLand | C++ | 1 |
| PyKNOSSOS<sup>21</sup> | .nmx | Neuromorpho.Org | Custom | Python | 1 |
| SNT TRACES<sup>5,6</sup> | .traces | Neuromorpho.Org | FIJI<sup>17</sup> (SNT plugin<sup>11</sup>); Custom | Java; Python | 2 |
| TREES Toolbox<sup>9</sup> | .mtr | Neuromorpho.Org | TREES Toolbox<sup>†</sup> | Octave | 1 |
| TREES Toolbox | .mat | Neuromorpho.Org | TREES Toolbox<sup>†</sup> | Octave | 2 |
| Visualization Toolkit<sup>26</sup> | .vtk | Neuromorpho.Org | skeletor<sup>†</sup> | Python | 2 |

\*KNOSSOS .nml format while being an XML file is not compliant with the NeuroML .nml format. \*\*NeuroML recommends using .cell.nml for NeuroML v2 cell files<sup>15</sup>, and .nml1 for NeuroML v1 files<sup>14,27</sup>. <sup>†</sup>Customized implementation. <sup>#</sup>Supports triangle mesh formats (trimsh.org/index.html).

## References
1.	Glaser, J. R. & Glaser, E. M. Neuron imaging with neurolucida — A PC-based system for image combining microscopy. Comput. Med. Imaging Graph. 14, 307–317 (1990).
2.	Stalling, D., Westerhoff, M. & Hege, H.-C. Amira: A Highly Interactive System for Visual Data Analysis. in 749–767 (Elsevier, 2005). doi:10.1016/B978-012387582-2/50040-X.
3.	Helmstaedter, M., Briggman, K. L. & Denk, W. High-accuracy neurite reconstruction for high-throughput neuroanatomy. Nat. Neurosci. 14, 1081–1088 (2011).
4.	Capowski, J. J. An automatic neuron reconstruction system. J. Neurosci. Methods 8, 353–364 (1983).
5.	Ferreira, T. A. et al. Neuronal morphometry directly from bitmap images. Nat. Methods 11, 982–984 (2014).
6.	Arshadi, C., Günther, U., Eddison, M., Harrington, K. I. S. & Ferreira, T. A. SNT: a unifying toolbox for quantification of neuronal anatomy. Nat. Methods 18, 374–377 (2021).
7.	Meijering, E. et al. Design and validation of a tool for neurite tracing and analysis in fluorescence microscopy images. Cytometry A 58A, 167–176 (2004).
8.	Bakker, R. & Tiesinga, P. Web-based neuron morphology viewer as an aid to develop new standards for neuron morphology file formats. Front. Neuroinformatics 10, (2016).
9.	Cuntz, H., Forstner, F., Borst, A. & Häusser, M. The TREES Toolbox—Probing the Basis of Axonal and Dendritic Branching. Neuroinformatics 9, 91–96 (2011).
10.	Carnevale, N. T. & Hines, M. L. The NEURON Book. (Cambridge University Press, 2006).
11.	Hines, M. L. & Carnevale, N. T. The NEURON simulation environment. Neural Comput. 9, 1179–1209 (1997).
12.	Bower, J. M. & Beeman, D. The Book of GENESIS: Exploring Realistic Neural Models with the GEneral NEural SImulation System. (Springer Science & Business Media, 2012).
(2022).
13.	Sullivan, A. E. et al. A Comprehensive, FAIR File Format for Neuroanatomical Structure Modeling. Neuroinformatics 20, 221–240 (2022).
14.	Gleeson, P. et al. NeuroML: A Language for Describing Data Driven Models of Neurons and Networks with a High Degree of Biological Detail. PLOS Comput. Biol. 6, e1000815 (2010).
15.	Cannon, R. C. et al. LEMS: a language for expressing complex biological models in concise and hierarchical form and its use in underpinning NeuroML 2. Front. Neuroinformatics 8, (2014).
16.	Bates, A. S. et al. The natverse, a versatile toolbox for combining and analysing neuroanatomical data. eLife 9, e53350 (2020).
17.	Schindelin, J. et al. Fiji: an open-source platform for biological-image analysis. Nat. Methods 9, 676–682 (2012).
18.	Nanda, S. et al. Design and implementation of multi-signal and time-varying neural reconstructions. Sci. Data 5, 170207 (2018).
19.	Cannon, R. C., O’Donnell, C. & Nolan, M. F. Stochastic Ion Channel Gating in Dendritic Neurons: Morphology Dependence and Probabilistic Synaptic Activation of Dendritic Spikes. PLOS Comput. Biol. 6, e1000886 (2010).
20.	Mátyás, F., Freund, T. F. & Gulyás, A. I. Convergence of excitatory and inhibitory inputs onto CCK-containing basket cells in the CA1 area of the rat hippocampus. Eur. J. Neurosci. 19, 1243–1256 (2004).
21.	Wanner, A. A., Genoud, C. & Friedrich, R. W. 3-dimensional electron microscopic imaging of the zebrafish olfactory bulb and dense reconstruction of neurons. Sci. Data 3, 160100 (2016).
22.	Dercksen, V. J., Hege, H.-C. & Oberlaender, M. The Filament Editor: An Interactive Software Environment for Visualization, Proof-Editing and Analysis of 3D Neuron Morphology. Neuroinformatics 12, 325–339 (2014).
23.	Schlegel, P. & Kazimiers, T. schlegelp/skeletor: Version 1.1.0. (2021) doi:10.5281/zenodo.5138552.
24.	Nimchinsky, E. A., Hof, P. R., Young, W. G., Bloom, F. E. & Morrison, J. H. NeuroZoom software: Development, validation, and neurobiological applications. FASEB J. 12, (1998).
25.	Cullheim, S., Fleshman, J. W., Glenn, L. L. & Burke, R. E. Membrane area and dendritic structure in type-identified triceps surae alpha motoneurons. J. Comp. Neurol. 255, 68–81 (1987).
26.	Schroeder, W. J., Martin, K. W. & Lorensen, W. E. The Visualization Toolkit. (Kitware, 2006).
27.	Crook, S., Gleeson, P., Howell, F., Svitak, J. & Silver, R. A. MorphML: Level 1 of the NeuroML Standards for Neuronal Morphology Data and Model Specification. Neuroinformatics 5, 96–104 (2007).
28.	Langhammer, C. G. et al. Automated Sholl analysis of digitized neuronal morphology at multiple scales: Whole-cell Sholl analysis vs. Sholl analysis of arbor sub-regions. Cytom. Part J. Int. Soc. Anal. Cytol. 77, 1160–1168 (2010).

---
