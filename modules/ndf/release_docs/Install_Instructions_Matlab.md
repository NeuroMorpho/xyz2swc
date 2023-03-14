# Deploy Standalone MATLAB Application

###### tags: `xyz2swc` `documentation` `MATLAB`
<br/>

## 1. Prerequisites for Deployment

[Click here to download](https://ssd.mathworks.com/supportfiles/downloads/R2021a/Release/6/deployment_files/installer/complete/glnxa64/MATLAB_Runtime_R2021a_Update_6_glnxa64.zip) the 64-bit Linux version of MATLAB Runtime for R2021a.

For information on how to install MATLAB Runtime (mcr) please see:
[https://www.mathworks.com/help/compiler/install-the-matlab-runtime.html](https://www.mathworks.com/help/compiler/install-the-matlab-runtime.html)

You can let the MATLAB Runtime installer choose the default installation folder or specify it using the `-destinationFolder` option. The destination folder is required during [Step 3. Run](#3-Run).

Note: A complete list of all available mcr versions can be found on the MathWorks Compiler Products [webpage](https://www.mathworks.com/products/compiler/mcr/index.html).



    



<br/>

**Optional Optimization (Not Tested Yet):** 
To reduce the size of the Matlab Runtime installation you can specify as user options in `installer_input.txt`, only the list of required MCR products for this app.
*This list is subject to change as I add support for TreesToolBox `.mtr`*
```
%productNumber    productName
----------------------------------------------------------------------------------------
%35010            product.MATLAB_Runtime___Numerics
%35119            product.MATLAB_Runtime___Statistics_and_Machine_Learning_Toolbox_Addin
```
An [example installer control text file](https://github.com/guzman-raphael/matlab/blob/master/installer_input.txt).

---
<br/>
 
## 2. Files to Deploy
`ndf2swc` 
`run_ndf2swc.sh` (shell script for temporarily setting environment variables & executing the app)

---
<br/>

## 3. Run

To run the stand alone app just execute the shell script: 
`$ ./run_readNDF.sh <mcr_directory> <argument_list>`
       
<mcr_directory> is the directory where version 9.10 of the MATLAB Runtime is installed.
<argument_list> is all the arguments you want to pass to your application.

<mark> **Note:**</mark> The <mcr_directory> path will have to be updated in the python wrapper script.

---
<br/>

<!-- ## 4. Appendix (Only if Needed)

In the following directions, replace `MR/v910` by the directory on the target machine where MATLAB is installed, or MR by the directory where the MATLAB Runtime is installed.

If the environment variable LD_LIBRARY_PATH is undefined, set it to the following:

`MR/v910/runtime/glnxa64:MR/v910/bin/glnxa64:MR/v910/sys/os/glnxa64:MR/v910/sys/opengl/lib/glnxa64`

If it is defined, set it to the following:

`${LD_LIBRARY_PATH}:MR/v910/runtime/glnxa64:MR/v910/bin/glnxa64:MR/v910/sys/os/glnxa64:MR/v910/sys/opengl/lib/glnxa64` -->
