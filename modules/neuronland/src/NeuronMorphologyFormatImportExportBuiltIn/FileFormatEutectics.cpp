//
//
//

#include "ImportExportCommonIncludes.h"

#include <algorithm>
#include <math.h>

#include "Core/Parser.h"

/**

	Eutectics format

	Structure supported ->
		Soma		: MULTIPLE OPEN CONTOUR
		Dendrite	: BINARY TREE
		Axon		: BINARY TREE
		Marker		: NO*
		Spine		: SPINE BASES (import only)

	Note: axons and dendrites are only distinguished by arbitrary naming convention

	Pending:
	* Need more examples.
	* Could treat DS, DCP, DE as marker data ?
	* Check 6-char structure name limit

**/

static char gs_EutecticHeader[] =	" Point  Type  Tag      X       Y       Z  Thick   Name    Attachment";

const String kDefaultSomaPrefix("cell");
const String kDefaultDendPrefix("d");
const String kDefaultAxonPrefix("ax");
const String kTreeRootAttachmentText("------     0");

enum EutecticPointType
{
	MTO,	TTO,	BTO,
	CP,		FS,		SB,
	BP,		NE,		ES,
	MAE,	TAE,	BAE,
	SOS,	SCP,	SOE,
	OS,		OCP,	OE,
	DS,		DCP,	DE,

	kPointTypeNumber,
	kPointTypeError
};

const char* gs_EutecticPointNames[] = 
{
	"MTO",	"TTO",	"BTO",
	" CP",	" FS",	" SB",	
	" BP",	" NE",	" ES",
	"MAE",	"TAE",	"BAE",
	"SOS",	"SCP",	"SOE",
	" OS",	"OCP",	" OE",
	" DS",	"DCP",	" DE"
};

