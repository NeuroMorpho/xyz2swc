#ifndef _NLMC_IMPORTOPTIONS_HPP_
#define _NLMC_IMPORTOPTIONS_HPP_

#include <vector>

#include "Core/String.hpp"
#include "NeuronMorphologyFormatImportExportBuiltIn/HintOptions.h"

#include "ModifyOptions.h"


struct ImportOptions
{
	bool	doImport;
	bool	printStats;
	bool	printFaults;
	bool	createReport;
	String  reportFile;
	bool	verbose;
	bool	merging;
	bool	strictValidation;

	bool	runTest;
	String	testFile;
	String	testLogFile;
	BuiltInMorphologyFormat testFormat;

	HintOptions hints;

	ModifyOptions globalOptions;
	std::vector< std::pair<String, ModifyOptions> > importFileAndModifiers;

	ImportOptions()
	{
		doImport = false;
		printStats = false;
		printFaults = false;
		createReport = false;
		verbose = false;
		merging = false;
		strictValidation = false;
		runTest = false;
	}
};


#endif // _NLMC_IMPORTOPTIONS_HPP_
