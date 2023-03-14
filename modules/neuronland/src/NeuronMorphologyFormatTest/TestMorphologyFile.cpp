
#include <filesystem>

#include "Core/StringFormatting.h"
#include "Core/InputFile.h"
#include "Core/InputMem.h"
#include "Core/OutputFile.h"
#include "Core/OutputMem.h"
#include "Core/InputBuffer.h"


#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"

/** 

	The purpose of this test is to thoroughly check all the MorphologyFormat import/export/validate
	functionality.

	For as many files we can find, for all supported formats, we need to

	1) Check that the file A is correctly auto-identified.
	2) Check that the file A is loaded correctly.
	3) Check that the loaded data can be saved out to the same format (file B)
	4) Check if file A and file B are identical or not.
	5) Check that file B can be successfully loaded.
	6) Compare the in-memory representation of the two sets of data.
	7) Check that the file can be successfully saved out to all other supported formats. (file C upwards)
	8) Check that thes saved filees ecan be loaded correctly, and compare binary rep.

	*) All results must be logged
	*) Important - this test needs to be distributed with Master versions, and therefore needs to 
	load up plug-ins and scripts.
	*) Will also want to launch it from within user interface


	Usage 

	<Test> directory logfile

**/

#if defined(__linux__)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif

String Bool2Str(bool value)
{
	return value ? "yes" : "no ";
}

bool CompareFiles(Input& rInLhs, Input& rInRhs)
{
	while(rInLhs.remaining() && rInRhs.remaining())
	{
		if( rInLhs.readLine() != rInRhs.readLine())
			return false;
	}

	return (rInLhs.remaining() == rInRhs.remaining());
}

/*
struct NeuronComparisonInfo
{

}
*/
enum TestFlag
{
	kTestFlagDends			=1<<0,
	kTestFlagAxons			=1<<1,
	kTestFlagSomas			=1<<2,
	kTestFlagSpinesGlobal	=1<<3,
	kTestFlagSpinesTree		=1<<4,
	kTestFlagSpinesRootBranchSample0 =1<<5,
	kTestFlagMarkersGlobal	=1<<6,
	kTestFlagMarkersSoma	=1<<7,
	kTestFlagMarkersTree	=1<<8,
	kTestFlagMarkerNames	=1<<9,

	kTestFlagDendType		=1<<10,
	kTestFlagTerminalType	=1<<11
};

enum CompareResult
{
	kFailGeneral,
	kFailAxonCount,
	kFailDendriteCount,
	kFailSomaCount,
	kFailConnectivity,
	kFailSampleCount,
	kFailSomaSampleCount,
	kFailMarkerCount,
	kFailSpineCount,
	kFailSamplePositionTolerance,
	kFailSampleDiameterTolerance,
	kFailMarkerNumberOfTypes,
	kFailMarkerName,
	kFailMarkerPositionTolerance,
	kFailMarkerDiameterTolerance,
	kFailSpinePositionTolerance,
	kFailSpineDiameterTolerance,
	kFailSpineAssociatedSegment,
	kFailSomaType,
	kFailDendriteType,
	kFailTerminalType,
	kPass,
};

const char* compareStringArray[] = 
{
	"Failure: General",
	"Failure: Axon Count Mismatch",
	"Failure: Dendrite Count Mismatch",
	"Failure: Soma Count Mismatch",
	"Failure: Connectivity Mismatch",
	"Failure: Sample Count Mismatch",
	"Failure: Sample Count Mismatch (Soma)",
	"Failure: Marker Count Mismatch",
	"Failure: Spine Count Mismatch",
	"Failure: Sample Position Tolerance",
	"Failure: Sample Diameter Tolerance",
	"Failure: Marker Number of Types",
	"Failure: Marker Name",
	"Failure: Marker Position Tolerance",
	"Failure: Marker Diameter Tolerance",
	"Failure: Spine Position Tolerance",
	"Failure: Spine Diameter Tolerance",
	"Failure: Spine Associated Sample",
	"Failure: Soma Type",
	"Failure: Dendrite Type",
	"Failure: Terminal Type",
	"Pass"
};

