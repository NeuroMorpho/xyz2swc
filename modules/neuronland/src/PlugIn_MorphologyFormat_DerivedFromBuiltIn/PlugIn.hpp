
#include "NeuronPlugInSupportGeneral/PlugInDetails.hpp"
#include "NeuronPlugInSupportMorphologyFormat/MorphologyFormatPlugInInterface.hpp"


extern "C" __declspec(dllexport)	PlugInDetails							GetPlugInDetails();
extern "C" __declspec(dllexport)	const MorphologyFormatPlugInInterface&	GetMorphologyFormatInterface();
