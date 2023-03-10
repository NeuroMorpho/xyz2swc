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

	GulyasSeg1D format

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class GulyasSeg1DParser : public SimpleParser
{
	MorphologyBuilder builder;

public:
	GulyasSeg1DParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn)
	{
	}

	bool Parse()
	{
		return false;
	}
};

bool ImportGulyasSeg1D(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	GulyasSeg1DParser parser(rIn, nrn);
	return parser.Parse();
}

bool ImportRawGulyasSeg1D(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportGulyasSeg1D(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

bool ExportGulyasSeg1D(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateGulyasSeg1D(Input& rIn, const ValidationOptions& options)
{
	return kValidationFalse;
}
