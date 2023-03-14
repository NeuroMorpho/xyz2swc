//
//
//
#include "ImportExportCommonIncludes.h"

#include <iostream>
#include <sstream>
#include <functional>

#include <math.h>

#include "Core/Parser.h"

/** 

	OntoMorph format

	basically an XML representation of Neurolucida data

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class OntoMorphXMLParser : public SimpleParser
{
	MorphologyBuilder builder;

public:
	OntoMorphXMLParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn)
	{
	}

	bool Parse()
	{
		return false;
	}
};

bool ImportOntoMorphXML(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	OntoMorphXMLParser parser(rIn, nrn);
	return parser.Parse();
}


bool ImportRawOntoMorphXML(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportOntoMorphXML(*pIn, nrn, options);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

bool ExportOntoMorphXML(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateOntoMorphXML(Input& rIn, const ValidationOptions& options)
{
	return kValidationFalse;
}
