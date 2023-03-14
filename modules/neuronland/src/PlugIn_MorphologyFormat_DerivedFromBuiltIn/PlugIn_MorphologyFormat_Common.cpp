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
	return ImportFunction(rIn, nrn);
	//return ImportMorphologyFormat(morphologyFormat, rIn, nrn);
}

bool			DerivedMorphologyFormatPlugInInterface::Export(class Output& rOut, const class Neuron3D &nrn) const
{
	return ExportFunction(rOut, nrn);
	//return ExportMorphologyFormat(morphologyFormat, rOut, nrn);
}

ValidationResult	DerivedMorphologyFormatPlugInInterface::Validate(class Input& rIn) const
{
	return ValidateFunction(rIn);
	//return ValidateMorphologyFormat(morphologyFormat, rIn);
}

bool			DerivedMorphologyFormatPlugInInterface::IsImportable() const
{
	return true;//IsImportableMorphologyFormat(morphologyFormat);
}

bool			DerivedMorphologyFormatPlugInInterface::IsExportable() const
{
	return true;//IsExportableMorphologyFormat(morphologyFormat);
}

bool			DerivedMorphologyFormatPlugInInterface::IsValidatable() const
{
	return true;//IsValidatableMorphologyFormat(morphologyFormat);
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
	report("DLLMain - FORMAT_NAME = " + dec(morphologyFormat) );
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

