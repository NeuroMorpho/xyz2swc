# xyz2swc Supported Formats


| **Application**       | **Format** | **Status** | **Mass Validation** |
|:---------------------:|:----------:|:----------:|:-------------------:|
|                       |            |            |                     |
| Neurolucida           | .dat       | ✔️          |  ✔️                  |
|                       | .nrx       | ✔️          |                     |
|                       | .asc       | ✔️          |  ✔️                  |
|                       | .xml       | ✔️          |                     |
|                       |            |            |                     |
| NeuronJ               | .ndf       | ✔️          |  -                  |
|                       |            |            |                     |
| IMARIS                | .ims       | ✔️          |                     |
|                       | .imx       | ❌          |                     |
|                       | .iv        | ❌          |                     |
|                       |            |            |                     |
| IMARIS                | .hoc       | ✔️          |                     |
|                       |            |            |                     |
| Amira                 | .am        | ✔️          |  -                  |
|                       |            |            |                     |
| Knossos               | .nml       | ✔️          |                     |
|                       | .nmx       | ✔️          |                     |
|                       |            |            |                     |
| Simple Neurite Tracer | .traces    | ✔️          |                     |
|                       |            |            |                     |
| Trees Toolbox         | .mtr       | ✔️          |  ✔️                  |
| Trees Toolbox         | .mat       | ✔️          |  ✔️                  |
|                       |            |            |                     |
| Vaa3D                 | .eswc      | ✔️          |                     |
| NeuTube               | .eswc      | ✔️          |                     |
|                       |            |            |                     |
| Arbor                 | .ntr       | ✔️          |                     |

<!-- | Eutectic              | .hoc       | -          |                     |
| Neurolucida           | .hoc       | -          |                     | -->

<br/>
<br/>
<br/>

# Mass Validation

## Neurolucida .asc files

Total: 19628  
Failed Download: 0  
Failed Conversion: 162  
Failed Standardization: 0  

<br/>

## Custom .am files (FlyCircuit)

Total: 16048  
Failed Download: 0  
Failed Conversion: 0  
Failed Standardization: 0  

<br/>

## Amira .am files (FlyCircuit)

Total: 488  
Failed Download: 3  

| NeuronID          | Name                          | Archive        |
|-------------------|-------------------------------|----------------|
| 104489            | 040823_5_sn                   | namiki         |
| 111414            | protocerebrum_extension_left  | beer_foerster  |
| 124613            | Crisia1_Tub2-labels2a         | temereva       |

Failed Conversion: 137  
Failed Standardization: 4  



<br/>

## NeuronJ .ndf files (FlyCircuit)

Total: 7844  
Failed Download: 19  
Failed Conversion: 176  
Failed Standardization: 0  

<br/>
<br/>
<br/>

# Archived (Code has substantially changed since these validations)

## Neurolucida .dat files

Total: 58520  
Failed Download: 5613  
Failed Conversion: 1246  
Failed Standardization: 0  
*Note: Added single contour conversion.*


<br/>

## TressToolBox .mtr files

Total: 591  
Failed Download: 331  
Failed Conversion: 2  
Failed Standardization: 25  
*Note: Failed Standardization was because `Index` order was not sorted.*

The format of the original reconstruction is not .mtr

| Archive           | No. of Files | Format  |
|-------------------|--------------|---------|
| Beining           | 240          | .swc    |
| Moons             | 91           | .mat    |


<br/>

## TressToolBox .mat files

Total: 871  
Failed Download: 0  
Failed Conversion: 110  
Failed Standardization: 0  
*Note: Failed conversion were all files of the `Moons` archive - unable to detect tree structure in .mat files.*


<br/>

## Neurolucida .swc files

The format of the original reconstruction is not .swc

| Archive           | No. of Files | Format  |
|-------------------|--------------|---------|
| Feller            | 5            | .traces |
| Vivinetto_Cave    | 157          | .traces |
| Zhong_Mao_Ganguli | 6            | .asc    |

The format of the original reconstruction is .swc, but the reconstruction software isn't Neurolucida. These errors are much harder to detect, and I haven't caught all of them.

| Archive      | No. of Files | Software Used  |
|--------------|--------------|----------------|
| Li_Bolshakov | 32           | Neuronstudio   |
| Yousheng     | 20           | Trees Tool Box |

A neuron [NeuroMorpho.Org ID:130581] in the Tolias archive has its original reconstruction missing.
