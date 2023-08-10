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
|  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- |
| **Software Application** | **File Format** | **Test Data Source** | **Converter Module(s)** | **Programming Language(s)** | **No. of Variations Supported** |
| Amira<sup>7,56</sup> *(ThermoFisher, RRID:SCR\_007353)* | .am | Neuromorpho.Org | natverse<sup>28</sup> | R | 3 |
| Arbor<sup>44</sup> | .swc | Neuromorpho.Org | NeuronLand | C++ | 1 |
| ESWC<sup>35</sup> | .eswc | Neuromorpho.Org | Custom | Python | 2 |
| Eutectics<sup>9</sup> | .nts | Neuromorpho.Org | NeuronLand; Custom | C++; Python | 4 |
| Genesis<sup>17</sup> | .p | senselab.med.yale.edu/ModelDB/ | NeuronLand | C++ | 2 |
| HBP Morphology Viewer SWC+<sup>13</sup> | .swc | Neuromorpho.Org | Custom | Python | 1 |
| Imaris *(Oxford Instruments,  RRID:SCR\_007370)* | .ims | Neuromorpho.Org | NeuronLand (HDF5 Library) | C++ | 1 |
| KNOSSOS<sup>8</sup> | .nml\* | Neuromorpho.Org | Custom | Python | 1 |
| Neuroglancer | .stl .obj .ply  | http://fafb-ffn1.storage.googleapis.com/data.html | skeletor<sup>57</sup> | Python | 4# |
| Neurolucida<sup>6,20</sup> | .asc | Neuromorpho.Org | NeuronLand;HBP13<sup>†</sup> | C++; Node.js | 7 |
| Neurolucida | .dat | Neuromorpho.Org | NeuronLand; HBP<sup>†</sup> | C++; Node.js | 3 |
| Neurolucida | .nrx | Neuromorpho.Org | NeuronLand; HBP<sup>†</sup> | C++; Node.js | 1 |
| Neurolucida | .xml | Neuromorpho.Org | NeuronLand; HBP<sup>†</sup> | C++; Node.js | 1 |
| NeuroML<sup>21,22</sup> | .nml\*\* | Neuromorpho.Org | NeuronLand; Custom | C++; Python | 15 |
| NeuronJ<sup>12</sup> | .ndf | Neuromorpho.Org | Bonfire31<sup>†</sup> | Octave | 4 |
| NEURON<sup>15,16</sup> | .hoc | Neuromorpho.Org | Custom | Python | 11 |
| NeuroZoom<sup>58</sup> | .swc | Neuromorpho.Org | NeuronLand | C++ | 2 |
| NINDS3D<sup>59</sup> | .anat | Neuromorpho.Org | NeuronLand | C++ | 1 |
| PSICS<sup>43</sup> *(RRID: SCR\_014159)* | .xml | psics.org/examples.html | NeuronLand | C++ | 1 |
| PyKNOSSOS<sup>55</sup> | .nmx | Neuromorpho.Org | Custom | Python | 1 |
| SNT TRACES<sup>10,11</sup> | .traces | Neuromorpho.Org | FIJI<sup>29</sup> (SNT plugin<sup>11</sup>); Custom | Java; Python | 2 |
| TREES Toolbox<sup>14</sup> | .mtr | Neuromorpho.Org | TREES Toolbox<sup>†</sup> | Octave | 1 |
| TREES Toolbox | .mat | Neuromorpho.Org | TREES Toolbox<sup>†</sup> | Octave | 2 |
| Visualization Toolkit<sup>60</sup> | .vtk | Neuromorpho.Org | skeletor<sup>†</sup> | Python | 2 |
| \*KNOSSOS .nml format while being an XML file is not compliant with the NeuroML .nml format. \*\*NeuroML recommends using .cell.nml for NeuroML v2 cell files22, and .nml1 for NeuroML v1 files<sup>21,23</sup>. <sup>†</sup>Customized implementation. #Supports triangle mesh formats (trimsh.org/index.html). ||||||

