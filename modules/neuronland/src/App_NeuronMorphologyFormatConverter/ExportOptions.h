#ifndef _NLMC_EXPORTOPTIONS_HPP_
#define _NLMC_EXPORTOPTIONS_HPP_

#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/StyleOptions.h"

struct ExportOptions
{
	// single file export
	bool					doExport;
	String					outputFile;
	String					outputFormatStr;
	BuiltInMorphologyFormat	outputFormat;

	// split file export
	bool				splitExport;
	String				splitPrefix;
	String				splitPostfix;

	StyleOptions		styleOptions;

	ExportOptions()
	{
		doExport			= false;
		splitExport			= false;
	}
};


#endif // _NLMC_EXPORTOPTIONS_HPP_

