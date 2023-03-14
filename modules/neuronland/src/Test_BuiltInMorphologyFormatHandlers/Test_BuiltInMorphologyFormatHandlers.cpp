//
//
//

#include "Core/OutputFile.h"

#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"

#include "NeuronMorphologyFormatTest/TestMorphologyFile.h"

/**
	Standalone program for running the test regime on a single morphology file.
*/
int main(int argc, char** argv)
{
	bool success = false;

	if(argc == 4)
	{
		Output* pOut = OutputFile::open(argv[2]);
		if(pOut)
		{
			String inputFile(argv[1]);
			String formatStr(argv[3]);

			BuiltInMorphologyFormat format = BuiltInFormatNameToFormatID(formatStr);

			TestNeuronMorphologyFile(pOut, inputFile, format);
			success = true;

			delete pOut;
		}
	}

	return success ? 0 : 1;
}

