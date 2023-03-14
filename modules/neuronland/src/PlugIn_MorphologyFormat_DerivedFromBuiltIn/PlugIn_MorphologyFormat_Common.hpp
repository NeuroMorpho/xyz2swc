#ifndef PLUGINMORPHOLOGYFORMATCOMMON_HPP_
#define PLUGINMORPHOLOGYFORMATCOMMON_HPP_

#include <windows.h>

#include "CoreSL/Input.hpp"
#include "CoreSL/Output.hpp"

#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.hpp"
#include "NeuronMorphologyFormat/MorphologyFormatHandlerDetails.hpp"

#include "PlugIn.hpp"


#define FunctionDeclarations(s) \
	\
	const BuiltInMorphologyFormat morphologyFormat = kMorphologyFormat##s; \
	extern bool					Import##s(Input& rIn, Neuron3D &nrn); \
	extern bool					Export##s(Output& rOut, const Neuron3D &nrn); \
	extern ValidationResult 	Validate##s(Input& rIn); \
	\
	inline bool 				ImportFunction(Input& rIn, Neuron3D &nrn)			{ return Import##s(rIn, nrn); }\
	inline bool 				ExportFunction(Output& rOut, const Neuron3D &nrn)	{ return Export##s(rOut, nrn); }\
	inline ValidationResult		ValidateFunction(Input& rIn)						{ return Validate##s(rIn); }\


#endif // PLUGINMORPHOLOGYFORMATCOMMON_HPP_