String CompareResult2String(CompareResult result)
{
	return String(compareStringArray[result]);
}

void PrintSample(const SamplePoint& sample)
{
	printf("(%f, %f, %f) %f\n", sample.x, sample.y, sample.z, sample.d);
}

CompareResult CompareBranch3D(const Branch3D& lhs, const Branch3D& rhs, u32 testFlags, u32 locality, bool root)
{
	// check if sample data is identical
	if( lhs.m_samples.size() == rhs.m_samples.size() )
	{
		const u32 start = (root ? 0 : 1);

		for(u32 n=start;n<lhs.m_samples.size();++n)
		{
			SamplePoint sampleLhs = lhs.m_samples[n];
			SamplePoint sampleRhs = rhs.m_samples[n];

			if(	fabs(sampleLhs.x - sampleRhs.x) > 0.1 ||
				fabs(sampleLhs.y - sampleRhs.y) > 0.1 ||
				fabs(sampleLhs.z - sampleRhs.z) > 0.1 )
			{
				PrintSample(sampleLhs);
				PrintSample(sampleRhs);
				return kFailSamplePositionTolerance;
			}
/*
			if(	fabs(sampleLhs.d - sampleRhs.d) > 0.1 )
			{
				return kFailDiameterTolerance;
			}*/
		}
	}
	else
	{
		if(locality & kSoma)
			return kFailSomaSampleCount;
		else
			return kFailSampleCount;
	}

	// check if marker data is identical
	if( ((testFlags&kTestFlagMarkersTree) && locality == kBranch ) ||
		((testFlags&kTestFlagMarkersSoma) && locality == kSoma ) ||
		((testFlags&kTestFlagMarkersGlobal) && locality == kGlobal ) )
	{
		if( lhs.m_markers.size() == rhs.m_markers.size() )
		{
			Branch3D::Markers::const_iterator itL = lhs.m_markers.begin();
			Branch3D::Markers::const_iterator itR = rhs.m_markers.begin();
			for(;itL!=lhs.m_markers.end();++itL, ++itR)
			{
				if(testFlags&kTestFlagMarkerNames)
				{
					if(itL->first != itR->first)
						return kFailMarkerName;
				}

				if(itL->second.size() != itR->second.size() )
					return kFailMarkerNumberOfTypes;

				for(u32 n=0;n<itL->second.size();++n)
				{
					const MarkerPoint markerLhs = itL->second[n];
					const MarkerPoint markerRhs = itR->second[n];

					if(	fabs(markerLhs.x - markerRhs.x) > 0.1 ||
						fabs(markerLhs.y - markerRhs.y) > 0.1 ||
						fabs(markerLhs.z - markerRhs.z) > 0.1 )
					{
						PrintSample(markerLhs);
						PrintSample(markerRhs);
						return kFailMarkerPositionTolerance;
					}

					/*
					if(	fabs(sampleLhs.d - sampleRhs.d) > 0.1 )
					{
						return kFailDiameterTolerance;
					}*/

				}
			}
		}
		else
			return kFailMarkerCount;
	}

	// check if spine data is identical
	if( ((testFlags&kTestFlagSpinesTree) && locality == kBranch ) ||
		((testFlags&kTestFlagSpinesGlobal) && locality == kGlobal ) )
	{
		if( lhs.m_spines.size() == rhs.m_spines.size() )
		{
			for(u32 n=0;n<lhs.m_spines.size();++n)
			{
				SpinePoint sampleLhs = lhs.m_spines[n];
				SpinePoint sampleRhs = rhs.m_spines[n];

				if(	fabs(sampleLhs.x - sampleRhs.x) > 0.1 ||
					fabs(sampleLhs.y - sampleRhs.y) > 0.1 ||
					fabs(sampleLhs.z - sampleRhs.z) > 0.1 )
				{
					PrintSample( (SamplePoint&) sampleLhs);
					PrintSample( (SamplePoint&) sampleRhs);
					return kFailSpinePositionTolerance;
				}

				if(sampleLhs.associatedSegment != sampleRhs.associatedSegment)
				{
					const bool rootBranchSample0 = (root && (sampleLhs.associatedSegment == 0 || sampleRhs.associatedSegment) );

					if(!rootBranchSample0 || ( rootBranchSample0 && (testFlags&kTestFlagSpinesRootBranchSample0) ) )
						return kFailSpineAssociatedSegment;
				}
			}
		}
		else
			return kFailSpineCount;
	}

	return kPass;
}