## References
1.	Donohue, D. E. & Ascoli, G. A. Automated Reconstruction of Neuronal Morphology: An Overview. Brain Res. Rev. 67, 94–102 (2011).
2.	Halavi, M., Hamilton, K. A., Parekh, R. & Ascoli, G. A. Digital Reconstructions of Neuronal Morphology: Three Decades of Research Trends. Front. Neurosci. 6, 49 (2012).
3.	Parekh, R. & Ascoli, G. A. Quantitative investigations of axonal and dendritic arbors: development, structure, function, and pathology. Neurosci. Rev. J. Bringing Neurobiol. Neurol. Psychiatry 21, 241–254 (2015).
4.	Chavlis, S. & Poirazi, P. Modeling Dendrites and Spatially-Distributed Neuronal Membrane Properties. Adv. Exp. Med. Biol. 1359, 25–67 (2022).
5.	Stepanyants, A. & Chklovskii, D. B. Neurogeometry and potential synaptic connectivity. Trends Neurosci. 28, 387–394 (2005).
6.	Glaser, J. R. & Glaser, E. M. Neuron imaging with neurolucida — A PC-based system for image combining microscopy. Comput. Med. Imaging Graph. 14, 307–317 (1990).
7.	Stalling, D., Westerhoff, M. & Hege, H.-C. Amira: A Highly Interactive System for Visual Data Analysis. in 749–767 (Elsevier, 2005). doi:10.1016/B978-012387582-2/50040-X.
8.	Helmstaedter, M., Briggman, K. L. & Denk, W. High-accuracy neurite reconstruction for high-throughput neuroanatomy. Nat. Neurosci. 14, 1081–1088 (2011).
9.	Capowski, J. J. An automatic neuron reconstruction system. J. Neurosci. Methods 8, 353–364 (1983).
10.	Ferreira, T. A. et al. Neuronal morphometry directly from bitmap images. Nat. Methods 11, 982–984 (2014).
11.	Arshadi, C., Günther, U., Eddison, M., Harrington, K. I. S. & Ferreira, T. A. SNT: a unifying toolbox for quantification of neuronal anatomy. Nat. Methods 18, 374–377 (2021).
12.	Meijering, E. et al. Design and validation of a tool for neurite tracing and analysis in fluorescence microscopy images. Cytometry A 58A, 167–176 (2004).
13.	Bakker, R. & Tiesinga, P. Web-based neuron morphology viewer as an aid to develop new standards for neuron morphology file formats. Front. Neuroinformatics 10, (2016).
14.	Cuntz, H., Forstner, F., Borst, A. & Häusser, M. The TREES Toolbox—Probing the Basis of Axonal and Dendritic Branching. Neuroinformatics 9, 91–96 (2011).
15.	Carnevale, N. T. & Hines, M. L. The NEURON Book. (Cambridge University Press, 2006).
16.	Hines, M. L. & Carnevale, N. T. The NEURON simulation environment. Neural Comput. 9, 1179–1209 (1997).
17.	Bower, J. M. & Beeman, D. The Book of GENESIS: Exploring Realistic Neural Models with the GEneral NEural SImulation System. (Springer Science & Business Media, 2012).
18.	Wilkinson, M. D. et al. The FAIR Guiding Principles for scientific data management and stewardship. Sci. Data 3, 160018 (2016).
19.	Poline, J.-B. et al. Is Neuroscience FAIR? A Call for Collaborative Standardisation of Neuroscience Data. Neuroinformatics 20, 507–512 (2022).
20.	Sullivan, A. E. et al. A Comprehensive, FAIR File Format for Neuroanatomical Structure Modeling. Neuroinformatics 20, 221–240 (2022).
21.	Gleeson, P. et al. NeuroML: A Language for Describing Data Driven Models of Neurons and Networks with a High Degree of Biological Detail. PLOS Comput. Biol. 6, e1000815 (2010).
22.	Cannon, R. C. et al. LEMS: a language for expressing complex biological models in concise and hierarchical form and its use in underpinning NeuroML 2. Front. Neuroinformatics 8, (2014).
23.	Crook, S., Gleeson, P., Howell, F., Svitak, J. & Silver, R. A. MorphML: Level 1 of the NeuroML Standards for Neuronal Morphology Data and Model Specification. Neuroinformatics 5, 96–104 (2007).
24.	Cannon, R. C., Turner, D. A., Pyapali, G. K. & Wheal, H. V. An on-line archive of reconstructed hippocampal neurons. J. Neurosci. Methods 84, 49–54 (1998).
25.	Parekh, R. & Ascoli, G. A. Neuronal Morphology Goes Digital: A Research Hub for Cellular and System Neuroscience. Neuron 77, 1017–1038 (2013).
26.	Ascoli, G. A., Krichmar, J. L., Nasuto, S. J. & Senft, S. L. Generation, description and storage of dendritic morphology data. Philos. Trans. R. Soc. Lond. B. Biol. Sci. 356, 1131–1145 (2001).
27.	Akram, M. A., Nanda, S., Maraver, P., Armañanzas, R. & Ascoli, G. A. An open repository for single-cell reconstructions of the brain forest. Sci. Data 5, 180006 (2018).
28.	Bates, A. S. et al. The natverse, a versatile toolbox for combining and analysing neuroanatomical data. eLife 9, e53350 (2020).
29.	Schindelin, J. et al. Fiji: an open-source platform for biological-image analysis. Nat. Methods 9, 676–682 (2012).
30.	O’Halloran, D. M. Module for SWC neuron morphology file validation and correction enabled for high throughput batch processing. PLoS ONE 15, e0228091 (2020).
31.	Langhammer, C. G. et al. Automated Sholl analysis of digitized neuronal morphology at multiple scales: Whole-cell Sholl analysis vs. Sholl analysis of arbor sub-regions. Cytom. Part J. Int. Soc. Anal. Cytol. 77, 1160–1168 (2010).
32.	Ascoli, G. A. Cell morphologies in the nervous system: Glia steal the limelight. J. Comp. Neurol. 531, 338–343 (2023).
33.	Colombo, G. et al. A tool for mapping microglial morphology, morphOMICs, reveals brain-region and sex-dependent phenotypes. Nat. Neurosci. 25, 1379–1393 (2022).
34.	Migliore, M., Ferrante, M. & Ascoli, G. A. Signal Propagation in Oblique Dendrites of CA1 Pyramidal Cells. J. Neurophysiol. 94, 4145–4155 (2005).
35.	Nanda, S. et al. Design and implementation of multi-signal and time-varying neural reconstructions. Sci. Data 5, 170207 (2018).
36.	Parekh, R., Armañanzas, R. & Ascoli, G. A. The importance of metadata to assess information content in digital reconstructions of neuronal morphology. Cell Tissue Res. 360, 121–127 (2015).
37.	Bijari, K., Akram, M. A. & Ascoli, G. A. An open-source framework for neuroscience metadata management applied to digital reconstructions of neuronal morphology. Brain Inform. 7, 2 (2020).
38.	Bijari, K., Zoubi, Y. & Ascoli, G. A. Assisted neuroscience knowledge extraction via machine learning applied to neural reconstruction metadata on NeuroMorpho.Org. Brain Inform. 9, 26 (2022).
39.	Scheffer, L. K. et al. A connectome and analysis of the adult Drosophila central brain. eLife 9, e57443 (2020).
40.	Eckstein, N. et al. Neurotransmitter Classification from Electron Microscopy Images at Synaptic Sites in Drosophila Melanogaster. Preprint at https://www.biorxiv.org/content/10.1101/2020.06.12.148775v3 (2023).
41.	Dorkenwald, S. et al. FlyWire: online community for whole-brain connectomics. Nat. Methods 19, 119–128 (2022).
42.	Plaza, S. M. et al. neuPrint: An open access tool for EM connectomics. Front. Neuroinformatics 16, (2022).
43.	Cannon, R. C., O’Donnell, C. & Nolan, M. F. Stochastic Ion Channel Gating in Dendritic Neurons: Morphology Dependence and Probabilistic Synaptic Activation of Dendritic Spikes. PLOS Comput. Biol. 6, e1000886 (2010).
44.	Mátyás, F., Freund, T. F. & Gulyás, A. I. Convergence of excitatory and inhibitory inputs onto CCK-containing basket cells in the CA1 area of the rat hippocampus. Eur. J. Neurosci. 19, 1243–1256 (2004).
45.	Carnevale, N. T., Tsai, K. Y., Claiborne, B. J. & Brown, T. H. Comparative electrotonic analysis of three classes of rat hippocampal neurons. J. Neurophysiol. 78, 703–720 (1997).
46.	Halavi, M. et al. NeuroMorpho.Org Implementation of Digital Neuroscience: Dense Coverage and Integration with the NIF. Neuroinformatics 6, 241–252 (2008).
47.	Goldberg, I. G. et al. The Open Microscopy Environment (OME) Data Model and XML file: open tools for informatics and quantitative analysis in biological imaging. Genome Biol. 6, R47 (2005).
48.	Keating, S. M. et al. SBML Level 3: an extensible format for the exchange and reuse of biological models. Mol. Syst. Biol. 16, e9110 (2020).
49.	Gleeson, P., Davison, A. P., Silver, R. A. & Ascoli, G. A. A Commitment to Open Source in Neuroscience. Neuron 96, 964–965 (2017).
50.	Jiang, S. et al. Petabyte-Scale Multi-Morphometry of Single Neurons for Whole Brains. Neuroinformatics 20, 525–536 (2022).
51.	Liu, Y., Wang, G., Ascoli, G. A., Zhou, J. & Liu, L. Neuron tracing from light microscopy images: automation, deep learning and bench testing. Bioinformatics 38, 5329–5339 (2022).
52.	Winnubst, J. et al. Reconstruction of 1,000 Projection Neurons Reveals New Cell Types and Organization of Long-Range Connectivity in the Mouse Brain. Cell 179, 268-281.e13 (2019).
53.	Chiang, A.-S. et al. Three-Dimensional Reconstruction of Brain-wide Wiring Networks in Drosophila at Single-Cell Resolution. Curr. Biol. 21, 1–11 (2011).
54.	Ascoli, G. A., Maraver, P., Nanda, S., Polavaram, S. & Armañanzas, R. Win–win data sharing in neuroscience. Nat. Methods 14, 112–116 (2017).
55.	Wanner, A. A., Genoud, C. & Friedrich, R. W. 3-dimensional electron microscopic imaging of the zebrafish olfactory bulb and dense reconstruction of neurons. Sci. Data 3, 160100 (2016).
56.	Dercksen, V. J., Hege, H.-C. & Oberlaender, M. The Filament Editor: An Interactive Software Environment for Visualization, Proof-Editing and Analysis of 3D Neuron Morphology. Neuroinformatics 12, 325–339 (2014).
57.	Schlegel, P. & Kazimiers, T. schlegelp/skeletor: Version 1.1.0. (2021) doi:10.5281/zenodo.5138552.
58.	Nimchinsky, E. A., Hof, P. R., Young, W. G., Bloom, F. E. & Morrison, J. H. NeuroZoom software: Development, validation, and neurobiological applications. FASEB J. 12, (1998).
59.	Cullheim, S., Fleshman, J. W., Glenn, L. L. & Burke, R. E. Membrane area and dendritic structure in type-identified triceps surae alpha motoneurons. J. Comp. Neurol. 255, 68–81 (1987).
60.	Schroeder, W. J., Martin, K. W. & Lorensen, W. E. The Visualization Toolkit. (Kitware, 
---
