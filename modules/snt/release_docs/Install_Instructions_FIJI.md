# Install and Run FIJI on Ubuntu 18.04 LTS

###### tags: `xyz2swc` `documentation` `FIJI`
<br/>

## 1. [FIJI](https://imagej.net/software/fiji)

[Click here to download](https://downloads.imagej.net/fiji/archive/20201029-1752/fiji-linux64.zip) the 64-bit Linux portable version, dated 2020-10-29, of the FIJI application from the project website. Please note this version is tested and will work, newer versions may not work.


Unzip the download into the parent `snt` folder:
```
$ unzip fiji-linux64.zip -d ./modules/snt/
```
<br/>

This should result in the **following directory being created** `./modules/snt/Fiji.app`


---
<br/>
 
## 2. Install Java

FIJI scripting uses Jython - a Java implementation of Python.
(If you haven't already installed Python please see the main README file.)

For it's Java implementation FIJI requires at a minimum Java Runtime Environment (JRE) >=8.0. To mitigate any unexpected errors it is strongly encouraged to install the Java Development Kit (JDK) ver. 11.0.

```
$ sudo apt update
$ sudo apt install openjdk-11-jdk
```



