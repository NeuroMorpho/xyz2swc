//
// Doesnt work cos dont seem able to pass the correct value of FORMAT_NAME on the command line (using imgn)
// Hopefully will get this working eventually to remove the need for per-format cpp files...
//

#include <windows.h>

#include "CoreSL/Input.hpp"
#include "CoreSL/Output.hpp"

#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.hpp"
#include "NeuronMorphologyFormat/MorphologyFormatHandlerDetails.hpp"

#include "PlugIn.hpp"


/** 

The file format (FORMAT_NAME) is specified on the command line - everything else below will follow automatically

@todo - At the moment, plug-in DLLs are too big - each one includes the functions for the other formats,
due to the way they are all placed in an array (see BuiltInMorphologyFormat.cpp) for easy access.
Need to change this, so unused code can be stripped out.

**/

#ifndef FORMAT_NAME
#error FORMAT_NAME not defined !!
#endif


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
const BuiltInMorphologyFormat morphologyFormat = (BuiltInMorphologyFormat) FORMAT_NAME;
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


/**

Set up generic plug-in data, so it can be properly identified

**/

//PlugInDetails sg_plugInDetails = PlugInDetails( Version( String("BuiltIn:1.0.0") ), String("Morphology3D") );

/** 

Implement the specific plug-in interface here - in this case for a Morphology Format

**/

class DerivedMorphologyFormatPlugInInterface : public MorphologyFormatPlugInInterface
{
public:
	virtual const MorphologyFormatHandlerDetails&	GetHandlerDetails() const;

	virtual bool				Import(class Input& rIn, class Neuron3D &nrn) const;
	virtual bool				Export(class Output& rOut, const class Neuron3D &nrn) const;
	virtual ValidationResult	Validate(class Input& rIn) const;

	virtual bool				IsImportable() const;
	virtual bool				IsExportable() const;
	virtual bool				IsValidatable() const;
};

const MorphologyFormatHandlerDetails&	DerivedMorphologyFormatPlugInInterface::GetHandlerDetails() const
{
	return GetBuiltInMorphologyFormatHandlerDetailsAsPlugIn(morphologyFormat);
}

bool			DerivedMorphologyFormatPlugInInterface::Import(class Input& rIn, class Neuron3D &nrn) const
{
	return ImportMorphologyFormat(morphologyFormat, rIn, nrn);
}

bool			DerivedMorphologyFormatPlugInInterface::Export(class Output& rOut, const class Neuron3D &nrn) const
{
	return ExportMorphologyFormat(morphologyFormat, rOut, nrn);
}

ValidationResult	DerivedMorphologyFormatPlugInInterface::Validate(class Input& rIn) const
{
	return ValidateMorphologyFormat(morphologyFormat, rIn);
}

bool			DerivedMorphologyFormatPlugInInterface::IsImportable() const
{
	return IsImportableMorphologyFormat(morphologyFormat);
}

bool			DerivedMorphologyFormatPlugInInterface::IsExportable() const
{
	return IsExportableMorphologyFormat(morphologyFormat);
}

bool			DerivedMorphologyFormatPlugInInterface::IsValidatable() const
{
	return IsValidatableMorphologyFormat(morphologyFormat);
}


DerivedMorphologyFormatPlugInInterface sg_formatInterface;

/**

Plug-in functionality

**/

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	/*	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	break;
	}*/
	report("DLLMain - FORMAT_NAME = " + dec(FORMAT_NAME) );
	return TRUE;
}

PlugInDetails GetPlugInDetails()
{
	return PlugInDetails( Version( String("PlugIn(BuiltIn):1.0.0") ), String("Morphology3D") );
}

const MorphologyFormatPlugInInterface& GetMorphologyFormatInterface()
{
	return sg_formatInterface;
}

