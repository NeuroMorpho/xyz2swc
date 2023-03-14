
project "NeuronMorphologyFormatImportExportBuiltIn"
	neuronland_proj_lib()

	includedirs
	{
		"/usr/lib/i386-linux-gnu/hdf5/serial",
		"/usr/lib/i386-linux-gnu/hdf5/serial/include",
		"/usr/include",
		"../3rdParty/szip/szip-2.1.1/szip/include",
		"../3rdParty/szip/szip-2.1.1/szip/lib"
	}

	files
	{
		"../src/NeuronMorphologyFormatImportExportBuiltIn/*.cpp",
		"../src/NeuronMorphologyFormatImportExportBuiltIn/*.h*"
	}
