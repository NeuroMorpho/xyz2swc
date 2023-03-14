//
//
//

#include "ImportExportCommonIncludes.h"

#include <functional>
#include <algorithm>

#include "Core/Parser.h"

// we use neurolucida colours to specify export colours..
#include "NeurolucidaColours.h"
//

enum FieldType
{
	kLayer,				kStructure,			kColor,					kDisplayMode,
	kClosedContour,		kOpenContour,		
	kArborWithDiameter,	kArbor,	
	kPoint,

	kNumFieldTypes,
	kFieldTypeError
};


static const char* gs_fieldNames[] = 
{
	"LAYER",				"STRUCTURE",			"COLOR",	"DISPLAY_MODE",
	"CLOSED_CONTOUR",		"OPEN_CONTOUR",
	"ARBOR_WITH_DIAMETER",	"ARBOR_WITH_DIAMETER",
	"POINT"
};

FieldType FieldNameToFieldType(const String& name)
{
	for(u32 n=0;n<kNumFieldTypes;++n)
		if(name == gs_fieldNames[n])
			return (FieldType)n;

	return kFieldTypeError;
}

enum StructureType
{
	kStructure_Unknown,
	kStructure_Soma,
	kStructure_Dendrite,
	kStructure_Axon,
	kStructure_Spines,
	kStructure_Markers
};

struct StructureColor
{
	u32 r,g,b;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

static const HString kConnectionTagID = HString("connection");
static const HString kOffsetTagID = HString("offset!!");
static const HString kArborTagID = HString("arbor");

class NeuroZoomParser : public SimpleParser
{
private:
	MorphologyBuilder builder;
	Neuron3D& m_nrn;
	HintOptions mOptions;

public:
	NeuroZoomParser(Input& rIn, Neuron3D &nrn, HintOptions options) : SimpleParser(rIn), builder(nrn), m_nrn(nrn), mOptions(options)
	{
	}


	StructureType StructureName2Type(const String& s)
	{
		String somaTest = mOptions.hasHintSoma ? mOptions.hintSomaSubString : "soma";
		String axonTest = mOptions.hasHintAxon ? mOptions.hintAxonSubString : "axon";
		String dendTest = mOptions.hasHintDend ? mOptions.hintDendSubString : "dend";

		if(StringContains(s, somaTest))
			return kStructure_Soma;

		if(StringContains(s, axonTest))
			return kStructure_Axon;

		if(StringContains(s, dendTest))
			return kStructure_Dendrite;

		if(StringContains(s, "spine"))
			return kStructure_Spines;

		if(StringContains(s, "marker"))
			return kStructure_Markers;

		return kStructure_Unknown;
	}

	void ExtractDataPoint(SamplePoint& sample)
	{
		String item;

		SeekNextItem();
		item = GetNextItem();
		sample.x = std::stof( item );

		SeekNextItem();
		item = GetNextItem();
		sample.y = std::stof(item);

		SeekNextItem();
		item = GetNextItem();
		sample.z = std::stof(item);
	}

	bool ExtractContour(u32 numPoints)
	{
		for(u32 n=0;n<numPoints;++n)
		{
			SamplePoint sample;
			ExtractDataPoint(sample);

			builder.CsAddSample(sample);
		}

		return true;
	}

	bool ExtractBranch(u32 numPoints, vector<SamplePoint>& samples)
	{
		String item;
		SamplePoint sample;
		for(u32 n=0;n<numPoints;++n)
		{
			ExtractDataPoint(sample);
			samples.push_back(sample);
		}

		for(u32 n=1;n<numPoints;++n)
		{
			SeekNextItem();
			item = GetNextItem();
			samples[n].d = std::stof(item);
		}

		if(numPoints > 1)
		{
			samples[0].d = samples[1].d;
		}
		//else
			//printf("1-sample tree!!\n");

		return true;
	}

	void AddSamplesToCurrentBranch(const vector<SamplePoint>& samples)
	{
		for(vector<SamplePoint>::const_iterator it = samples.begin(); it != samples.end();++it)
			builder.CbAddSample(*it);
	}

	void TagEntireArbor(HString kTagID, u32 tag)
	{
		const u32 originalArborId = builder.CbGetTag(kArborTagID);

		MorphologyBuilder::Branch currentBranch = builder.CurrentBranch();

		// down the tree
		MorphologyBuilder::Branch testBranch = currentBranch;
		do
		{
			builder.SetTagForBranch(testBranch, kTagID, tag);
			testBranch = testBranch.GetParent();
		}
		while(	testBranch != MorphologyBuilder::NullBranch() &&
				builder.GetTagForBranch(testBranch, kArborTagID) == originalArborId );

		// up the tree - assumes arbor continues on first child, if it continues at all....(should be true)
		testBranch = currentBranch.GetChild();
		while(	testBranch != MorphologyBuilder::NullBranch() &&
				builder.GetTagForBranch(testBranch, kArborTagID) == originalArborId )
		{
			builder.SetTagForBranch(testBranch, kTagID, tag);
			testBranch = testBranch.GetChild();
		}
	}


