# Install R and Required Packages on Ubuntu 18.04 LTS

###### tags: `xyz2swc` `documentation` `R`
<br/>

## 1. Install R version 4.0.0

**Add the relevant GPG key.**
```
$ sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E298A3A825C0D65DFD57CBB651716619E084DAB9
```

**Add the R Repository**
```
$ sudo add-apt-repository 'deb https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/'
$ sudo apt update
```
Note: If not using Ubuntu 18.04, find the relevant repository from the [R Project Ubuntu list](https://cloud.r-project.org/bin/linux/ubuntu/fullREADME.html).

**Install R**
```
$ sudo apt install r-base
```

---
<br/>
 
## 2. Install nat
The required R packages may then be installed from the CRAN source packages, typically from inside R.

Start R bysimply executing the`$ R` command as a local user, and then:
```
> install.packages("nat", dependencies=TRUE)
```

<br/>

**Notes:**
- If you encounter problems during runtime, then you might want to install the development version of the package.
`> install.packages("natmanager")`
`> natmanager::install('natverse')`
- You might want to install these as a local user to ensure that the default library paths set automatially.
- Requires R version >=3.6.


