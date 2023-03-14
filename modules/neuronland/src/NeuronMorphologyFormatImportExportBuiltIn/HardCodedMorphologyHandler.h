#ifndef HARDCODEDMORPHOLOGYHANDLER_HPP_
#define HARDCODEDMORPHOLOGYHANDLER_HPP_

#include "NeuronMorphologyFormat/MorphologyFormatHandler.h"


#include "NeuronMorphologyFormatImportExportBuiltIn/HintOptions.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/StyleOptions.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/ValidationOptions.h"

typedef bool (*ImportFunction)(Input& rIn, NeuronType &nrn, const HintOptions& hintOptions);
typedef bool (*ExportFunction)(Output& rOut, const NeuronType &nrn, const StyleOptions& styleOptions);
typedef ValidationResult (*ValidateFunction)(Input& rIn, const ValidationOptions& validationOptions);


/** 

	@brief

**/

class HardCodedMorphologyHandler : public MorphologyFormatHandler
{
public:
	HardCodedMorphologyHandler(const MorphologyFormatHandlerDetails& rDetails, const ImportFunction, const ExportFunction, const ValidateFunction);

	virtual bool Import(Input& rIn, NeuronType &nrn) const;
	virtual bool Export(Output& rOut, const NeuronType &nrn) const;
	virtual ValidationResult Validate(Input& rIn) const;

	virtual bool IsImportable() const;
	virtual bool IsExportable() const;
	virtual bool IsValidatable() const;

private:

	ImportFunction					Importer;
	ExportFunction					Exporter;
	ValidateFunction				Validator;
};


HardCodedMorphologyHandler::HardCodedMorphologyHandler(const MorphologyFormatHandlerDetails& rDetails, const ImportFunction imp, const ExportFunction exp, const ValidateFunction val) :
	MorphologyFormatHandler(rDetails),
	Importer(imp),
	Exporter(exp),
	Validator(val)
{
}

bool HardCodedMorphologyHandler::Import(Input& rIn, NeuronType &nrn) const
{
	if(Importer != 0)
	{
		HintOptions opts;
		return (*Importer)(rIn, nrn, opts);
	}

	return false;
}

bool HardCodedMorphologyHandler::Export(Output& rOut, const NeuronType &nrn) const
{
	if(Exporter != 0)
	{
		StyleOptions opts;
		return (*Exporter)(rOut, nrn, opts);
	}

	return false;
}

ValidationResult HardCodedMorphologyHandler::Validate(Input& rIn) const
{
	const ValidationOptions options;

	if(Validator != 0)
		return (*Validator)(rIn, options);

	return kUndeterminable;
}

bool HardCodedMorphologyHandler::IsImportable() const
{
	return (Importer != 0);
}

bool HardCodedMorphologyHandler::IsExportable() const
{
	return (Exporter != 0);
}

bool HardCodedMorphologyHandler::IsValidatable() const
{
	return (Validator != 0);
}


#endif // HARDCODEDMORPHOLOGYHANDLER_HPP_