CompareResult CompareSoma3D(const Soma3D& lhs, const Soma3D& rhs, u32 testFlags)
{
	CompareResult result = CompareBranch3D(lhs, rhs, testFlags, kSoma, true);

	if(result != kPass)
		return result;

//	if(lhs.somaType != rhs.somaType)
//		return kFailSomaType;

	return kPass;
}

CompareResult CompareBranch(Neuron3D::DendriteTreeConstIterator itLhs, Neuron3D::DendriteTreeConstIterator itRhs, u32 testFlags)
{
	bool root = (!itLhs.parent());

	CompareResult result = CompareBranch3D(*itLhs, *itRhs, testFlags, kBranch, root);
	if(result != kPass)
		return result;

	// check if connectivity structure is identical
	if(itLhs.child() && itRhs.child())
	{
		CompareResult result = CompareBranch(itLhs.child(), itRhs.child(), testFlags);
		if(result != kPass)
			return result;
	}
	else if(itLhs.child() != itRhs.child()) // one 0, the other non-zero..
		return kFailConnectivity;
	else
	{
		// terminal: check terminal type
		if(testFlags&kTestFlagTerminalType)
		{
			if( (*itLhs).GetTerminalType() != (*itLhs).GetTerminalType() )
				return kFailTerminalType;
		}
	}

	if(itLhs.peer() && itRhs.peer())
	{
		CompareResult result = CompareBranch(itLhs.peer(), itRhs.peer(), testFlags);
		if(result != kPass)
			return result;
	}
	else if(itLhs.peer() != itRhs.peer())
		return kFailConnectivity;

	return kPass;
}

CompareResult CompareNeurons(const Neuron3D& nrnLhs, const Neuron3D& nrnRhs, u32 testFlags)
{
	// soma
	if(testFlags&kTestFlagSomas)
	{
		if(nrnLhs.CountSomas() != nrnRhs.CountSomas())
			return kFailSomaCount;

		for(Neuron3D::SomaConstIterator l=nrnLhs.SomaBegin(), r =nrnRhs.SomaBegin();l!=nrnLhs.SomaEnd();++l, ++r)
		{
			CompareResult result = CompareSoma3D( (*l), (*r), testFlags);
			if(result != kPass)
				return result;
		}
	}

	// global spine/marker data
	{
		CompareResult result = CompareBranch3D(nrnLhs.m_globalData, nrnRhs.m_globalData, testFlags, kGlobal, true);
		if(result != kPass)
			return result;
	}


	// axons
	if(testFlags&kTestFlagAxons)
	{
		if(nrnLhs.CountAxons() != nrnRhs.CountAxons())
			return kFailAxonCount;

		for(Neuron3D::AxonConstIterator l=nrnLhs.AxonBegin(), r =nrnRhs.AxonBegin();l!=nrnLhs.AxonEnd();++l, ++r)
		{
			CompareResult result = CompareBranch( (*l).root(), (*r).root(), testFlags);
			if(result != kPass)
				return result;
		}
	}

	// dendrites
	if(testFlags&kTestFlagDends)
	{
		if(nrnLhs.CountDendrites() != nrnRhs.CountDendrites())
			return kFailDendriteCount;

		for(Neuron3D::DendriteConstIterator l=nrnLhs.DendriteBegin(), r =nrnRhs.DendriteBegin();l!=nrnLhs.DendriteEnd();++l, ++r)
		{
			if(testFlags&kTestFlagDendType)
			{
				if( (*l).GetTreeType() != (*r).GetTreeType() )
					return kFailDendriteType;
			}

			CompareResult result = CompareBranch( (*l).root(), (*r).root(), testFlags);
			if(result != kPass)
				return result;
		}
	}

	return kPass;
}

