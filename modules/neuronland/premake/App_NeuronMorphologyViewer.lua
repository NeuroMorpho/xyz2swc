
project "NeuronMorpholgyViewer"
	neuronland_proj_app_windowed()
	
	files
	{
		"../src/App_NeuronMorphologyViewer/**.cpp",
		"../src/App_NeuronMorphologyViewer/**.h*"
	}

	libdirs
	{
		"../3rdParty/HDF5/1.12.0/lib"
	}
	
	linkoptions
	{
	}

	links
	{
		-- HDF5
		"hdf5.lib",
		"hdf5_cpp.lib",
		"zlib.lib",
		"szip.lib",

		-- TinyXml
		"TinyXml"		
	}
	
	links
	{
		"NeuronRepresentation",
		"NeuronMorphologyBuilder",
		"NeuronMorphologyFormat",
		"NeuronMorphologyFormatManager",
		"NeuronMorphologyFormatImportExportBuiltIn",
		"NeuronMorphologyFormatImportExportPlugIn",
		"NeuronMorphologyFormatImportExportScript",
		"NeuronMorphologyFormatTest",
		"NeuronMorphologyStatistics",
		
		"NeuronApplicationLogic"
	}
