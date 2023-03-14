//
//
//

#include "Core/Types.h"
#include "Core/String.hpp"

#include "CommandLineOptions.h"

const char* commandLineOptions[] = 
{
	"ERROR",

	"version",
	"help",

	"verbose",
	"stats",
	"warnings",
	"report",
	"test",

	"strictValidation",

	"hintSWCStyle",
	"hintSoma",
	"hintAxon",
	"hintDend",
	"hintAllContoursAsSoma",
	"hintNeuronHOCEntryProc",

	"set1DBifurAngle",
	"set1DBifurAngleApical",
	"set1DBifurAngleAxon",
	"set1DRootAngleRange",
	"set1DRootAngleRangeApical",
	"set1DRootAngleRangeAxon",
	"set1DBranchingPattern",

	"omitAllSomas",
	"omitAllAxons",
	"omitAllDends",
	"omitSomas",
	"omitTrees",

	"omitAllMarkers",
	"omitGlobalMarkers",
	"omitSomaMarkers",
	"omitDendMarkers",
	"omitAxonMarkers",
	"omitAllSpines",
	"omitGlobalSpines",
	"omitDendSpines",
	"omitAxonSpines",

	"allDendsAsAxons",
	"allAxonsAsDends",
	"flipTreeType",
	"reverseAllTrees",
	"reverseTrees",

	"markersToSpines",
	"spinesToMarkers",

	"spinesToBranches",

	"mapGlobalSpinesToBranches",
	"mapGlobalMarkersToBranches",

	"forceSpinesToBeGlobal",
	"forceMarkersToBeGlobal",

	"renameMarker",

	"forceSphereSoma",
	"forceCylinderSoma",

	"scale",
	"translate",
	"rotate",
	"moveOriginToSoma",

	"style",
	"versionNumber",
	"identifyNL",
	"cellName",
	"addComment",

	"colourSoma",
	"colorSoma",
	"colourDendrite",
	"colorDendrite",
	"colourAxon",
	"colorAxon",
	"colourSpine",
	"colorSpine",
	"colourMarker",
	"colorMarker",

	"setPrefixSoma",
	"setPrefixAxon",
	"setPrefixDend",

	"export",
	"merge",
	"split"
};

CommandLineOption StringToCommandLineOption(String s)
{
	for(u32 n=0;n<sizeof(commandLineOptions)/sizeof(char*); ++n)
	{
		if(s == String(commandLineOptions[n]))
		{
			return (CommandLineOption)n;
		}
	}
	
	return kError;
}

const char* CommandLineOptionToString(CommandLineOption opt)
{
	return commandLineOptions[opt];
}