struct ExportReimportResult
{
	bool exported;
	bool reimported;
	CompareResult neuronsIdentical;

	ExportReimportResult()
	{
		exported = false;
		reimported = false;
		neuronsIdentical = kFailGeneral;
	}
};

#define WORK_IN_MEMORY
size_t g_pSize=0;
void* g_pMem=0;
 
ExportReimportResult DoExportReimportTest(const Neuron3D& nrn, String outputFileName, BuiltInMorphologyFormat format, u32 testFlags, const HintOptions& hintOptions, const StyleOptions& styleOptions)
{
	ExportReimportResult results;

#ifdef WORK_IN_MEMORY
	if(!g_pMem)
	{
		g_pSize = 30*1024*1024;
		g_pMem = malloc(g_pSize);
	}

	OutputMem* pOutNew = new OutputMem(g_pMem, g_pSize);
	u32 currentFileSize = 0;
#else
	Output* pOutNew = OutputFile::open(outputFileName);
#endif


	if(pOutNew)
	{
		results.exported = ExportMorphologyFormat(format, *pOutNew, nrn, styleOptions);
#ifdef WORK_IN_MEMORY
		currentFileSize = pOutNew->GetBytesWritten();
#endif
		delete pOutNew;
	}

	if(results.exported)
	{
#ifdef WORK_IN_MEMORY
		InputMem* pInNew = new InputMem(g_pMem, currentFileSize);
#else
		Input* pInNew = InputFile::open(outputFileName);
#endif
		if(pInNew)
		{
			Neuron3D nrn_reimport;

			results.reimported = ImportMorphologyFormat(format, *pInNew, nrn_reimport, hintOptions);

			if(results.reimported)
			{
				//filesIdentical = CompareFiles(rInBuffer, *pInNew);
				results.neuronsIdentical = CompareNeurons(nrn, nrn_reimport, testFlags);
			}
			delete pInNew;
		}
	}

	return results;
}

bool IsFormatActiveForTesting(BuiltInMorphologyFormat format)
{
	return	IsFormatFull3D(format) && 
			(format != kMorphologyFormatGlasgow) &&
			(format != kMorphologyFormatImarisHDF) &&
			(format != kMorphologyFormatInventor) &&
			(format != kMorphologyFormatSimpleNeuriteTracer);
}

u32 GetNumberOfExportVariationsSupported(BuiltInMorphologyFormat format)
{
	if(format == kMorphologyFormatNeuroML)
		return GetNumberOfExportVersionsSupported(format);
	else 
		return GetNumberOfExportStylesSupported(format);
}

void ConfigureStyleOptionsForVariation(StyleOptions& styleOptions, BuiltInMorphologyFormat format, u32 variation)
{
	if(format == kMorphologyFormatNeuroML)
		styleOptions.versionId = (NeuroMLVersion)variation;
	else
	{
		styleOptions.formatStyle = (FormatStyle)( (u32)GetFormatStyleStartId(format) + variation );
	}
}

String GetVariationName(BuiltInMorphologyFormat format, u32 variation)
{
	if(format == kMorphologyFormatNeuroML)
		return ConvertNeuroMLVersion2String( (NeuroMLVersion)variation);
	else
	{
		FormatStyle formatStyle = (FormatStyle)( (u32)GetFormatStyleStartId(format) + variation );
		return GetFormatStyleName(formatStyle);
	}
}