	bool Parse()
	{
		String currentStructureName;
		StructureType currentStructure = kStructure_Unknown;

		u32 arborCount = 0;

		while(rIn.remaining() > 0)
		{
			String item = GetNextItem();
			FieldType field = FieldNameToFieldType(item);

			switch(field)
			{
			case kLayer:
			case kColor:
			case kDisplayMode:
				SeekNextLine();
				break;

			case kStructure:
				SeekNextItem();
				currentStructureName = GetRestOfLine();
				currentStructure = StructureName2Type(currentStructureName);
				break;

			case kOpenContour:
				if(currentStructure != kStructure_Soma)
					LogImportWarning("Found open contour for non-soma structure!?");

				builder.NewSomaOpenContour();

				SeekNextItem();
				ExtractContour( std::stoi(GetNextItem()));
				break;

			case kClosedContour:
				if(currentStructure != kStructure_Soma)
					LogImportWarning("Found closed contour for non-soma structure!?");

				builder.NewSomaClosedContour();

				SeekNextItem();
				ExtractContour(std::stoi(GetNextItem()));
				break;

			case kArborWithDiameter:
				{
					++arborCount;

					u32 numPoints;
					int connectionPoint;
					u32 numConnections;

					SeekNextItem();
					item = GetNextItem();
					numPoints = std::stoi(item);

					SeekNextItem();
					item = GetNextItem();
					if(item == "nil")
						connectionPoint = -1;
					else
						connectionPoint = std::stoi(item);

					SeekNextItem();
					item = GetNextItem();
					numConnections = std::stoi(item);

					// extract the branch sample data ahead of fixing up the morphology
					vector<SamplePoint> samples;
					ExtractBranch(numPoints, samples);

					//
					if(connectionPoint == -1)
					{
						builder.ClearTags();
						
						if(currentStructure == kStructure_Axon)
							builder.NewAxon();
						else
							builder.NewDendrite();

						builder.CbSetTag(kConnectionTagID, numConnections);
						builder.CbSetTag(kOffsetTagID, 0);
						builder.CbSetTag(kArborTagID, arborCount);
						AddSamplesToCurrentBranch(samples);
					}
					else
					{
						u32 numOriginalBranchConnections = builder.CbGetTag(kConnectionTagID);
						u32 originalBranchArborId = builder.CbGetTag(kArborTagID);
						u32 connectionPointOffset = builder.CbGetTag(kOffsetTagID);
						int localConnectionPoint = connectionPoint - connectionPointOffset;

						// if the tree data is not well ordered, we have to look up the 
						// branch to which we need to connect
						if(localConnectionPoint < 0)
						{
							while(connectionPoint < connectionPointOffset)
							{
								builder.CbToParent();
								assert(builder.CbGetTag(kArborTagID) == originalBranchArborId);
								connectionPointOffset = builder.CbGetTag(kOffsetTagID);
								localConnectionPoint = connectionPoint - connectionPointOffset;
							}
						}
						else if(localConnectionPoint > builder.CurrentBranch().NumSamples()-1)
						{
							while(localConnectionPoint > builder.CurrentBranch().NumSamples()-1)
							{
								builder.CbToChild();
								while(builder.CbGetTag(kArborTagID) != originalBranchArborId)
									builder.CbToPeer();
								
								connectionPointOffset = builder.CbGetTag(kOffsetTagID);
								localConnectionPoint = connectionPoint - connectionPointOffset;
							}
						}

						const bool internalPoint	= (localConnectionPoint > 0 && localConnectionPoint < builder.CurrentBranch().NumSamples()-1);
						const bool startPoint		= (localConnectionPoint == 0);
						const bool endPoint			= (localConnectionPoint == builder.CurrentBranch().NumSamples()-1);

						const u32 pendingConnectionsForThisArbor = numOriginalBranchConnections - 1;

						// normal branch point - need to split current branch so we can insert it...
						if(internalPoint)
						{
							builder.CbSplit(localConnectionPoint);

							builder.CbSetTag(kConnectionTagID, pendingConnectionsForThisArbor);
							builder.CbSetTag(kOffsetTagID, connectionPoint); // not necessary...but shouldn't matter..
							builder.CbSetTag(kArborTagID, originalBranchArborId);

							builder.CbToParent();
						}
						// non-binary branch point! no need to split - be careful..
						else if(startPoint)
						{
							assert( builder.CbGetTag(kOffsetTagID) == connectionPoint);

							builder.CbSetTag(kConnectionTagID, pendingConnectionsForThisArbor);
							builder.CbSetTag(kOffsetTagID, connectionPoint); // not necessary...but shouldn't matter..

							builder.CbToParent();
						}
						// else: continuation of current branch - not necessarily a real branch point...but we will treat it like a new branch point anyway...
						//else if(endPoint) 
						//{
						//	builder.CbSetTag(kConnectionTagID, pendingConnectionsForThisArbor);
						//}

						// previously we just re-tagged current branch - need to do entire arbor to cope with unordered data..
						TagEntireArbor(kConnectionTagID, pendingConnectionsForThisArbor);

						// fresh new branch
						builder.CbNewChildLast(false);
						builder.CbSetTag(kConnectionTagID, numConnections);
						builder.CbSetTag(kOffsetTagID, 0);
						builder.CbSetTag(kArborTagID, arborCount);

						// 
						//samples[0].d = builder.CurrentBranch().GetParent().GetLastSample().d;
						samples[0] = builder.CurrentBranch().GetParent().GetLastSample();
						AddSamplesToCurrentBranch(samples);

						if(numConnections == 0)
						{
							if( builder.CbAscendToBranchWithTagCondition(kConnectionTagID, std::bind2nd(not_equal_to<u32>(),0) ) )
							{
								// need this check, for cases where arbor was directly attached to end of parent arbor.
								//if(builder.CurrentBranch().GetChild().GetPeer() != MorphologyBuilder::NullBranch())
								//{
								//	builder.CbToChild();
								//}
							}
						}
					}
				}
				break;

			case kPoint:
				{
					SeekNextItem();
					String xstr = GetNextItem();
					SeekNextItem();
					String ystr = GetNextItem();
					SeekNextItem();
					String zstr = GetNextItem();

					if(currentStructure == kStructure_Spines)
					{
						SamplePoint pt(std::stof(xstr), std::stof(ystr), std::stof(zstr), 0.0f );
						SpinePoint sp(pt, kSpineNoDetail, 0);
						builder.AddSpine(sp);
					}
					else
					{
						builder.AddMarker(currentStructureName, MarkerPoint(std::stof(xstr), std::stof(ystr), std::stof(zstr), 0.0f ) );
					}
				}

				break;

			default:
				LogImportWarning( String("Unexpected FieldType: might be able to cope with it: ") + item );
				break;
			}

			SeekNextItem();

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
};

bool ImportNeuroZoom(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	NeuroZoomParser parser(rIn, nrn, options);
	return parser.Parse();
}


bool ImportRawNeuroZoom(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeuroZoom(*pIn, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

StructureColor ColourString2Colour(String colour)
{
	StructureColor result;
	u8 r,g,b;

	if(StringContains(colour, "RGB"))
	{
		Strings rgb = StringSplit( StringBefore( StringAfter(colour, '('), ')'), ':');

		r = std::stoi(rgb[0]);
		g = std::stoi(rgb[1]);
		b = std::stoi(rgb[2]);
	}
	else
	{
		NeurolucidaColour nlCol = NeurolucidaColourString2Id(colour);

		if(nlCol == kColour_Unknown)
			nlCol = kColour_White;

		GetNeurolucidaColourRGB(nlCol, r,g,b);
	}

	result.r = 256 * (u16)r;
	result.g = 256 * (u16)g;
	result.b = 256 * (u16)b;

	return result;
}



bool WriteStructureMinimal(Output& rOut, const String& name, StructureColor c)
{
	rOut.writeLine("STRUCTURE " + name);
	rOut.writeLine("COLOR " +	dec(c.r) + " " + dec(c.g) + " " + dec(c.b) + " " );
	rOut.writeLine("DISPLAY_MODE Use Mark");

	return true;
}

template<typename T>
void WritePoint(Output& rOut, const T& pt)
{
	rOut.writeLine("POINT");
	rOut.writeLine(	PrependSpaces( dec(pt.x, 2), 9) + "\t" +
					PrependSpaces( dec(pt.y, 2), 9) + "\t" +
					PrependSpaces( dec(pt.z, 2), 9) );
}

void WriteBranchSpinePoints(Output& rOut, Neuron3D::DendriteTreeConstIterator it)
{
	for(u32 i=0; i<(*it).m_spines.size();++i)
		WritePoint(rOut, (*it).m_spines[i]);

	if(it.child()) 
		WriteBranchSpinePoints(rOut, it.child());
	if(it.peer()) 
		WriteBranchSpinePoints(rOut, it.peer());
}

void WriteBranchMarkerPoints(Output& rOut, Neuron3D::DendriteTreeConstIterator it, StructureColor colour)
{
	for(Branch3D::Markers::const_iterator mit=(*it).m_markers.begin();mit!=(*it).m_markers.end();++mit)
	{
		WriteStructureMinimal(rOut, mit->first, colour);

		for(u32 i=0; i<mit->second.size();++i)
			WritePoint(rOut, (*mit).second[i]);
	}

	if(it.child()) 
		WriteBranchMarkerPoints(rOut, it.child(), colour);
	if(it.peer()) 
		WriteBranchMarkerPoints(rOut, it.peer(), colour);
}

void WriteSpinePointsNZ(Output& rOut, const Neuron3D& nrn, StructureColor colour)
{
	WriteStructureMinimal(rOut, "spine", colour);

	// global
	for(u32 i=0; i<nrn.m_globalData.m_spines.size();++i)
		WritePoint(rOut, nrn.m_globalData.m_spines[i]);

	// branch
/*	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		WriteBranchSpinePoints(rOut, (*d).root());

	for(Neuron3D::AxonConstIterator d = nrn.AxonBegin(); d != nrn.AxonEnd(); ++d)
		WriteBranchSpinePoints(rOut, (*d).root());*/
}

void WriteMarkerPointsNZ(Output& rOut, const Neuron3D& nrn, StructureColor colour)
{
	// global
	for(Branch3D::Markers::const_iterator mit=nrn.m_globalData.m_markers.begin();mit!=nrn.m_globalData.m_markers.end();++mit)
	{
		WriteStructureMinimal(rOut, mit->first, colour);

		for(u32 i=0; i<mit->second.size();++i)
			WritePoint(rOut, (*mit).second[i]);
	}

	// branch
/*	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		WriteBranchMarkerPoints(rOut, (*d).root(), colour);

	for(Neuron3D::AxonConstIterator d = nrn.AxonBegin(); d != nrn.AxonEnd(); ++d)
		WriteBranchMarkerPoints(rOut, (*d).root(), colour);*/
}

void WriteContour(Output& rOut, const vector<SamplePoint>& samples, bool closed)
{
	const u32 numSamplePoints = samples.size();

	rOut.writeLine( (closed ? String("CLOSED_CONTOUR ") : String("OPEN_CONTOUR ")) + dec(numSamplePoints));

	for(u32 n=0;n<numSamplePoints;++n)
	{
		rOut.writeLine(	PrependSpaces( dec(samples[n].x, 2), 9) + "\t" +
						PrependSpaces( dec(samples[n].y, 2), 9) + "\t" +
						PrependSpaces( dec(samples[n].z, 2), 9) );
	}
}

static bool WriteSomasNZ(Output& rOut, const Neuron3D& nrn, const StructureColor colour, const String& somaName)
{
	WriteStructureMinimal(rOut, somaName, colour);

	Neuron3D::SomaConstIterator s = nrn.SomaBegin();
	Neuron3D::SomaConstIterator end = nrn.SomaEnd();

	for(;s!=end;++s)
		WriteContour(rOut, (*s).m_samples, (s->somaType == Soma3D::kContourClosed) );

	return true;
}

static void WriteArborHeader(Output& rOut, u32 numPoints, int connectionNode, u32 numConnections)
{
	rOut.writeLine(String(gs_fieldNames[kArborWithDiameter]) + " " + dec(numPoints) + " " + (connectionNode == -1 ? "nil" : dec(connectionNode)) + " " + dec(numConnections) );
}

static void WriteArborSamplePointsAndDiameters(Output& rOut, vector<SamplePoint>& samples)
{
	// sample points
	for(u32 n=0;n<samples.size();++n)
		rOut.writeLine( dec(samples[n].x, 2) + " " + dec(samples[n].y, 2) + " " + dec(samples[n].z, 2));

	// sample diameters
	for(u32 n=1;n<samples.size();++n)
		rOut.writeLine(dec(samples[n].d, 6));
}

bool WriteArborLong(Output& rOut, Neuron3D::DendriteTreeConstIterator startIt, int connectionNode)
{
	Neuron3D::DendriteTreeConstIterator it = startIt;

	vector<SamplePoint> arborPoints;

	u32 numConnections = 0;

	// determine the properties of the arbor
	do
	{
		if(arborPoints.size() == 0)
			arborPoints.push_back( (*it).m_samples[0]);

		for(u32 n=1;n<(*it).m_samples.size();++n)
			arborPoints.push_back( (*it).m_samples[n]);

		if(it.child() && it.child().peer())
		{
			numConnections += (Neuron3D::DendriteTree::countchildren(it) - 1);
		}

		it = it.child();
	}
	while(it);


	WriteArborHeader(rOut, arborPoints.size(), connectionNode, numConnections);
	WriteArborSamplePointsAndDiameters(rOut, arborPoints);


	// recursively parse tree
	u32 numConnectionsToDo = numConnections;
	u32 arborConnectionNode = 0;

	while(numConnectionsToDo > 0)
	{
		arborConnectionNode += (*startIt).m_samples.size()-1;

		// get the root of the connecting arbor (in case we have peer-less children)
		while(startIt.child() && !startIt.child().peer())
		{
			startIt = startIt.child();
			arborConnectionNode += (*startIt).m_samples.size()-1;
		}
		//

		assert(startIt.child());

		Neuron3D::DendriteTreeConstIterator peerLooper = startIt.child().peer();

		do
		{
			WriteArborLong(rOut, peerLooper, arborConnectionNode);
			--numConnectionsToDo;
			peerLooper = peerLooper.peer();
		}
		while(peerLooper);

		startIt = startIt.child();
	}

	return true;
}

bool WriteArborShort(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int connectionNode)
{
	// todo - account for unary branch points!?

	const u32 numConnections = Neuron3D::DendriteTree::countchildren(it);
	const u32 numPoints = it->m_samples.size();


	WriteArborHeader(rOut, numPoints, connectionNode, numConnections);
	WriteArborSamplePointsAndDiameters(rOut, it->m_samples);


	if(it.child())
		WriteArborShort(rOut, it.child(), numPoints-1);

	if(it.peer())
		WriteArborShort(rOut, it.peer(), connectionNode);

	return true;
}


bool WriteTreesNZ(Output& rOut, const Neuron3D& nrn, const StructureColor colourDend, const StructureColor colourAxon, const String& dendName, const String& axonName, bool shortStyle)
{
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		WriteStructureMinimal(rOut, dendName, colourDend);

		if(shortStyle)
			WriteArborShort(rOut, (*d).root(), -1);
		else
			WriteArborLong(rOut, (*d).root(), -1);
	}

	for(Neuron3D::AxonConstIterator d = nrn.AxonBegin(); d != nrn.AxonEnd(); ++d)
	{
		WriteStructureMinimal(rOut, axonName, colourAxon);

		if(shortStyle)
			WriteArborShort(rOut, (*d).root(), -1);
		else
			WriteArborLong(rOut, (*d).root(), -1);
	}

	return true;
}


bool ExportNeuroZoom(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// configure
	const StructureColor colourSoma		= ColourString2Colour(options.somaColour);
	const StructureColor colourDend		= ColourString2Colour(options.dendColour);
	const StructureColor colourAxon		= ColourString2Colour(options.axonColour);
	const StructureColor colourSpine	= ColourString2Colour(options.spineColour);
	const StructureColor colourMarker	= ColourString2Colour(options.markerColour);

	const bool shortStyle = (options.formatStyle == kStyleNeuroZoom_ShortPath);

	const String nameSoma = options.customiseSomaName ? options.somaName : "soma";
	const String nameAxon = options.customiseAxonName ? options.axonName : "axon";
	const String nameDend = options.customiseDendName ? options.dendName : "dendrite";

	// note - no comments

	//
	//rOut.writeLine("LAYER Layer 1");

	WriteSomasNZ(rOut, nrn, colourSoma, nameSoma);

	WriteTreesNZ(rOut, nrn, colourDend, colourAxon, nameDend, nameAxon, shortStyle);

	WriteSpinePointsNZ(rOut, nrn, colourSpine);

	WriteMarkerPointsNZ(rOut, nrn, colourMarker);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeuroZoom(Input& rIn, const ValidationOptions& options)
{
	String line;

	u32 lineCount = 0;
	const u32 maxLines = 2;

	while(rIn.remaining() && lineCount < maxLines)
	{
		line = rIn.readLine();

		bool validName = false;
		for(u32 n=0;n<kNumFieldTypes;++n)
		{
			if( StringBegins(line, gs_fieldNames[n]) )
			{
				validName = true;
				break;
			}
		}

		if(!validName)
			return kValidationFalse;

		++lineCount;
	}

	return kValidationTrue;
}