static EutecticPointType PointNameToType(const String& name)
{
	for(u32 n=0;n<kPointTypeNumber;++n)
		if(name == gs_EutecticPointNames[n])
			return (EutecticPointType)n;

	return kPointTypeError;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class EutecticsParser : public SimpleParser
{
public:
	EutecticsParser(Input& rIn, Neuron3D &nrn, const String& str, bool allcontours) : SimpleParser(rIn), m_nrn(nrn), axonSubstring(str), allContoursAsSomas(allcontours)
	{
	}

	bool Parse()
	{
		MorphologyBuilder builder(m_nrn);

		bool doingSoma = false;
		bool addChildToCurrentBranch = false;

		while(rIn.remaining() > 0)
		{
			String line = rIn.readLine();

			if(line == "" || line.length() < 10 || line == gs_EutecticHeader || StringBegins(line, "No. points") || StringBegins(line, "No. trees") || StringBegins(line, "Text"))
				continue;

			const char* linePtr = line.c_str();

			const u32 pointNumber =					std::stoi( String(linePtr, 8) );
			const EutecticPointType pointType =		PointNameToType( String(linePtr+8, 3) );
			const float x =							std::stof(String(linePtr + 16, 8) );
			const float y =							std::stof(String(linePtr + 24, 8) );
			const float z =							std::stof(String(linePtr + 32, 8) );
			const float d =							std::stof(String(linePtr + 40, 8) );
			String name =							String(linePtr + 48, 8);

			SamplePoint dataPoint(x, y, z, d);

			if(doingSoma && (
				(pointType != SOS && pointType != SCP && pointType != SOE) || 
				(allContoursAsSomas && (pointType != OS && pointType != OCP && pointType != OE && pointType != DS && pointType != DCP && pointType != DE) ) ) )
			{
				LogImportWarning("Doing soma, but contour not terminated as expected. Trying to continue");
				doingSoma = false;
			}


			if(	pointType == MTO ||
				pointType == TTO ||
				pointType == BTO ||
				pointType == SOS ||
				pointType == SCP ||
				pointType == SOE ||
				(allContoursAsSomas && (pointType == OS || pointType == OCP || pointType == OE) ) || 
				(allContoursAsSomas && (pointType == DS || pointType == DCP || pointType == DE) ) )
			{
				addChildToCurrentBranch = false;
			}

			if(addChildToCurrentBranch)
			{
				addChildToCurrentBranch = false;
				builder.CbNewChildLast(true);
			}

			switch(pointType) 
			{
				case MTO: 
				case BTO: 
				case TTO: 

					if(StringContains(name, axonSubstring))
						builder.NewAxonWithSample(dataPoint);
					else
						builder.NewDendriteWithSample(dataPoint);


					break;

				case BP:
					builder.CbAddSample(dataPoint);
					builder.CbNewChildLast(true);
					break;

				case FS:
				case SB:
				case CP: 
					if(doingSoma)
						builder.CsAddSample(dataPoint);
					else
					{
						if(builder.CurrentBranch())
						{
							builder.CbAddSample(dataPoint);

							// add a spine also
							if(pointType == SB)
							{
								if(doingSoma)
								{
									LogImportWarning("Encountered spine base while constructing soma! - ignoring data.");
								}
								else
								{
									builder.CbAddSpine( SpinePoint(dataPoint, kSpineNoDetail, builder.CurrentBranch().NumSamples() - 1) );
								}
							}
						}
					}
					break;

				case NE:
				case ES:
				case MAE:
				case TAE:
				case BAE: 
					builder.CbAddSample(dataPoint);
					if(builder.CbAscendToUnaryBranchPoint())
						addChildToCurrentBranch = true;
					break;

				case OS:
				case DS:
					if(!allContoursAsSomas)
					{
						LogImportWarning( String("Ignoring non-soma Outline/Dot point type: ") + std::to_string(pointType) );
						break;
					}
					// deliberate fall-through
				case SOS:
					doingSoma = true;
					builder.NewSomaOpenContour();
					builder.CsAddSample(dataPoint);
					break;

				case OCP:
				case DCP:
					if(!allContoursAsSomas)
					{
						LogImportWarning( String("Ignoring non-soma Outline/Dot point type: ") + std::to_string(pointType) );
						break;
					}
					// deliberate fall-through
				case SCP:
					builder.CsAddSample(dataPoint);
					break;

				case OE:
				case DE:
					if(!allContoursAsSomas)
					{
						LogImportWarning( String("Ignoring non-soma Outline/Dot point type: ") + std::to_string(pointType) );
						break;
					}
					// deliberate fall-through
				case SOE:
					builder.CsAddSample(dataPoint);
					doingSoma = false;
					break;

				default:
					LogImportFailure( String("File format error - unknown point type: ") + std::to_string(pointType) );
					return false;
			}

			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}

		builder.ApplyChanges();

		m_nrn.MergeAllUnaryBranchPoints();
		m_nrn.EnhanceSingleSampleTrees();

		return true;
	}

private:
	Neuron3D& m_nrn;
	String axonSubstring;
	bool allContoursAsSomas;
};

bool ImportEutectics(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	EutecticsParser parser(rIn, nrn, options.hasHintAxon ? options.hintAxonSubString : kDefaultAxonPrefix, options.allContoursAsSoma);

	return parser.Parse();
}


bool ImportRawEutectics(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportEutectics(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

static String StripZero(const String& str)
{
	if(StringBegins(str, "0"))
		return StringAfter(str, "0");
	else if(StringBegins(str, "-0"))
		return "-" + StringAfter(str, "-0");
	else 
		return str;
}

void WriteEutecticsDataPoint(Output& rOut, u32 segmentcount, EutecticPointType type, const SamplePoint& sample, const String& id = "")
{
	rOut.writeLine( PrependSpaces(dec(segmentcount), 6)		+
					PrependSpaces(gs_EutecticPointNames[type], 5)	+
					String("    0") + 
					PrependSpaces( StripZero(dec(sample.x, 1)), 8)		+
					PrependSpaces( StripZero(dec(sample.y, 1)), 8)		+
					PrependSpaces( StripZero(dec(sample.z, 1)), 8)		+
					PrependSpaces( StripZero(dec(sample.d < 999.9 ? sample.d : 999.9f, 1)), 6)		+
					id);
}

void WriteEutecticsHeader(Output& rOut, u32 numPoints, u32 numTrees, const String& text)
{
	rOut.writeLine("No. points" + PrependSpaces(dec(numPoints), 7));
	rOut.writeLine("No. trees " + PrependSpaces(dec(numTrees), 7));
	rOut.writeLine("Text  " + text);
	rOut.writeLine("");
	rOut.writeLine( String(gs_EutecticHeader) );
	rOut.writeLine("");
}

int ExportEutecticsSoma(Output& rOut, const Neuron3D &nrn, int segment, const String& nameprefix)
{
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s!=nrn.SomaEnd(); ++s)
	{
		const Soma3D& soma = (*s);
		const u32 numPoints = soma.m_samples.size();

		String id;
		for(u32 i=0;i<numPoints;++i)
		{
			id = "";

			EutecticPointType type = SCP;
			if(i == 0)
			{
				type = SOS;
				id = String("  ") + AppendSpaces(nameprefix, 6);
			}
			// make sure this doesn't override above if numPoints == 1
			else if (i == numPoints-1)
			{
				type = SOE;
			}

			++segment;
			WriteEutecticsDataPoint(rOut, segment, type, soma.m_samples[i], id);
		}
	}
	return segment;
}

int ExportEutecticsBranch(Output& rOut, Neuron3D::DendriteTree::const_iterator it, int segment)
{
	for(u32 i=1; i<(*it).m_samples.size();++i)
	{
		EutecticPointType type = CP;

		if(i == (*it).m_samples.size() - 1)
		{
			if(it.child())
			{
				if(it.child().peer())
					type = BP;
			}
			else
				type = NE;
		}

		++segment;
		WriteEutecticsDataPoint(rOut, segment, type, (*it).m_samples[i]);
	}

	if(it.child()) 
		segment = ExportEutecticsBranch(rOut, it.child(), segment);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it) )
		segment = ExportEutecticsBranch(rOut, it.peer(), segment);

	return segment;
}

