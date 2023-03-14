//
//
//

#include <algorithm>
#include <iostream>
#include <map>
#include <utility>

#include "Core/v4f.h"

#include "Core/InputFile.h"
#include "Core/OutputFile.h"
#include "Core/InputMem.h"

#include "NeuronRepresentation/Neuron3D.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/NeurolucidaColours.h"
#include "NeuronMorphologyFormatTest/TestMorphologyFile.h"

#include "NeuronMorphologyStatistics/MorphologyStatistics.h"

#include "ConversionLogger.h"

#include "CommandLineOptions.h"
#include "Usage.h"
#include "ModifyOptions.h"
#include "ImportOptions.h"
#include "ExportOptions.h"

#include "NeuronMorphologyBuilder/MorphologyBuilder.h"


const char* GetApplicationName()
{
	return "NLMorphologyConverter";
}

const char* GetApplicationVersion()
{
	return "0.10.0";
}

bool IsValidSWCHintStyle(FormatStyle style)
{
	return	style == kStyleSwc_Std ||
			style == kStyleSwc_CNIC ||
			style == kStyleSwc_Gulyas;
}

bool IsValidColourString(const String& str)
{
	return	NeurolucidaColourString2Id(str) != kColour_Unknown ||
			StringBegins(str, "RGB(");
}

bool ExtractTreeList(std::vector<u32>& trees, String str)
{
	if(StringBegins(str, "--"))
		return false;

	Strings ids = StringExplode(str, ",");
	for(Strings::iterator it = ids.begin(); it != ids.end(); ++it)
	{
		trees.push_back( std::stoi( (*it) ) );
	}
		
	return true;
}