bool CanCompareDendrites(BuiltInMorphologyFormat format1, BuiltInMorphologyFormat format2)
{
	return DoesFormatSupportDendrites(format1) && DoesFormatSupportDendrites(format2);
}

bool CanCompareAxons(BuiltInMorphologyFormat format1, BuiltInMorphologyFormat format2)
{
	return DoesFormatSupportAxons(format1) && DoesFormatSupportAxons(format2);
}

bool CanCompareDendriteType(BuiltInMorphologyFormat format1, BuiltInMorphologyFormat format2)
{
	return DoesFormatSupportDendriteSubTypes(format1) && DoesFormatSupportDendriteSubTypes(format2);
}

bool CanCompareTerminalType(BuiltInMorphologyFormat format1, BuiltInMorphologyFormat format2)
{
	return DoesFormatSupportTerminalTypes(format1) && DoesFormatSupportTerminalTypes(format2);
}


void TestNeuronMorphologyFile(Output* pOut, const String& filename, BuiltInMorphologyFormat origFormat)
{
	bool validated = false;
	bool misvalidated = false;
	bool imported = false;

	bool validationArray[kMorphologyFormatBuiltInNumber];

	Input* pInOriginal = InputFile::open(/*directory + "/" +*/ filename);
	if(pInOriginal)
	{
		InputBuffer rInBuffer(pInOriginal->remaining(), pInOriginal);

		for(u32 n=0;n<kMorphologyFormatBuiltInNumber;++n)
		{
			ValidationOptions options;
			BuiltInMorphologyFormat testFormat = (BuiltInMorphologyFormat)n;
			validationArray[n] = (ValidateMorphologyFormat(testFormat, rInBuffer, options) == kValidationTrue);
			rInBuffer.seek(0, IoBase::Absolute);

			if(origFormat == testFormat)
			{
				validated = validationArray[n];
			}
			else if(!misvalidated)
			{
				misvalidated = validationArray[n];
			}
		}

		Neuron3D nrn_original;

		HintOptions hintOptions;
		imported = ImportMorphologyFormat(origFormat, rInBuffer, nrn_original, hintOptions);
		rInBuffer.seek(0, IoBase::Absolute);

		pOut->writeLine("File: " + filename + ",  Format: " + GetBuiltInMorphologyFormatDetails(origFormat).GetName());
		pOut->writeLine("    Test Validation for specified format: " +  (validated ? String("Pass") : String("Fail")) );
		pOut->writeLine("    Test No Validation for other formats: " +  (!misvalidated ? String("Pass") : String("Fail")) );
		pOut->writeLine(String("    Test Import: ") + (imported ? String("Pass") : String("Fail") ) );

		if(imported)
		{
			const char* testDirectory = /*directory +*/ "Test\\";
			if( !fs::is_directory( fs::path(testDirectory) ) )
				fs::create_directory( testDirectory );

			// neuron properties - helps us to decide what we can compare in the test...
			const bool binaryAxons	= nrn_original.HasOnlyBinaryAxons();
			const bool binaryDends	= nrn_original.HasOnlyBinaryDends();
			const u32 numAxons		= nrn_original.CountAxons();
			const u32 numDendrites	= nrn_original.CountDendrites();
			const u32 numSomas		= nrn_original.CountSomas();

			const u32 numSpines = nrn_original.CountAllSpines();
			const bool hasSpinesGlobal	= (nrn_original.CountGlobalSpines() > 0);
			const bool hasSpinesTree	= (nrn_original.CountDendriteSpines() + nrn_original.CountAxonSpines() > 0);

			const u32 numMarkers = nrn_original.CountAllMarkers();
			const bool hasMarkersGlobal	= (nrn_original.CountGlobalMarkers() > 0);
			const bool hasMarkersSoma	= (nrn_original.CountSomaMarkers() > 0);
			const bool hasMarkersTree = (nrn_original.CountDendriteMarkers() + nrn_original.CountAxonMarkers() > 0);
			const bool hasMarkerNames = (numMarkers > 0) && DoesFormatSupportMarkers(origFormat, kNamed);


			u32 maxSamplesFor1Soma = 0;
			bool hasPointSoma = false;
			bool hasExtendedSoma = false;
			bool has1or2PtsSoma = false;
			for(Neuron3D::SomaConstIterator sit=nrn_original.SomaBegin();sit!=nrn_original.SomaEnd();++sit)
			{
				maxSamplesFor1Soma = (*sit).m_samples.size();

				if(maxSamplesFor1Soma <=2)
					has1or2PtsSoma = true;

				if(maxSamplesFor1Soma == 1)
					hasPointSoma = true;
				else
					hasExtendedSoma = true;
			}


			u8 somaSupportRequired =		(numSomas == 0 ? kSomaNone : 0) |
											(numSomas > 1 ? kSomaMultiple : 0) |
											(maxSamplesFor1Soma > 1 ? kSomaContour : kSomaPoint) |
											( (hasPointSoma && hasExtendedSoma) ? kSomaMixed : 0) |
											(has1or2PtsSoma ? kSoma1or2Pts : 0);
			//


			HintOptions hintOptions;
			StyleOptions styleOptions;
			styleOptions.appName = "Test_NLMorphologyConverter";
			styleOptions.appVersion = "1.0";
			styleOptions.fileName = filename;

			ExportReimportResult results;

			pOut->writeLine("    Neuron Properties: binary - " + Bool2Str(binaryAxons&&binaryDends) + ", axons - " + std::to_string(numAxons) + ", dendrites - " + std::to_string(numDendrites) + ", somas - " + std::to_string(numSomas) + ", markers - " + std::to_string(numMarkers) + ", spines - " + std::to_string(numSpines));

			pOut->writeLine("    Test Symmetric re-import:");
			const u32 numSupportedVariations = GetNumberOfExportVariationsSupported(origFormat);
			for(u32 n=0;n<numSupportedVariations;++n)
			{
				// special case HOC hack (NTSCable)
				bool testAxonsHack = !(origFormat == kMorphologyFormatNeuronHOC && n==1);
				//

				// Eutectics - spines can be imported but not exported..
				bool testSpinesHack = !(origFormat == kMorphologyFormatEutectics);
				//

				// NeurolucidaASCClassic - cannot yet guarantee the marker name ids..
				bool testMarkerNamesHack = !(origFormat == kMorphologyFormatNeurolucidaASCClassic);
				//


				ConfigureStyleOptionsForVariation(styleOptions, origFormat, n);
				const String variationName = GetVariationName(origFormat, n);
				
				String outputFileName = testDirectory + filename;
				outputFileName += ".testfile." + GetBuiltInMorphologyFormatDetails(origFormat).GetName() + "." + variationName;

				const u32 testFlags =	(testAxonsHack ? kTestFlagAxons : 0)|
										kTestFlagSomas|
										kTestFlagSpinesGlobal|
										(testSpinesHack ? kTestFlagSpinesTree : 0)|
										kTestFlagMarkersGlobal|
										kTestFlagMarkersSoma|
										kTestFlagMarkersTree|
										(testMarkerNamesHack ? kTestFlagMarkerNames : 0);

				results = DoExportReimportTest(nrn_original, outputFileName, origFormat, testFlags, hintOptions, styleOptions);
				pOut->writeLine("                                " + AppendSpaces("("+variationName+ ")", 24) + ":  export - " + Bool2Str(results.exported) + ", reimport - " + Bool2Str(results.reimported) + ", neuron-match - " + CompareResult2String(results.neuronsIdentical));
			}

			pOut->writeLine("    Test Asymmetric re-import:");
			for(u32 n=0;n<kMorphologyFormatBuiltInNumber;++n)
			{
				BuiltInMorphologyFormat outputFormat = (BuiltInMorphologyFormat)n;

				if(outputFormat == origFormat)
					continue;

				if(IsFormatActiveForTesting(outputFormat))
				{
					// only test components which are equally well supported by both formats
					const bool canCompareAxons = binaryAxons || !IsFormatBinaryOnly(outputFormat);
					const bool canCompareDends = binaryDends || !IsFormatBinaryOnly(outputFormat);

					const bool testAxons			= canCompareAxons && CanCompareAxons(origFormat, outputFormat);
					const bool testDendrites		= canCompareDends && CanCompareDendrites(origFormat, outputFormat);
					const bool testSomas			= DoesFormatSupportSomas(outputFormat, somaSupportRequired);

					const bool testDendriteType		= testDendrites && CanCompareDendriteType(origFormat, outputFormat);
					const bool testTerminalType		= (testDendrites || testAxons) && CanCompareTerminalType(origFormat, outputFormat);

					const u32 markerSupport			= GetFormatMarkerSupport(outputFormat);
					const u32 spineSupport			= GetFormatSpineSupport(outputFormat);

					const bool testMarkersGlobal	= (markerSupport & kGlobal) != 0;
					const bool testMarkersSoma		= (markerSupport & kSoma) != 0;
					const bool testMarkersTree		= (markerSupport & kBranch) != 0;
					const bool testMarkerNames		= hasMarkerNames && (markerSupport & kNamed);

					const bool testSpinesGlobal		= (spineSupport & kGlobal) != 0;
					const bool testSpinesTree		= (spineSupport & kBranch) != 0;
					const bool testSpinesRootSample0 = (spineSupport & kRootSample0) != 0;

					const bool ignoringAxons		= (numAxons > 0) && !testAxons;
					const bool ignoringDendrites	= (numDendrites > 0) && !testDendrites;
					const bool ignoringSomas		= (numSomas > 0) && !testSomas;
					const bool ignoringMarkersGlobal= (hasMarkersGlobal) && !testMarkersGlobal;
					const bool ignoringMarkersSoma	= (hasMarkersSoma) && !testMarkersSoma;
					const bool ignoringMarkersTree	= (hasMarkersTree) && !testMarkersTree;
					const bool ignoringSpinesGlobal	= (hasSpinesGlobal) && !testSpinesGlobal;
					const bool ignoringSpinesTree	= (hasSpinesTree) && !testSpinesTree;
					const bool ignoringDendTypes	= (numDendrites > 0) && !testDendriteType;
					const bool ignoringTerminalTypes = (numDendrites+numAxons > 0) && !testTerminalType;

					const bool testingSomethingNonSoma =	(numAxons > 0 && testAxons) ||
															(numDendrites > 0 && testDendrites) ||
															(hasMarkersGlobal && testMarkersGlobal) ||
															(hasMarkersSoma && testMarkersSoma) ||
															(hasMarkersTree && testMarkersTree) ||
															(hasSpinesGlobal && testSpinesGlobal) ||
															(hasSpinesTree && testSpinesTree);

					const bool ignoringStructure	=	ignoringAxons || ignoringDendrites || ignoringSomas || 
														ignoringMarkersGlobal || ignoringMarkersSoma || ignoringMarkersTree || 
														ignoringSpinesGlobal || ignoringSpinesTree || ignoringDendTypes || ignoringTerminalTypes;


					const bool testingSomething		=	testingSomethingNonSoma || 
														(numSomas > 0 && testSomas);

					const u32 testFlags				=	(testAxons ? kTestFlagAxons : 0) |
														(testDendrites ? kTestFlagDends : 0) |
														(testDendriteType ? kTestFlagDendType : 0) |
														(testTerminalType ? kTestFlagTerminalType : 0) |
														(testSomas ? kTestFlagSomas : 0) |
														(testMarkersGlobal ? kTestFlagMarkersGlobal : 0) |
														(testMarkersSoma ? kTestFlagMarkersSoma : 0) |
														(testMarkersTree ? kTestFlagMarkersTree : 0) |
														(testSpinesGlobal ? kTestFlagSpinesGlobal : 0) |
														(testSpinesTree ? kTestFlagSpinesTree : 0) |
														(testSpinesRootSample0 ? kTestFlagSpinesRootBranchSample0 : 0) |
														(testMarkerNames ? kTestFlagMarkerNames : 0);


					String untestedStr;
					if(ignoringStructure)
					{
						untestedStr = "     (Ignoring:";

						if(ignoringAxons)
							untestedStr += " axons";
						else
						{
							if(ignoringTerminalTypes)
								untestedStr += " axonterms";
						}

						if(ignoringDendrites)
							untestedStr += " dends";
						else 
						{
							if(ignoringDendTypes)
								untestedStr += " dendtypes";
							if(ignoringTerminalTypes)
								untestedStr += " dendterms";
						}

						if(ignoringSomas)
							untestedStr += " somas";
						if(ignoringMarkersGlobal || ignoringMarkersSoma || ignoringMarkersTree)
						{
							untestedStr += " markers(";
							if(ignoringMarkersGlobal)
								untestedStr += "G";
							if(ignoringMarkersSoma)
								untestedStr += "S";
							if(ignoringMarkersTree)
								untestedStr += "T";
							untestedStr += ")";
						}
						if(ignoringSpinesGlobal || ignoringSpinesTree)
						{
							untestedStr += " spines(";
							if(ignoringSpinesGlobal)
								untestedStr += "G";
							if(ignoringSpinesTree)
								untestedStr += "T";
							untestedStr += ")";
						}

						untestedStr += ")";
					}

					if(!testingSomething)
					{
						pOut->writeLine("        " + AppendSpaces(GetBuiltInMorphologyFormatDetails(outputFormat).GetName(), 48) + ":  No compatible data, ignoring this format.");
					}
					else
					{
						const u32 numSupportedVariations = GetNumberOfExportVariationsSupported(outputFormat);
						for(u32 n=0;n<numSupportedVariations;++n)
						{
							// special case HOC hack (NTSCable)
							u32 testFlagsWithHack = testFlags;
							bool testAxonsHack = !(outputFormat == kMorphologyFormatNeuronHOC && n==1);
							if(!testAxonsHack)
								testFlagsWithHack &= (~kTestFlagAxons);
							//

							ConfigureStyleOptionsForVariation(styleOptions, outputFormat, n);

							String variationName = GetVariationName(outputFormat, n);
							
							String outputFileName = testDirectory + filename;
							outputFileName += ".testfile." + GetBuiltInMorphologyFormatDetails(outputFormat).GetName() + "." + variationName;

							results = DoExportReimportTest(nrn_original, outputFileName, outputFormat, testFlagsWithHack, hintOptions, styleOptions);
							pOut->writeLine("        " + AppendSpaces(GetBuiltInMorphologyFormatDetails(outputFormat).GetName(), 24) + ( numSupportedVariations > 1 ? AppendSpaces("("+variationName+ ")", 24) : AppendSpaces("", 24) ) +  ":  export - " + Bool2Str(results.exported) + ", reimport - " + Bool2Str(results.reimported) + ", neuron-match - " + CompareResult2String(results.neuronsIdentical) + untestedStr);
						}
					}
				}
				else if(IsFormatFull3D(outputFormat))
				{
					pOut->writeLine("        " + AppendSpaces(GetBuiltInMorphologyFormatDetails(outputFormat).GetName(), 48) + ":  Format not currently included in testing");
				}
			}
		}

		pOut->writeLine(""); // empty line (to separate results if we are going to concatenate the logs..)
	}

}
