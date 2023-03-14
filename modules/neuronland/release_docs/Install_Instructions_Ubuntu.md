# Build Neuronland on Ubuntu 18.04 LTS

###### tags: `xyz2swc` `documentation`
<br/>


# Core Libraries

### 1. Enable Multi Arch support for 32-bit
```
$ sudo dpkg --add-architecture i386
$ sudo apt-get update
$ sudo apt-get upgrade
$ sudo apt-get dist-upgrade
```

<br/>

### 2. Install latest version of g++ and gcc.

To get the add-apt-repository command, install the software-properties-common package:

```
$ sudo apt update
$ sudo apt install software-properties-common
```

Add the PPA
```
$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test
$ sudo apt-get update
```

If you have trouble adding ppa:
```
$ sudo apt-get install --reinstall ca-certificates
$ sudo -E add-apt-repository ppa:ubuntu-toolchain-r/test
$ sudo apt update
```
Or try specifying full path:
```
$ sudo apt-add-repository "deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu bionic main"
$ sudo apt update
```

Install g++ and gcc (from ppa:toolchain that we just added).
Note: Try `C++11`, if that gives error, try `C++10`
```
$ sudo apt install gcc-11 g++-11 gcc-11-multilib g++-11-multilib
```

Make it the default by adding soft links:
```
$ sudo ln -s /usr/bin/gcc-11 /usr/bin/gcc
$ sudo ln -s /usr/bin/g++-11 /usr/bin/g++
```

If successful, then a quick version check via the terminal `$ g++ --version` and `$ gcc --version` should return gcc version 11.x.x.

<br/>

### 3. Install required packages.

Necessary dev tools and libraries. Preserve installation order to to ensure dependencies are met.
```
$ sudo apt-get install libc6-dev-i386
$ sudo apt-get install libsz2:i386
$ sudo apt-get install zlib1g:i386 zlib1g-dev:i386
$ sudo apt-get install libhdf5-100:i386 libhdf5-cpp-100:i386 libhdf5-dev:i386
$ sudo apt-get install libdlib18:i386 libdlib-dev:i386
$ sudo apt-get install make:i386
```


<br/><br/>
# szip
Extract `./3rdParty/szip/szip-2.1.1.tar.gz` then install libraries. Ensure you build for 32-bit architecture.
```
$ ./configure --build=i686-pc-linux-gnu --host=i686-pc-linux-gnu CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32
$ make
$ make check
$ make install
```

Note: You might need to create a soft link to help the linker `ld` find the installed HDF5 library. 
```
ln -s (source file path: where libhdf5.so resides) (new link path: /usr/lib/libhdf5.so)
```
For exmaple,
```
ln -s /usr/lib/i386-linux-gnu/hdf5/serial/libhdf5.so /usr/lib/libhdf5.so
```
The library paths must also be correctly specified in the [Lua premake files](#Lua-Premake).

<br/><br/>
# Build Project Files
```
$ ./GenerateBuildFiles_gmake2.sh
$ cd solution_gmake2
$ make
```

<br/><br/>
# Lua Premake

Some file edits **may** be required to ensure that the library paths are correct.


---

LIB-flags and LIB-dirs in `./premake/App_NeuronMorphologyFormatConverter.lua`
```
libdirs
{
	"/usr/lib/i386-linux-gnu/hdf5/serial",           -- Ubuntu hdf5 paths
	"/usr/lib/i386-linux-gnu/hdf5/serial/include",   -- Ubuntu hdf5 paths
	"/usr/include",
	"../3rdParty/szip/szip-2.1.1/szip/include",
	"../3rdParty/szip/szip-2.1.1/szip/lib"
}

links
{
	-- HDF5 (static libs)
	"hdf5",
	"hdf5_cpp",
	"dl",
	"z",
	"sz",
	-- TinyXml (linked project)
	"TinyXml"
}
```

---
LIB-dirs in `./premake/NeuronMorphologyFormatImportExportBuiltIn.lua`
```
libdirs
{
	"/usr/lib/i386-linux-gnu/hdf5/serial",           -- Ubuntu hdf5 paths
	"/usr/lib/i386-linux-gnu/hdf5/serial/include",   -- Ubuntu hdf5 paths
	"/usr/include",
	"../3rdParty/szip/szip-2.1.1/szip/include",
	"../3rdParty/szip/szip-2.1.1/szip/lib"
}
```
