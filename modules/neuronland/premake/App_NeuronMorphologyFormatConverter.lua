
project "NeuronMorphologyFormatConverter"
	neuronland_proj_app_console()

	files
	{
		"../src/App_NeuronMorphologyFormatConverter/*.cpp",
		"../src/App_NeuronMorphologyFormatConverter/*.h*"
	}

	libdirs
	{
		"/usr/lib/i386-linux-gnu/hdf5/serial",
		"/usr/lib/i386-linux-gnu/hdf5/serial/include",
		"/usr/include",
		"../3rdParty/szip/szip-2.1.1/szip/include",
		"../3rdParty/szip/szip-2.1.1/szip/lib"
	}

	filter { "system:windows" }
	linkoptions
	{
		"/STACK:4194304"
	}
	filter { }

	links
	{
		"NeuronMorphologyFormatTest",

		"NeuronMorphologyStatistics",

		"NeuronMorphologyFormatImportExportBuiltIn",
		"NeuronMorphologyFormatImportExportPlugIn",
		"NeuronMorphologyFormatImportExportScript",

		"NeuronMorphologyBuilder",

		"NeuronMorphologyFormatManager",
		"NeuronMorphologyFormat",

		"NeuronRepresentation",

		"Core",
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
