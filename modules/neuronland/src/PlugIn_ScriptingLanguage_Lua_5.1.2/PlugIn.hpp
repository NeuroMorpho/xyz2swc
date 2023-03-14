
#include "NeuronPlugInSupportGeneral/PlugInDetails.hpp"
#include "NeuronPlugInSupportScriptingLanguage/ScriptingLanguagePlugInInterface.hpp"


extern "C" __declspec(dllexport)	PlugInDetails							GetPlugInDetails();
extern "C" __declspec(dllexport)	const ScriptingLanguagePlugInInterface&	GetScriptingLanguagePlugInInterface();