int ExportEutecticsDendrites(Output& rOut, const Neuron3D &nrn, int pointCount, const String& dendPrefix)
{
	u32 dendriteCount = 0;

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		++dendriteCount;
		++pointCount;

		String id = AppendSpaces(String("  ") + dendPrefix + dec(dendriteCount), 10) + kTreeRootAttachmentText;
		WriteEutecticsDataPoint(rOut, pointCount, MTO, (* (*d).root() ).m_samples[0], id);

		pointCount = ExportEutecticsBranch(rOut, (*d).root(), pointCount);
	}

	return pointCount;
}

int ExportEutecticsAxons(Output& rOut, const Neuron3D &nrn, int pointCount, const String& axonPrefix)
{
	u32 axonCount = 0;

	for(Neuron3D::AxonConstIterator d = nrn.AxonBegin(); d != nrn.AxonEnd(); ++d)
	{
		++axonCount;
		++pointCount;

		String id = AppendSpaces(String("  ") + axonPrefix + dec(axonCount), 10) + kTreeRootAttachmentText;
		WriteEutecticsDataPoint(rOut, pointCount, MTO, (* (*d).root() ).m_samples[0], id);

		pointCount = ExportEutecticsBranch(rOut, (*d).root(), pointCount);
	}

	return pointCount;
}

bool ExportEutectics(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// configure
	String somaPrefix = options.customiseSomaName ? options.somaName : kDefaultSomaPrefix;
	String dendPrefix = options.customiseDendName ? options.dendName : kDefaultDendPrefix;
	String axonPrefix = options.customiseAxonName ? options.axonName : kDefaultAxonPrefix;

	const u32 numPoints = nrn.CountTotalSamplePoints();
	const u32 numTrees = nrn.CountDendrites() + nrn.CountAxons() + nrn.CountSomas();

	String text;

	if(options.identifyNL)
		text = "File written by " + options.appName + "(version" + options.appVersion + " ). " + options.commentText;
	else if(options.addCommentText)
		text = options.commentText;
	//


	WriteEutecticsHeader(rOut, numPoints, numTrees, text);

	int pointCount = 0;
	pointCount = ExportEutecticsSoma(rOut, nrn, pointCount, somaPrefix);
	pointCount = ExportEutecticsDendrites(rOut, nrn, pointCount, dendPrefix);
	pointCount = ExportEutecticsAxons(rOut, nrn, pointCount, axonPrefix);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateEutectics(Input& rIn, const ValidationOptions& options)
{
	String line1;
	String line2;
	String line3;

	// get first three lines...ignore empty lines (to cope with NeroMorpho:barrionuevo files)
	while( rIn.remaining() && (line1 = rIn.readLine()) == "");
	while( rIn.remaining() && (line2 = rIn.readLine()) == "");
	while( rIn.remaining() && (line3 = rIn.readLine()) == "");
	//

	if(	StringBegins(line1, "No. points") &&
		StringBegins(line2, "No. trees") &&
		StringBegins(line3, "Text"))
	{
		u32 linecount = 0;
		while(linecount < 20 && rIn.remaining() > 0)
		{
			String line = rIn.readLine();

			if(line == gs_EutecticHeader)
			{
				return kValidationTrue;
			}

			++linecount;
		}
	}

	if(!options.strictValidation)
	{
		if( StringBegins(line1, "Text") )
			return kValidationTrue;
	}

	return kValidationFalse;
}

