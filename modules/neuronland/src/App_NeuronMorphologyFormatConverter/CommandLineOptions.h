#ifndef _NLMC_COMMANDLINEOPTIONS_HPP_
#define _NLMC_COMMANDLINEOPTIONS_HPP_


enum CommandLineOption
{
	kError,

	kVersion,
	kHelp,

	kVerbose,
	kStats,
	kWarnings,
	kReport,
	kTest,

	kStrictValidation,

	kHintSWCStyle,
	kHintSoma,
	kHintAxon,
	kHintDend,
	kHintAllContoursAsSoma,
	kHintNeuronHOCEntryProc,

	kSet1DBifurAngle,
	kSet1DBifurAngleApical,
	kSet1DBifurAngleAxon,
	kSet1DRootAngleRange,
	kSet1DRootAngleRangeApical,
	kSet1DRootAngleRangeAxon,
	kSet1DBranchingPattern,

	kOmitAllSomas,
	kOmitAllAxons,
	kOmitAllDends,
	kOmitSomas,
	kOmitTrees,

	kOmitAllMarkers,
	kOmitGlobalMarkers,
	kOmitSomaMarkers,
	kOmitDendMarkers,
	kOmitAxonMarkers,
	kOmitAllSpines,
	kOmitGlobalSpines,
	kOmitDendSpines,
	kOmitAxonSpines,

	kAllDendsAsAxons,
	kAllAxonsAsDends,
	kFlipTreeAxonOrDend,
	kReverseAllTrees,
	kReverseTrees,

	kMarkersToSpines,
	kSpinesToMarkers,

	kSpinesToBranches,

	kMapGlobalSpinesToBranches,
	kMapGlobalMarkersToBranches,

	kForceSpinesToBeGlobal,
	kForceMarkersToBeGlobal,

	kRenameMarker,

	kForceSphereSoma,
	kForceCylinderSoma,
	
	kScale,
	kTranslate,
	kRotate,
	kMoveOriginToSoma,

	kStyle,
	kVersionNumber,
	kIdentifyNL,
	kCellName,
	kAddComment,

	kColourSoma,
	kColorSoma,
	kColourDendrite,
	kColorDendrite,
	kColourAxon,
	kColorAxon,
	kColourSpine,
	kColorSpine,
	kColourMarker,
	kColorMarker,

	kSetPrefixSoma,
	kSetPrefixAxon,
	kSetPrefixDend,

	kExport,
	kMerge,
	kSplit
};


CommandLineOption StringToCommandLineOption(String s);
const char* CommandLineOptionToString(CommandLineOption opt);


#endif // _NLMC_COMMANDLINEOPTIONS_HPP_