bool HandleCommandLine(u32 argc, char** argv, ImportOptions& iOptions, ExportOptions& eOptions, ValidationOptions& vOptions)
{
	if(argc == 1)
	{
		printf("No arguments specified:\n");
		PrintHelpHint();
		return false;
	}

	const bool onlyOneOption = (argc == 2);

	String singleImportFile;
	ModifyOptions* pCurrentModifyOptions = &iOptions.globalOptions;

	// Be careful with options which don't require "--"
	bool haveImportFile = false;
	bool haveExportFile = false;
	bool haveExportFormat = false;

	for(u32 n=1;n<argc;++n)
	{
		if( StringBegins( argv[n], "--") )
		{
			CommandLineOption option = StringToCommandLineOption( String(&argv[n][2]) );
			if(option == kError)
			{
				printf("Unsupported option: %s\n", argv[n]);
				PrintHelpHint();
				return false;
			}

			switch(option)
			{
				// HELP OPTIONS
				case kVersion:
					if(onlyOneOption)
						PrintVersion();
					break;
				case kHelp:
					if(onlyOneOption)
						PrintUsage();
					break;

				// INFO OPTIONS
				case kVerbose:
					iOptions.verbose = true;
					break;
				case kStats:
					iOptions.printStats = true;
					break;
				case kWarnings:
					iOptions.printFaults = true;
					break;
				case kReport:
					if(	n + 1 >= argc || /*!IsValidFilename() ||*/
						StringBegins(argv[n+1], "--"))
					{
						printf("Error: --report: Report file not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.reportFile = String(argv[n+1]);
						iOptions.createReport = true;
						n += 1;
					}
					break;

				case kTest:
					if(	n + 3 >= argc || /*!IsValidFilename() ||*/
						StringBegins(argv[n+1], "--"))
					{
						printf("Error: --test: test format and log file not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.testFile = String(argv[n+1]);
						String testFormatStr = String(argv[n+2]);
						iOptions.testFormat = BuiltInFormatNameToFormatID(testFormatStr);
						iOptions.testLogFile = String(argv[n+3]);
						iOptions.runTest = true;
						n += 3;
					}
					break;

				// IMPORT OPTIONS
				case kStrictValidation:
					iOptions.strictValidation = true;
					break;

				case kHintSWCStyle:

					if(	n + 1 >= argc || StringBegins(argv[n+1], "--"))
					{
						printf("Error: --hintSWCStyle: style not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
					
						FormatStyle style = ConvertString2FormatStyle(String(argv[n+1]));
						if( !IsValidSWCHintStyle(style) )
						{
							printf("Error: --hintSWCStyle: invalid style specified: %s!!\n", argv[n+1] );
							PrintHelpHint();
							return false;
						}

						iOptions.hints.hasHintStyle = true;
						iOptions.hints.hintStyle = style;
						n += 1;
					}
					break;

				case kHintSoma:
					if(	n + 1 >= argc ||
						StringBegins(argv[n+1], "--"))
					{
						printf("Error: --hintXXXX : soma hint not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.hasHintSoma = true;
						iOptions.hints.hintSomaSubString = String(argv[n+1]);
						n += 1;
					}
					break;

				case kHintAxon:
					if(	n + 1 >= argc ||
						StringBegins(argv[n+1], "--"))
					{
						printf("Error: --hintXXXX : axon hint not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.hasHintAxon = true;
						iOptions.hints.hintAxonSubString = String(argv[n+1]);
						n += 1;
					}
					break;

				case kHintDend:
					if(	n + 1 >= argc ||
						StringBegins(argv[n+1], "--"))
					{
						printf("Error: --hintXXXX : dend hint not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.hasHintDend = true;
						iOptions.hints.hintDendSubString = String(argv[n+1]);
						n += 1;
					}
					break;

				case kHintAllContoursAsSoma:
					iOptions.hints.allContoursAsSoma = true;
					break;

				case kHintNeuronHOCEntryProc:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --hintNeuronHOCEntryProc: Entry procedure not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.hasEntryProc = true;
						iOptions.hints.entryProc = String( argv[n+1] );
						n += 1;
					}

					break;


				case kSet1DBifurAngle:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --set1DBifurAngle: angle not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.artificialBranchingConfig.childBranchRangeAngle = std::stof(argv[n + 1]);
						n += 1;
					}
					break;

				case kSet1DBifurAngleApical:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --set1DBifurAngleApical: angle not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.artificialBranchingConfig.childBranchRangeAngleApical = std::stof(argv[n + 1]);
						n += 1;
					}
					break;

				case kSet1DBifurAngleAxon:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --set1DBifurAngleAxon: angle not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.artificialBranchingConfig.childBranchRangeAngleAxon = std::stof(argv[n + 1]);
						n += 1;
					}
					break;

				case kSet1DRootAngleRange:
					if(	n + 2 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--"))
					{
						printf("Error: --set1DRootAngleRange: angle and/or range not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.artificialBranchingConfig.rootCenterAngle = std::stof( argv[n+1] );
						iOptions.hints.artificialBranchingConfig.rootRangeAngle = std::stof(argv[n+2] );
						n += 2;
					}
					break;

				case kSet1DRootAngleRangeApical:
					if(	n + 2 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--"))
					{
						printf("Error: --set1DRootAngleRangeApical: angle and/or range not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.artificialBranchingConfig.rootCenterAngleApical = std::stof(argv[n + 1]);
						iOptions.hints.artificialBranchingConfig.rootRangeAngleApical = std::stof(argv[n + 2]);
						n += 2;
					}
					break;

				case kSet1DRootAngleRangeAxon:
					if(	n + 2 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--"))
					{
						printf("Error: --set1DRootAngleRangeAxon: angle and/or range not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.artificialBranchingConfig.rootCenterAngleAxon = std::stof(argv[n + 1]);
						iOptions.hints.artificialBranchingConfig.rootCenterAngleAxon = std::stof(argv[n + 2]);
						n += 2;
					}
					break;

				case kSet1DBranchingPattern:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --set1DBranchingPattern: angle not specified!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						iOptions.hints.artificialBranchingConfig.pattern = kPatternPlanar;//String( argv[n+1] ).toFloat();
						n += 1;
					}
					break;

				case kMerge:
					if(haveImportFile)
					{
						printf("Error: --merge: When using this option, all merged files must be specified with --merge option. You cannot specify a separate import file.\n");
						PrintHelpHint();
						return false;
					}

					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --merge: Input file must be specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						String inFile( argv[n+1] );

						iOptions.merging = true;
						iOptions.doImport = true;

						iOptions.importFileAndModifiers.push_back( std::make_pair(inFile, ModifyOptions()) );

						pCurrentModifyOptions = &(iOptions.importFileAndModifiers.front().second);

						n += 1;
					}

					break;

				// MODIFY OPTIONS
				case kOmitAllSomas: 
					pCurrentModifyOptions->omitAllSomas = true;
					break;
				case kOmitAllAxons:
					pCurrentModifyOptions->omitAllAxons = true;
					break;
				case kOmitAllDends:
					pCurrentModifyOptions->omitAllDends = true;
					break;
				case kOmitTrees:
					if(	n + 1 >= argc || 
						!ExtractTreeList(pCurrentModifyOptions->treesOmitted, String(argv[n+1])) )
					{
						printf("Error: --omitTrees: No tree Ids specified!!");
						PrintHelpHint();
						return false;
					}
					n += 1;
					break;

				case kOmitSomas:
					if(	n + 1 >= argc || 
						!ExtractTreeList(pCurrentModifyOptions->somasOmitted, String(argv[n+1])) )
					{
						printf("Error: --omitSomas: No tree Ids specified!!");
						PrintHelpHint();
						return false;
					}
					n += 1;
					break;

				case kOmitAllMarkers:
					pCurrentModifyOptions->omitGlobalMarkers = true;
					pCurrentModifyOptions->omitSomaMarkers = true;
					pCurrentModifyOptions->omitDendMarkers = true;
					pCurrentModifyOptions->omitAxonMarkers = true;
					break;
				case kOmitGlobalMarkers:
					pCurrentModifyOptions->omitGlobalMarkers = true;
					break;
				case kOmitSomaMarkers:
					pCurrentModifyOptions->omitSomaMarkers = true;
					break;
				case kOmitDendMarkers:
					pCurrentModifyOptions->omitDendMarkers = true;
					break;
				case kOmitAxonMarkers:
					pCurrentModifyOptions->omitAxonMarkers = true;
					break;

				case kOmitAllSpines:
					pCurrentModifyOptions->omitGlobalSpines = true;
					pCurrentModifyOptions->omitDendSpines = true;
					pCurrentModifyOptions->omitAxonSpines = true;
					break;
				case kOmitGlobalSpines:
					pCurrentModifyOptions->omitGlobalSpines = true;
					break;
				case kOmitDendSpines:
					pCurrentModifyOptions->omitDendSpines = true;
					break;
				case kOmitAxonSpines:
					pCurrentModifyOptions->omitAxonSpines = true;
					break;

				case kAllDendsAsAxons:
					pCurrentModifyOptions->allDendsAsAxons = true;
					break;
				case kAllAxonsAsDends:
					pCurrentModifyOptions->allAxonsAsDends = true;
					break;
				case kFlipTreeAxonOrDend:
					if(	n + 1 >= argc || 
						!ExtractTreeList(pCurrentModifyOptions->treesToFlip, String(argv[n+1])) )
					{
						printf("Error: --flipTreeType: No tree Ids specified for axon/dend flipping!!\n");
						PrintHelpHint();
						return false;
					}
					n += 1;
					break;

				case kReverseAllTrees:
					pCurrentModifyOptions->reverseAllTrees = true;
					break;

				case kReverseTrees:
					if(	n + 1 >= argc || 
						!ExtractTreeList(pCurrentModifyOptions->treesToReverse, String(argv[n+1])) )
					{
						printf("Error: --flipTreeType: No tree Ids specified for axon/dend flipping!!\n");
						PrintHelpHint();
						return false;
					}
					n += 1;
					break;

				case kMarkersToSpines:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --markersToSpines : marker name not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						if( String(argv[n+1]) == "all")
						{
							pCurrentModifyOptions->allMarkersToSpines = true;
						}
						else /*if(!pCurrentModifyOptions->allMarkersToSpines)*/
						{
							pCurrentModifyOptions->someMarkersToSpines = true;
							pCurrentModifyOptions->markerNamesAsSpines.push_back( String(argv[n+1]) );
						}
						n += 1;
					}
					break;

				case kSpinesToMarkers:
					if(	n + 1 >= argc ||
						StringBegins(argv[n+1], "--"))
					{
						printf("Error: --spinesToMarkers : marker name not specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->spineToMarker = true;
						pCurrentModifyOptions->spineMarkerName = String(argv[n+1]);
						n += 1;
					}
					break;

				case kSpinesToBranches:
					pCurrentModifyOptions->treeSpinesToBranches = true;
					break;

				case kMapGlobalSpinesToBranches:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --%s : proximity limit not specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->mapGlobalSpinesToBranches = true;
						pCurrentModifyOptions->spineMapLimit = std::stof(argv[n+1]);
						n += 1;
					}
					break;

				case kMapGlobalMarkersToBranches:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --%s : proximity limit not specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->mapGlobalMarkersToBranches = true;
						pCurrentModifyOptions->markerMapLimit = std::stof(argv[n + 1]);
						n += 1;
					}
					break;

				case kForceSpinesToBeGlobal:
					pCurrentModifyOptions->forceSpinesToBeGlobal = true;
					break;

				case kForceMarkersToBeGlobal:
					pCurrentModifyOptions->forceMarkersToBeGlobal = true;
					break;

				case kRenameMarker:
					if(	n + 2 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--"))
					{
						printf("Error: --%s : need to specify old and new marker names\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->markerRenameMap[ String(argv[n+1]) ] = String(argv[n+2]);
						n += 2;
					}
					break;

				case kForceSphereSoma:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--"))
					{
						printf("Error: --%s : soma diameter not specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->somaReplacementType = kSomaForceSphere;
						pCurrentModifyOptions->somaDiameter = std::stof(argv[n + 1]);
						n += 1;
					}
					break;
				case kForceCylinderSoma:
					if(	n + 2 >= argc ||
						StringBegins(argv[n + 1], "--")||
						StringBegins(argv[n + 2], "--"))
					{
						printf("Error: --%s : soma diameter or length not specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->somaReplacementType = kSomaForceCylinder;
						pCurrentModifyOptions->somaDiameter = std::stof(argv[n + 1]);
						pCurrentModifyOptions->somaLength = std::stof(argv[n + 2]);
						n += 2;
					}
					break;

				case kScale:
					if(	n + 4 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--") ||
						StringBegins(argv[n + 3], "--") ||
						StringBegins(argv[n + 4], "--"))
					{
						printf("Error: --scale: Scaling factor not correctly specified (must be 4 values: x y z d)!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->scale.SetX(std::stof(argv[n + 1]));
						pCurrentModifyOptions->scale.SetY(std::stof(argv[n + 2]));
						pCurrentModifyOptions->scale.SetZ(std::stof(argv[n + 3]));
						pCurrentModifyOptions->scale.SetW(std::stof(argv[n + 4]));
						n += 4;
						pCurrentModifyOptions->scaleNeuron = true;
					}

					break;
				case kTranslate:
					if(	n + 3 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--") ||
						StringBegins(argv[n + 3], "--"))
					{
						printf("Error: --translate: translation vector not correctly specified (must be 3 values: x y z)!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->translation.SetX(std::stof(argv[n + 1]));
						pCurrentModifyOptions->translation.SetY(std::stof(argv[n + 2]));
						pCurrentModifyOptions->translation.SetZ(std::stof(argv[n + 3]));
						n += 3;

						pCurrentModifyOptions->translateNeuron = true;
					}
					break;
				case kRotate:
					if(	n + 4 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--") ||
						StringBegins(argv[n + 3], "--") ||
						StringBegins(argv[n + 4], "--"))
					{
						printf("Error: --rotate: Rotation direction and angle not correctly specified (must be 4 values - x y z A) !!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						pCurrentModifyOptions->rotation.SetX(std::stof(argv[n + 1]));
						pCurrentModifyOptions->rotation.SetY(std::stof(argv[n + 2]));
						pCurrentModifyOptions->rotation.SetZ(std::stof(argv[n + 3]));
						pCurrentModifyOptions->rotation.SetW(std::stof(argv[n + 4]));
						n += 4;

						pCurrentModifyOptions->rotateNeuron = true;
					}
					break;

				case kMoveOriginToSoma:
					pCurrentModifyOptions->moveOriginToSoma = true;

					break;

				// EXPORT OPTIONS
				case kExport:

					if(haveExportFile || haveExportFormat)
					{
						// TODO - allow multiple --export
						printf("Error: --export: file and/or format have already been specified!!\n");
						PrintHelpHint();
						return false;
					}

					if(	n + 2 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--") )
					{
						printf("Error: --export: Requires both filename and file format to be specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.outputFile = String(argv[n+1]);
						eOptions.outputFormatStr = String(argv[n+2]);

						haveExportFile = true;
						eOptions.outputFormat = BuiltInFormatNameToFormatID(eOptions.outputFormatStr);

						if(eOptions.outputFormat != kMorphologyFormatError)
						{
							eOptions.doExport = true;
							haveExportFormat = true;
						}
						else
						{
							printf("Error: --export: Unsupported output file format: %s\n", argv[n]);
							PrintHelpHint();
							return false;
						}

						n += 2;
					}

					break;

				case kSplit:
					if(	n + 3 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						StringBegins(argv[n + 2], "--") ||
						StringBegins(argv[n + 3], "--") )
					{
						printf("Error: --split: Split export requires output format plus prefix and postfix string to be specified!!\n");
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.outputFormatStr = String(argv[n+1]);
						eOptions.outputFormat = BuiltInFormatNameToFormatID(String(argv[n+1]));
						eOptions.splitPrefix = argv[n+2];
						eOptions.splitPostfix = argv[n+3];
						n += 3;

						eOptions.splitExport = true;
						eOptions.doExport = true;
					}
					break;

				case kStyle:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--") ||
						!IsExportStyle(String(argv[n+1])) )
					{
						printf("Error: --%s : specified style does not exist\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.formatStyle = ConvertString2FormatStyle( String(argv[n+1]) );
						n += 1;
					}
					break;
				case kVersionNumber:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--") )
					{
						printf("Error: --%s : invalid version number!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.versionId = ConvertString2NeuroMLVersion( String(argv[n+1]) );
						n += 1;
					}
					break;

				case kIdentifyNL:
					eOptions.styleOptions.identifyNL = true;
					break;

				case kCellName:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--") )
					{
						printf("Error: --%s : expecting cell name to be specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.cellName = String(argv[n+1]);
						n += 1;
					}
					break;

				case kAddComment:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--") )
					{
						printf("Error: --%s : expecting cell name to be specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.addCommentText = true;
						eOptions.styleOptions.commentText = String(argv[n+1]);
						n += 1;
					}
					break;


				case kColourSoma:
				case kColorSoma:
					if(	n + 1 >= argc ||
						!IsValidColourString(String(argv[n+1])) )
					{
						printf("Error: --%s : invalid colour string!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
	
					return false;
					}
					else
					{
						eOptions.styleOptions.somaColour = String(argv[n+1]);
						n += 1;
					}
				break;
				case kColourDendrite:
				case kColorDendrite:
					if(	n + 1 >= argc ||
						!IsValidColourString(String(argv[n+1])) )
					{
						printf("Error: --%s : invalid colour string!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.dendColour = String(argv[n+1]);
						n += 1;
					}
				break;
				case kColourAxon:
				case kColorAxon:
					if(	n + 1 >= argc ||
						!IsValidColourString(String(argv[n+1])) )
					{
						printf("Error: --%s : invalid colour string!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.axonColour = String(argv[n+1]);
						n += 1;
					}
				break;
				case kColourSpine:
				case kColorSpine:
					if(	n + 1 >= argc ||
						!IsValidColourString(String(argv[n+1])) )
					{
						printf("Error: --%s : invalid colour string!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.spineColour = String(argv[n+1]);
						n += 1;
					}
				break;
				case kColourMarker:
				case kColorMarker:
					if(	n + 1 >= argc ||
						!IsValidColourString(String(argv[n+1])) )
					{
						printf("Error: --%s : invalid colour string!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						eOptions.styleOptions.markerColour = String(argv[n+1]);
						n += 1;
					}
				break;

				case kSetPrefixSoma:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--") )
					{
						printf("Error: --%s : soma prefix not specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						if(eOptions.styleOptions.customiseSomaName)
							printf("Warning: --%s : overriding previously specified soma prefix: %s\n", CommandLineOptionToString(option), eOptions.styleOptions.somaName.c_str());

						eOptions.styleOptions.somaName = String(argv[n+1]);
						eOptions.styleOptions.customiseSomaName = true;
						n += 1;
					}
					break;

				case kSetPrefixAxon:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--") )
					{
						printf("Error: --%s : axon prefix not specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						if(eOptions.styleOptions.customiseAxonName)
							printf("Warning: --%s : overriding previously specified axon prefix: %s\n", CommandLineOptionToString(option), eOptions.styleOptions.axonName.c_str());

						eOptions.styleOptions.axonName = String(argv[n+1]);
						eOptions.styleOptions.customiseAxonName = true;
						n += 1;
					}
					break;

				case kSetPrefixDend:
					if(	n + 1 >= argc ||
						StringBegins(argv[n + 1], "--") )
					{
						printf("Error: --%s : dendrite prefix not specified!!\n", CommandLineOptionToString(option));
						PrintHelpHint();
						return false;
					}
					else
					{
						if(eOptions.styleOptions.customiseDendName)
							printf("Warning: --%s : overriding previously specified dendrite prefix: %s\n", CommandLineOptionToString(option), eOptions.styleOptions.dendName.c_str());

						eOptions.styleOptions.dendName = String(argv[n+1]);
						eOptions.styleOptions.customiseDendName = true;
						n += 1;
					}
					break;
			}
		}
		else if(!haveImportFile)
		{
			if(iOptions.importFileAndModifiers.size() > 1)
			{
				printf("Error: Cannot mix --merge option with normal method of filename specification.\n");
				PrintHelpHint();
				return false;
			}

			singleImportFile = String(argv[n]);
			iOptions.doImport = true;
			haveImportFile = true;
		}
		else if(!haveExportFile)
		{
			eOptions.outputFile = String(argv[n]);
			haveExportFile = true;
		}
		else if(!haveExportFormat)
		{
			eOptions.outputFormatStr = String(argv[n]);
			eOptions.outputFormat = BuiltInFormatNameToFormatID(String(argv[n]));

			if(eOptions.outputFormat != kMorphologyFormatError)
			{
				eOptions.doExport = true;
				haveExportFormat = true;
			}
			else
			{
				printf("Error: Unsupported output file format: %s\n", argv[n]);
				PrintHelpHint();
				return false;
			}
		}
		else
		{
			printf("Error: Unexpected argument: %s (doesn't start with \"--\", and import file, export file, export format have already been set).\n", argv[n]);
			PrintHelpHint();
			return false;
		}
	}

	/*
	// TODO
	if( UsingOptionWhichRequiresImportFile(); && !haveImportFile)
	{
		printf("Import file not specified!\n");
		PrintHelpHint();
		return false;
	}
	*/

	if(!eOptions.splitExport && (haveExportFile && !haveExportFormat) )
	{
		printf("Both export file name and format must be specified (%s)\n", eOptions.outputFile.c_str());
		PrintHelpHint();
		return false;
	}

	if(!iOptions.merging)
	{
		// set the first file in the list correctly, reset global options which is basically a duplicate now...
		iOptions.importFileAndModifiers.push_back( std::make_pair(String(), ModifyOptions()));
		iOptions.importFileAndModifiers[0].first = singleImportFile;
		iOptions.importFileAndModifiers[0].second = iOptions.globalOptions;
		iOptions.globalOptions = ModifyOptions();

		// now there is a consistent representation of normal single file import and merge import...
	}

	return true;
}

void WriteToLogOptionNoEffect(CommandLineOption option)
{
	WriteToLog("Option has no effect: --" + String(CommandLineOptionToString(option)) );
}


// @@@ need to clean this up...
// use stats instead of repeated tree parsing!!
bool ModifyNeuron(Neuron3D& nrn, const ModifyOptions& mods)
{
	// todo - warning about options which will override other options..

	Neuron3D originalNrn(nrn);

	// first chop out anything that isn't wanted
	u32 somaCount = 1;
	Neuron3D::SomaIterator sit = nrn.SomaBegin();
	Neuron3D::SomaIterator send = nrn.SomaEnd();
	for(;sit!=send;)
	{
		bool toOmit = std::find(mods.somasOmitted.begin(), mods.somasOmitted.end(), somaCount) != mods.somasOmitted.end();

		if(toOmit || mods.omitAllSomas)
		{
			sit = nrn.DeleteSoma(sit);
			WriteToLog("\tRemoved soma: " + dec(somaCount));
		}
		else
		{
			++sit;
		}

		++somaCount;
	}

	if(mods.omitAllSomas)
	{
		assert(nrn.CountSomas() == 0);
	}

	// reduce to stumps any trees which are to be omitted..
	// flip any we wish to flip at the same time...
	u32 treeCount = 1;

	Neuron3D::AxonIterator ait = nrn.AxonBegin();
	Neuron3D::AxonIterator aend = nrn.AxonEnd();
	for(;ait!=aend;++ait)
	{
		const bool toOmit = std::find(mods.treesOmitted.begin(), mods.treesOmitted.end(), treeCount) != mods.treesOmitted.end();
		const bool toReverse = std::find(mods.treesToReverse.begin(), mods.treesToReverse.end(), treeCount) != mods.treesToReverse.end();

		if(toOmit || mods.omitAllAxons)
		{
			(*ait).clear();
			WriteToLog("\tRemoved axon tree: " + dec(treeCount));

			if(toReverse)
				WriteToLog("\t\tIgnoring request to reverse child branches (doesn't make sense)");
		}
		else
		{
			if(toReverse || mods.reverseAllTrees)
			{
				Neuron3D::ReverseChildOrderSubTree( (*ait).root() );
				WriteToLog("\tReversed axon tree: " + dec(treeCount));
			}
		}
		
		++treeCount;
	}

	Neuron3D::DendriteIterator dit = nrn.DendriteBegin();
	Neuron3D::DendriteIterator dend = nrn.DendriteEnd();
	for(;dit!=dend;++dit, ++treeCount)
	{
		const bool toOmit = find(mods.treesOmitted.begin(), mods.treesOmitted.end(), treeCount) != mods.treesOmitted.end();
		const bool toReverse = std::find(mods.treesToReverse.begin(), mods.treesToReverse.end(), treeCount) != mods.treesToReverse.end();

		if(toOmit || mods.omitAllDends)
		{
			(*dit).clear();
			WriteToLog("\tRemoved dendritic tree: " + dec(treeCount));

			if(toReverse)
				WriteToLog("\t\tIgnoring request to reverse child branches (doesn't make sense)");
		}
		else
		{
			if(toReverse || mods.reverseAllTrees)
			{
				Neuron3D::ReverseChildOrderSubTree( (*dit).root() );
				WriteToLog("\tReversed axon tree: " + dec(treeCount));
			}
		}

	}

	// flip trees
	treeCount = 1;
	ait = nrn.AxonBegin();
	aend = nrn.AxonEnd();
	for(;ait!=aend; ++treeCount)
	{
		const bool toOmit = find(mods.treesToFlip.begin(), mods.treesToFlip.end(), treeCount) != mods.treesToFlip.end();

		if(mods.allAxonsAsDends || toOmit)
		{
			ait = nrn.ChangeAxonToDendrite(ait);
			WriteToLog("\tChanged axonal tree to dendritic tree: " + dec(treeCount));
		}
		else
		{
			++ait;
		}
	}

	dit = nrn.DendriteBegin();
	dend = nrn.DendriteEnd();
	for(;dit!=dend; ++treeCount)
	{
		const bool toOmit = find(mods.treesToFlip.begin(), mods.treesToFlip.end(), treeCount) != mods.treesToFlip.end();

		if(	mods.allDendsAsAxons || toOmit)
		{
			dit = nrn.ChangeDendriteToAxon(dit);
			WriteToLog("\tChanged dendritic tree to axonal tree: " + dec(treeCount));
		}
		else
		{
			++dit;
		}
	}

	// now erase all stump tree
	ait = nrn.AxonBegin();
	aend = nrn.AxonEnd();
	for(;ait!=aend;)
	{
		if( (*ait).root() == Neuron3D::AxonTree::null() )
		{
			ait = nrn.DeleteAxon(ait);
		}
		else
		{
			++ait;
		}
	}

	dit = nrn.DendriteBegin();
	dend = nrn.DendriteEnd();
	for(;dit!=dend;)
	{
		if( (*dit).root() == Neuron3D::DendriteTree::null() )
		{
			dit = nrn.DeleteDendrite(dit);
		}
		else
		{
			++dit;
		}
	}

	if(mods.somaReplacementType != kSomaOriginal)
	{
		nrn.DeleteAllSomas();

		// heavy handed - using MorphologyBuilder
		MorphologyBuilder builder(nrn);
		const v4f centre = nrn.CalculateSomaCentre(); // will be based on tree roots

		if(mods.somaReplacementType == kSomaForceSphere)
		{
			SamplePoint pt(centre.GetX(), centre.GetY(), centre.GetZ(), mods.somaDiameter);
			builder.NewSomaSinglePoint(pt);
			WriteToLog("\tRemoved all soma data, replacing with sphere: d = " + dec(mods.somaDiameter));
		}
		else // if(mods.somaReplacementType = kSomaForceCylinder)
		{
			builder.NewSomaSegment();

			SamplePoint pt1(centre.GetX() - 0.5f*mods.somaLength, centre.GetY(), centre.GetZ(), mods.somaDiameter);
			SamplePoint pt2(centre.GetX() + 0.5f*mods.somaLength, centre.GetY(), centre.GetZ(), mods.somaDiameter);
			builder.CsAddSample(pt1);
			builder.CsAddSample(pt2);
			WriteToLog("\tRemoved all soma data, replacing with segment: d = " + dec(mods.somaDiameter) + ", l = " + dec(mods.somaLength) );
		}

		builder.ApplyChanges();
	}

	//
	if(mods.allMarkersToSpines)
	{
		if(nrn.CountAllMarkers() > 0)
		{
			nrn.ConvertAllMarkersToSpines();
			WriteToLog("\tConverted all markers to spines");
		}
	}
	else if(mods.someMarkersToSpines)
	{
		nrn.ConvertNamedMarkersToSpines(mods.markerNamesAsSpines);
	}
	else if(mods.spineToMarker)
	{
		if(nrn.CountAllSpines() > 0)
		{
			nrn.ConvertAllSpinesToMarkers(mods.spineMarkerName);
			WriteToLog("\tConverted all spines to markers");
		}
	}

	if(mods.mapGlobalMarkersToBranches)
	{
		nrn.MapGlobalMarkersToBranches(mods.markerMapLimit);
	}
	else if(mods.forceMarkersToBeGlobal)
	{
		nrn.MakeAllMarkersGlobal();
	}

	if(mods.omitGlobalMarkers)
	{
		if(nrn.CountGlobalMarkers() > 0)
		{
			nrn.ClearGlobalMarkerData();
			WriteToLog("\tRemoved all global markers");
		}
	}
	if(mods.omitSomaMarkers)
	{
		if(nrn.CountSomaMarkers() > 0)
		{
			nrn.ClearSomaMarkerData();
			WriteToLog("\tRemoved all soma markers");
		}
	}
	if(mods.omitDendMarkers)
	{
		if(nrn.CountDendriteMarkers() > 0)
		{
			nrn.ClearDendriteMarkerData();
			WriteToLog("\tRemoved all dendrite markers");
		}
	}
	if(mods.omitAxonMarkers)
	{
		if(nrn.CountAxonMarkers() > 0)
		{
			nrn.ClearAxonMarkerData();
			WriteToLog("\tRemoved all axon markers");
		}
	}

	if(mods.omitGlobalSpines)
	{
		if(nrn.CountGlobalSpines() > 0)
		{
			nrn.ClearGlobalSpineData();
			WriteToLog("\tRemoved all global spines");
		}
		//else
		//	WriteToLogOptionNoEffect(kOmitDendSpines);
	}
	if(mods.omitDendSpines)
	{
		if(nrn.CountDendriteSpines() > 0)
		{
			nrn.ClearDendriteSpineData();
			WriteToLog("\tRemoved all dendrite spines");
		}
		//else
		//	WriteToLogOptionNoEffect(kOmitDendSpines);
	}
	if(mods.omitAxonSpines)
	{
		if(nrn.CountAxonSpines() > 0)
		{
			nrn.ClearAxonSpineData();
			WriteToLog("\tRemoved all axon spines");
		}
		//else
		//	WriteToLogOptionNoEffect(kOmitAxonSpines);
	}

	if(mods.markerRenameMap.size() > 0)
	{
		nrn.RenameMarkers(mods.markerRenameMap);
		WriteToLog("\tRenaming markers");
	}

	if(mods.mapGlobalSpinesToBranches && nrn.CountGlobalSpines() > 0)
	{
		nrn.MapGlobalSpinesToBranches(mods.spineMapLimit);
		WriteToLog("\tMapping global spines to branches");
	}
	else if(mods.forceSpinesToBeGlobal)
	{
		nrn.MakeAllSpinesGlobal();
		WriteToLog("\tForcing all spines to be global");
	}

	if(mods.treeSpinesToBranches)
	{
		nrn.ConvertTreeSpinesToBranches();
	}


	// Do geometrical transforms on remaining structure
	if(mods.rotateNeuron)
	{
		nrn.Rotate(mods.rotation);
		WriteToLog("\tRotated neuron: (" + dec(mods.rotation.GetX()) + "," + dec(mods.rotation.GetY()) + "," + dec(mods.rotation.GetZ()) + "," + dec(mods.rotation.GetW()) + ")");
	}

	if(mods.scaleNeuron)
	{
		nrn.Scale(mods.scale);
		WriteToLog("\tScaled neuron: (" + dec(mods.rotation.GetX()) + "," + dec(mods.rotation.GetY()) + "," + dec(mods.rotation.GetZ()) + "," + dec(mods.rotation.GetW()) + ")");
	}

	bool doTranslation = mods.translateNeuron;
	v4f translation = mods.translation;

	if(mods.moveOriginToSoma)
	{
		if(doTranslation)
			WriteToLog("\tWarning: explicitly specified translation overridden by 'moveOriginToSoma'");

		doTranslation = true;
		translation = - nrn.CalculateSomaCentre();
		WriteToLog("\tTranslating soma centre to origin");
	}

	if(doTranslation)
	{
		nrn.Translate(translation);
		WriteToLog("\tTranslated neuron: (" + dec(translation.GetX()) + "," + dec(translation.GetY()) + "," + dec(translation.GetZ()) + ")");
	}

	return originalNrn != nrn;
}

void WriteImportLog(Output* pOut, ConversionLogger& logger)
{
	pOut->writeLine("----------------------------------------------------");
	pOut->writeLine("Neuron Converter Log:");
	pOut->writeLine("----------------------------------------------------");
	logger.OutputLog(pOut);
}

void WriteFaults(Output* pOut, MorphologyDataFaults& faults)
{
	pOut->writeLine("----------------------------------------------------");
	pOut->writeLine("Neuron Warnings (original, unmodified neuron):");
	pOut->writeLine("----------------------------------------------------");
	faults.Print(pOut);
}

void WriteStatistics(Output* pOut, NeuronStatisticsBasic& statistics)
{
	pOut->writeLine("----------------------------------------------------");
	pOut->writeLine("Neuron Statistics (original, unmodified neuron):");
	pOut->writeLine("----------------------------------------------------");
	statistics.Print(pOut);
}

void OutputReportFile(String filename, ConversionLogger& logger, NeuronStatisticsBasic& statistics, MorphologyDataFaults& faults)
{
	Output* pOut = OutputFile::open(filename);
	pOut->writeLine("####################################################");
	pOut->writeLine("NLMorphologyConverter Report:");
	pOut->writeLine("####################################################");
	pOut->writeLine("");
	pOut->writeLine("");

	// import log
	WriteImportLog(pOut, logger);

	pOut->writeLine("");
	pOut->writeLine("");

	// faults report
	WriteFaults(pOut, faults);

	pOut->writeLine("");
	pOut->writeLine("");

	// statistics overview
	WriteStatistics(pOut, statistics);

	pOut->writeLine("");
	pOut->writeLine("");

	pOut->writeLine("------------------------------------------");
	pOut->writeLine("End of Report:");
	pOut->writeLine("------------------------------------------");

	delete pOut;
}

void ShowStatistics(const NeuronStatisticsBasic& stats, String desc)
{
	std::cout << desc << std::endl;
	stats.Print(std::cout);
}

void ShowFaults(const MorphologyDataFaults& faults, String desc)
{
	std::cout << desc << std::endl;
	faults.Print(std::cout);
}

void MergeNeurons(Neuron3D& dest, const Neuron3D& src)
{
	dest.CopyMergeNeuron(src);

	// WRONG - will trash global data when merging multiple neurons...
	dest.m_globalData = src.m_globalData;
	//
}

int main(int argc, char** argv)
{
	bool success = false;

	ImportOptions	iOptions;
	ExportOptions	eOptions;
	ValidationOptions vOptions;

	if( HandleCommandLine( (u32)argc, argv, iOptions, eOptions, vOptions) )
	{
		if(iOptions.runTest)
		{
			Output* pOut = OutputFile::open(iOptions.testLogFile);
			TestNeuronMorphologyFile(pOut, iOptions.testFile, iOptions.testFormat);
			success = true;
		}
		else
		{


		if(eOptions.outputFormat != kMorphologyFormatError)
		{
			if(iOptions.doImport)
			{
				ConversionLogger logger(iOptions.verbose, iOptions.createReport);
				SetCurrentLogger(&logger);

				NeuronStatisticsBasic statisticsPre;
				MorphologyDataFaults faultsPre;
				NeuronStatisticsBasic statisticsPost;
				MorphologyDataFaults faultsPost;

				bool successfulImport = true;

				Neuron3D resultantNeuron;

				for(u32 importCount=0;importCount<iOptions.importFileAndModifiers.size();++importCount)
				{
					String inputFile = iOptions.importFileAndModifiers[importCount].first;
					ModifyOptions& modifiers = iOptions.importFileAndModifiers[importCount].second;

					Input* pInRaw = InputFile::open(inputFile);
					Input* pIn = pInRaw;
					u8*	buffer = 0;

					if(pIn)
					{
						u32 numInputFormatCandidates = 0;
						BuiltInMorphologyFormat inputFormat;

						// first check the format of the input file
						for(u32 n = 0;n < kMorphologyFormatBuiltInNumber;++n)
						{
							BuiltInMorphologyFormat format =  (BuiltInMorphologyFormat)n;
							const ValidationResult result = ValidateMorphologyFormat( format, *pIn, vOptions);
							pIn->seek(0, IoBase::Absolute);

							if(result == kValidationTrue)
							{
								if(numInputFormatCandidates > 0)
									printf("Warning - haven't uniquely identified input file format!\nPlease report problem to feedback@neuronland.org\n");

								++numInputFormatCandidates;
								inputFormat = format;

								WriteToLog("Determined Input File Format: " + GetBuiltInMorphologyFormatDetails(format).GetName());
								break;
							}
						}

						// see if it actually loads in correctly
						if(numInputFormatCandidates)
						{
							const String inputFormatName = GetBuiltInMorphologyFormatDetails(inputFormat).GetName();

							Neuron3D nrn;
							WriteToLog("Importing neuron data file: " + inputFile + "....");

							bool importSuccess = false;

							if( IsFormatFullyBufferable(inputFormat) )
							{
								// faster if we load it into memory first
								u32 size = pInRaw->remaining();
								buffer = new u8[size];
								InputMem* pInputMem =  new InputMem(buffer, size);
								pInRaw->read(buffer, size);
								delete pInRaw;
								//

								pIn = pInputMem; // so that gets cleaned up properly later..

								importSuccess = ImportMorphologyFormat(inputFormat, *pIn, nrn, iOptions.hints);
							}
							else
							{
								delete pIn;
								pIn = 0;
								importSuccess = ImportRawMorphologyFormat(inputFormat, inputFile.c_str(), nrn, iOptions.hints);
							}

							if(importSuccess)
							{
								WriteToLog("...successfully completed import.");

								// Apply import options to unmodified imported neuron data...
								//NeuronStatisticsBasic statisticsPre;
								//MorphologyDataFaults faultsPre;
								GenerateStatisticsAndFaults(nrn, statisticsPre, faultsPre);

								if(iOptions.printStats)
									ShowStatistics(statisticsPre, String("STATISTICS for original imported neuron data (unmodified)") );

								if(iOptions.printFaults)
									ShowFaults(faultsPre, String("WARNINGS for original imported neuron data (unmodified)") );

								// Apply export modification options, before saving file..
								WriteToLog("Applying modifiers...");
								
								if( ModifyNeuron(nrn, modifiers) )
								{
									//NeuronStatisticsBasic statisticsPost;
									//MorphologyDataFaults faultsPost;
									//GenerateStatisticsAndFaults(nrn, statisticsPost, faultsPost);

									//if(iOptions.printStats)
									//	ShowStatistics(statisticsPost, String("STATISTICS for modified neuron data") );

									//if(iOptions.printFaults)
									//	ShowFaults(faultsPost, String("WARNINGS for modified neuron data") );
								}
								else
								{
									WriteToLog("\tnone");
								}

								MergeNeurons(resultantNeuron, nrn);
							}
							else
							{
								WriteToLog(inputFormatName + ": Failed to complete import.");
								printf("Couldn't successfully import the file: %s as format %s\n", inputFile.c_str(), GetBuiltInMorphologyFormatDetails(inputFormat).GetName().c_str());
								break;
							}

							if(!successfulImport)
								break;
						}

						delete pIn;

						if(buffer)
							delete [] buffer;
					}
					else
						printf("Cannot open input file: %s\n", inputFile.c_str());
				}

				if(successfulImport)
				{
					if(eOptions.doExport)
					{
						// set derived style options
						String inputFileList;
						for(u32 importCount=0;importCount<iOptions.importFileAndModifiers.size();++importCount)
						{
							if(importCount > 0)
								inputFileList += ",";
							String inputFile = iOptions.importFileAndModifiers[importCount].first;
							inputFileList += inputFile;
						}

						eOptions.styleOptions.appName = GetApplicationName();
						eOptions.styleOptions.appVersion = GetApplicationVersion();
						eOptions.styleOptions.fileName = inputFileList;
						//


						// apply global modifiers

						if(iOptions.merging)
							WriteToLog("Applying global modifiers to merged data...");

						if( ModifyNeuron(resultantNeuron, iOptions.globalOptions) )
						{
						}
						else
						{
							if(iOptions.merging)
								WriteToLog("\tnone");
						}

						if( resultantNeuron.HasOnlyBinaryTrees() || !IsFormatBinaryOnly(eOptions.outputFormat) )
						{
							if(resultantNeuron.CountAxons() > 0 && !DoesFormatSupportAxons(eOptions.outputFormat) )
							{
								if(resultantNeuron.CountDendrites() > 0)
									printf("Original file (after any modifications) has axon data. Selected format (%s) does not support this. Axons will be ignored\n", GetBuiltInMorphologyFormatDetails(eOptions.outputFormat).GetName().c_str());
								else
								{
									printf("Original file (after any modifications) only has axon data. Selected format (%s) does not support this.\n", GetBuiltInMorphologyFormatDetails(eOptions.outputFormat).GetName().c_str());
									return 0;
								}
							}

							std::map<String, Neuron3D*> neuronsToExport;
							if(eOptions.splitExport)
							{
								u32 treeCount = 1;
								for(Neuron3D::DendriteConstIterator dit = resultantNeuron.DendriteBegin();dit!=resultantNeuron.DendriteEnd();++dit, ++treeCount)
								{
									String treeFileName = eOptions.splitPrefix + "_dend_" + dec(treeCount) + "." + eOptions.splitPostfix;
									Neuron3D* pNrn = new Neuron3D;
									Neuron3D::DendriteIterator newdit = pNrn->CreateDendrite();
									*newdit = *dit;
									neuronsToExport[treeFileName] = pNrn;
								}

								for(Neuron3D::AxonConstIterator ait = resultantNeuron.AxonBegin();ait!=resultantNeuron.AxonEnd();++ait, ++treeCount)
								{
									String treeFileName = eOptions.splitPrefix + "_axon_" + dec(treeCount) + "." + eOptions.splitPostfix;
									Neuron3D* pNrn = new Neuron3D;
									Neuron3D::AxonIterator newait = pNrn->CreateAxon();
									*newait = *ait;
									neuronsToExport[treeFileName] = pNrn;
								}
							}
							else
							{
								neuronsToExport[eOptions.outputFile] = &resultantNeuron;
							}

							// for each neuron
							for(std::map<String, Neuron3D*>::const_iterator it = neuronsToExport.begin();it != neuronsToExport.end();++it)
							{
								// now output the desired file
								Output* pOut = OutputFile::open( (*it).first );
								if(pOut)
								{
									WriteToLog("Exporting file: " + (*it).first + ".....");
									if(ExportMorphologyFormat(eOptions.outputFormat, *pOut, *( (*it).second), eOptions.styleOptions) )
									{
										WriteToLog("....export successfully completed");
										success = true;
									}
									else
										printf("Couldn't successfully export in format %s\n", eOptions.outputFormatStr.c_str());

									delete pOut;
								}
								else
									printf("Cannot open output file: %s\n", eOptions.outputFile.c_str());
							}

							if(eOptions.splitExport)
							{
								for(std::map<String, Neuron3D*>::const_iterator it = neuronsToExport.begin();it != neuronsToExport.end();++it)
									delete it->second;

								neuronsToExport.clear();
							}

						}
						else
							printf("Original file has non-binary branch points. Selected format (%s) does not support this\n", GetBuiltInMorphologyFormatDetails(eOptions.outputFormat).GetName().c_str());
					}
					else
					{
						// didn't have to export...and got this far, so success!
						success = true;
					}
				}
				else
				{
					//printf("Import failed...");
				}

				// finally, create report
				if(iOptions.createReport)
				{
					OutputReportFile(iOptions.reportFile, logger, statisticsPre, faultsPre);
					WriteToLog("Wrote report file: " + iOptions.reportFile);
				}

			}
		}
		}
	}

	return success ? 0 : 1;
}
