
# Core Libraries
List of required packages for Fedora (or other RPM based distros like CentOS).  Preserve installation order to to ensure dependencies are met.
```
$ dnf install libgcc.*
$ dnf install libstdc++-*
$ dnf install glibc*.i686
$ dnf install glibc*.x86_64
$ dnf install zlib.i686 zlib-devel.i686
$ dnf install hdf5.i686 hdf5-devel.i686
```

  

# szip
Extract `./3rdParty/szip/szip-2.1.1.tar.gz' then install libraries. Ensure you build for 32-bit architecture.
```
$ ./configure --build=i686-pc-linux-gnu --host=i686-pc-linux-gnu CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32
$ make
$ make check
$ make install
```

  

# Build the project files
```
$ ./GenerateBuildFiles_gmake2.sh
$ cd solution_gmake2
$ make
```

  

# File Edits

#### Make Script
Added relative path in `./premake/generate_project_gmake2.sh`
```
#!/bin/bash

PREMAKE_EXE=./3rdParty/premake/premake5

$PREMAKE_EXE --verbose --file=./premake/workspace.lua gmake2
```

  

#### Imaris HDF Support

`./src/NeuronMorphologyFormatImportExportBuiltIn/FileFormatImarisHDF.cpp`

Comment lines #10,11,12,13,14,17,92,349
Edit lines #93,94: `const const` -> `const`

---

`./src/NeuronMorphologyFormatImportExportBuiltIn/hdf5_utils.h`

Comment lines #54,73 `extern`

---  

#### Lua Premake

Corrected LIB flags and added dirs  `./premake/App_NeuronMorphologyFormatConverter.lua`
```
libdirs
{
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
Added LIB dirs `./premake/NeuronMorphologyFormatImportExportBuiltIn.lua`
```
includedirs
{
	"/usr/include",
	"../3rdParty/szip/szip-2.1.1/szip/include",
	"../3rdParty/szip/szip-2.1.1/szip/lib"
}
```
