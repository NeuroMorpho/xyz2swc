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

	AmiraMesh format

	Structure supported ->
		Soma		: NO
		Dendrite	: BINARY
		Axon		: BINARY
		Marker		: NO
		Spine		: NO

	Pending:
	* Add formatted time in comments.

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

struct EnhancedSampleData
{
	EnhancedSampleData() : nbrCount(0), connected(false) {}

	SamplePoint pt;
	u32 nbrCount;
	vector<u32> nbrList;
	bool connected;
};

u32 FindStartingPoint(std::vector<EnhancedSampleData>& sampleData)
{
	u32 n=0;
	for(;n<sampleData.size();++n)
	{
		if(!sampleData[n].connected && sampleData[n].nbrCount == 1)
			break;
	}

	return n;
}

void BuildTree(MorphologyBuilder& builder, MorphologyBuilder::Branch currentBranch, u32 parentSample, u32 currentPoint, std::vector<EnhancedSampleData>& sampleData)
{
	u32 nbrCount = sampleData[currentPoint].nbrCount;

	u32 connectedCount = 0;

	for(u32 n=0;n<nbrCount;++n)	
	{
		const u32 currentNbr = sampleData[currentPoint].nbrList[n];
		const bool connected = sampleData[currentNbr].connected;

		if(!connected)
		{
			sampleData[currentNbr].connected = true;

			builder.CbNewChildLast(true);
			builder.CbAddSample( sampleData[currentNbr].pt );
			BuildTree(builder, builder.CurrentBranch(), currentPoint, currentNbr, sampleData);
			builder.SetCurrentBranch(currentBranch);
		}
		else
		{
			//if(connectedCount > 0)
			//	LogImportFailure(String("Too many neighbours already connected") );

			++connectedCount;

			//if( currentNbr != parentSample )
			//	LogImportFailure(String("Inconsistent neighbour sample id") );
		}
	}
}

static String SeekLineBeginning(Input& rIn, const String& str)
{
	String line;
	while(rIn.remaining() > 0)
	{
		line = rIn.readLine();

		if( StringBegins(line, str) )
			return line;
	}

	return String("");
}

const HString kTagLastSample("LastSample");

class AmiraMeshParser : public SimpleParser
{
	MorphologyBuilder builder;

	vector<SamplePoint> samplePoints;

	Neuron3D& m_nrn;

public:
	AmiraMeshParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn), m_nrn(nrn)
	{
		SetIgnoreAfter('#');
		samplePoints.reserve(10000);
	}

	int ReadBranchConnectivity(String line, int maxSample)
	{
		istringstream ss(line.c_str(), stringstream::in);

		u32 lastsample = -1;
		u32 sampleCount = 0;
		while(!ss.eof() && !ss.fail() && ! ss.bad() )
		{
			int sample;
			ss >> sample;

			if(sample == -1)
			{
				builder.CbSetTag(kTagLastSample, lastsample);
				break;
			}

			if(sampleCount == 0)
			{
				if(sample > maxSample)
				{
					builder.NewDendriteWithSample(samplePoints[sample]);
				}
				else
				{
					builder.SetCurrentBranch( builder.CdFindBranchWithTag(kTagLastSample, sample) );
					builder.CbNewChildLast(true);
				}
			}
			else
			{
				builder.CbAddSample(samplePoints[sample]);
			}

			maxSample = max(sample, maxSample);

			++sampleCount;

			lastsample = sample;
		}

		return maxSample;
	}

	enum 
	{
		kTypes,
		kParameters,
		kData
	};

	bool ReadSampleCoords(Input& rIn, int nTotal, vector<EnhancedSampleData>& samples)
	{
		u32 n=0;
		while(n < nTotal)
		{
			if(!rIn.remaining())
			{
				LogImportFailure("File terminated too early, while reading coordinates");
				return false;
			}

			String line = rIn.readLine();

			if(line == "")
			{
				LogImportWarning("Found empty line in middle of coordinate list... unusual!!");
				continue;
			}

			istringstream ss(line.c_str(), stringstream::in);
			EnhancedSampleData sample;
			ss >> sample.pt.x >> sample.pt.y >> sample.pt.z;

			samples[n] = sample;

			++n;
		}
	}

#if 0
	bool ReadSampleDiams(Input& rIn, vector<EnhancedSampleData> samples)
	{
		int nTotal = samples.size();
		u32 n=0;
		while(n < nTotal)
		{
			if(!rIn.remaining())
			{
				LogImportFailure("File terminated too early, while reading coordinates");
				return false;
			}

			String line = rIn.readLine();

			if(line == "")
			{
				LogImportWarning("Found empty line in middle of radius list... unusual!!");
				continue;
			}

			istringstream ss((const char *)line, stringstream::in);
			float radius;
			ss >> radius;

			samples[n].d = radius*2.0f;

			++n;
		}
	}
#endif

	bool ReadSampleNeighbourCount(Input& rIn, vector<EnhancedSampleData> samples, u32& totalNeighbourCount)
	{
		int nTotal = samples.size();

		totalNeighbourCount = 0;
		u32 n=0;
		while(n < nTotal)
		{
			if(!rIn.remaining())
			{
				LogImportFailure("File terminated too early, while reading neighbour count");
				return false;
			}

			String line = rIn.readLine();

			if(line == "")
			{
				LogImportWarning("Found empty line in middle of neighbour count list... unusual!!");
				continue;
			}

			istringstream ss(line.c_str(), stringstream::in);
			u32 nbrCount;
			ss >> nbrCount;

			samples[n].nbrCount = nbrCount;

			totalNeighbourCount += nbrCount;

			++n;
		}
	}

#if 0
	bool ReadSampleNeighbourList(Input& rIn, vector<EnhancedSampleData> samples, u32 nTotalNeighbourCount)
	{
		int nTotal = samples.size();
		u32 n=0;
		while(n < nTotal)
		{
			if(!rIn.remaining())
			{
				LogImportFailure("File terminated too early, while reading neighbour count");
				return false;
			}

			String line = rIn.readLine();

			if(line == "")
			{
				LogImportWarning("Found empty line in middle of neighbour count list... unusual!!");
				continue;
			}

			istringstream ss((const char *)line, stringstream::in);

			for(u32 n=0;n<samples[sampleCount].nbrCount
			while( samples[sampleCount].nbrList.size() )
							while( == )
					++sampleCount;

				
				u32 nbrCount;
			ss >> nbrCount;

			samples[n].nbrCount = nbrCount;

			n += ;
		}

		if(nTotalNeighbourCount != )
			LogImportFailure(String("Neighbour count is not consistent with neighbour list!") );
	}
#endif

#if 0
	bool BuildNeuronFromNeighbourData(vector<EnhancedSampleData> samples)
	{
		// Now connect up data points...
		u32 pointsToConnect = samples.size();

		while(1)
		{
			u32 startingPoint = FindStartingPoint(samples);
			if( startingPoint >= pointsToConnect)
				break;

			samples[startingPoint].connected = true;
			builder.NewDendriteWithSample( samples[startingPoint].pt );

			const u32 currentNbr = samples[startingPoint].nbrList[0];
			samples[currentNbr].connected = true;
			BuildTree(builder, builder.CurrentBranch(), startingPoint, currentNbr, samples);
		};

		// check that all sample points are connected up
		for(u32 n=0;n<samples.size();++n)
		{
			if(!sampleData[n].connected)
				LogImportWarning( String("Sample point has not been added to tree: ") + dec(n+1) );
		}
	}
#endif

	bool ReadSampleConnectivityAndBuildNeuron()
	{
		u32 maxSample = -1;
		while(rIn.remaining() > 0)
		{
			String line = rIn.readLine();

			if(line == "")
				break;

			maxSample = ReadBranchConnectivity(line, maxSample);

			if(builder.HasFailed())
			{
				LogImportFailure(String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}
	}


	enum ParseStage
	{
		kStageDefines,
		kStageParameters,
		kStageTypes,
		kStageData
	};

	enum ContentType
	{
		kContentUnknown,
		kContentLineSet,
		kContentSkeletonGraph
	};

#if 0
	bool IsBackwardCompatible(const String& str)
	{
		return 
			str == nNodes ||
			str == nTriangles ||
			str == nTetrahedra ||
			str == nEdges;
	}
#endif

	bool IsRelevantType(const String& str)
	{
		return 
			str == "Vertices" ||
			str == "Lines" ||
			str == "EdgeData" || str == "Edges" ||
			str == "Origins" ||
			str == "vertexTypeList";
	}

	bool IsKnownType(const String& str)
	{
		return 
			IsRelevantType(str) ||

			str == "Nodes" || str == "NodeData" ||
			str == "Triangles" || str == "TriangleData" ||
			str == "Tetrahedra" || str == "TetrahedraData" ||
			str == "Lattice" ||
			str == "Coordinates" ||
			str == "Markers" ||
			
			// spatial graph
			str == "VERTEX" ||
			str == "EDGE" ||
			str == "POINT";
	}

#if 0
	// interpolators
	Field
	Contant
	EdgeElem

	bool CheckTypes(std::vector<TypeInfo>& info)
	{
		bool patternMatched;
		bool tooMany;

		u32 bestMatch;

		for(patterns)
		{
			if(info.size() < pattern.count)
			{
				continue;
			}
			else if(info.size() > pattern.count)
			{
				LogImportWarning();
			}

			for()
				find(info.begin(), info.end(), String("") );

			return false;
		}

		if()
	}

	struct TypeInfo
	{
		String	name;
		u32		count;
	};

	std::vector<TypeInfo> mTypesFound;

	bool ParseNew()
	{
		vector<EnhancedSampleData> sampleData;

		ParseStage stage	= kStageDefines;
		ContentType content = kContentUnknown;
		mTypesFound.clear();
		bool bHaveCheckedTypes = false;

		while(1)
		{
			SeekNextItem(); 
			String item = GetNextItem();

			// Stage 1 - extract type names
			const bool isBackwardCompatible = IsBackwardCompatible(item);
			if( item.begins("define") || isBackwardCompatible)
			{
				if(stage != kStageDefines)
				{
					LogImportFailure("Unexpected data order..");
					return false;
				}

				String typeName;
				if(isBackwardCompatible)
				{
					typeName = item.after('n');
				}
				else
				{
					SeekNextItem();
					typeName = GetNextItem();
				}

				if( IsRelevantType(str) )
				{
					SeekNextItem();
					GetNextItem();
					
					int count = ...;
					TypeInfo info = { typeName, count };
					mTypesFound.push_back(info);
				}
				else
				{
					if(IsKnownType(str))
						LogImportWarning("Ignoring known keyword: " + str + ". Might still be able to parse file.");
					else
						LogImportWarning("Ignoring unknown keyword: " + str + ". Might still be able to parse file.");
					// skip...
				}
			}
			else
			{
				if(!bHaveCheckedTypes)
				{
					if(!CheckTypes(mTypesFound))
					{
						LogImportFailure("No pattern of types matched..");
						return false;
					}

					bHaveCheckedTypes = true;
				}

				// Stage 2 - extract content type
				if(item.begins("Parameters"))
				{
					
					if(content != kContentUnknown)
					{
						LogImportFailure("More than one Content type.");
						return false;
					}

					if(stage != kStageDefines)
					{
						LogImportFailure("Unexpected data order.");
						return false;
					}

					stage = kStageParameters;
					content = ParseParameters();
					stage = kStageTypes;
				}
				else if( IsExpectedType(item) )
				{
					if(stage != kStageTypes)
					{
						LogImportFailure("Unexpected data order.");
						return false;
					}
				}
				else if(item.begins("@"))
				{
					// add data types..
				}
		}


		u32 totalNeighbourCount = 0;

		while(1)
		{
			// Seek @

			// work out what type of data it should be.. read as appropriate..

			String dataId = "@" + dec(data[n].id);
			SeekLineBeginning(rIn, (const char*) dataId);
			SeekNextLine();

			switch(type)
			{
			case kCoordinates:
				if( !ReadSampleCoords(rIn, numSamples, sampleData) )
					return false;
				break;
			case kNeighbourCount:
				if( !ReadSampleNeighbourCount(rIn, numSamples, sampleData, totalNeighbourCount) )
					return false;
				break;
			case kRadii:
				if( !ReadSampleDiameters(rIn, numSamples, sampleData, totalNeighbourCount) )
					return false;
				break;
				case kNeighbourList
					if( !ReadSampleNeighbourList(rIn, totalNeighbourCount, sampleData) )
						return false;

			case kConnectivity:
				ReadSampleConnectivityAndBuildTree();
				break;
			}
		}

		switch(contentType)
		{
		case kHxLineSet:
			// nothing for now.. already done as part of kConnectivity
			break;
		case kSkeletonGraph:
			if( !BuildNeuronFromNeighbourData(sampleData, ) )
				return false;
			break;
		}
	

		builder.ApplyChanges();

		m_nrn.MergeAllUnaryBranchPoints();
		m_nrn.EnhanceSingleSampleTrees();

		return true;
	}
#endif

	bool Parse()
	{
		String firstLine = rIn.readLine();

		bool version2 = false;
		if( StringContains(firstLine, "AmiraMesh 3D ASCII 2.0"))
			version2 = true;
				

		samplePoints.clear();

		if(version2)
		{
			vector<EnhancedSampleData> sampleData;

			// coordinates
			SeekLineBeginning(rIn, "@1");
			SeekNextItem();
			SeekNextItem();

			while(rIn.remaining() > 0)
			{
				String line = rIn.readLine();

				if(line == "")
					break;

				istringstream ss(line.c_str(), stringstream::in);
				EnhancedSampleData sample;
				ss >> sample.pt.x >> sample.pt.y >> sample.pt.z;

				sampleData.push_back(sample);
			}

			const u32 numSamples = sampleData.size();

			// neighbour count
			u32 totalNeighbourCount = 0;
			SeekLineBeginning(rIn, "@2");
			SeekNextItem();
			SeekNextItem();

			u32 count = 0;
			while(rIn.remaining() > 0)
			{
				assert(count <= numSamples);

				String line = rIn.readLine();

				if(line == "")
					break;

				istringstream ss(line.c_str(), stringstream::in);
				ss >> sampleData[count].nbrCount;

				totalNeighbourCount += sampleData[count].nbrCount;

				++count;
			}

			if(count != numSamples)
			{
				LogImportFailure(String("Neighbour count and sample points size mismatch") );
				return false;
			}

			// diameter
			SeekLineBeginning(rIn, "@3");
			SeekNextItem();
			SeekNextItem();

			count = 0;
			while(rIn.remaining() > 0)
			{
				assert(count <= numSamples);

				String line = rIn.readLine();

				if(line == "")
					break;

				istringstream ss(line.c_str(), stringstream::in);
				ss >> sampleData[count].pt.d;

				// AmiraMesh uses radius
				sampleData[count].pt.d *= 2.0f;

				++count;
			}


			// neighbour list
			SeekLineBeginning(rIn, "@4");
			SeekNextItem();
			SeekNextItem();

			u32 sampleCount = 0;
			u32 neighbourListCount = 0;
			while(rIn.remaining() > 0)
			{
				String line = rIn.readLine();

				if(line == "")
					break;

				istringstream ss(line.c_str(), stringstream::in);
				u32 neighbour;
				ss >> neighbour;

				while(sampleData[sampleCount].nbrList.size() == sampleData[sampleCount].nbrCount)
					++sampleCount;

				sampleData[sampleCount].nbrList.push_back(neighbour);

				++neighbourListCount;
			}

			if(totalNeighbourCount != neighbourListCount)
			{
				LogImportFailure(String("Neighbour count is not consistent with neighbour list!") );
				return false;
			}

			// Now connect up data points...
			u32 pointsToConnect = sampleData.size();

			while(1)
			{
				u32 startingPoint = FindStartingPoint(sampleData);
				if( startingPoint >= pointsToConnect)
					break;

				sampleData[startingPoint].connected = true;
				builder.NewDendriteWithSample( sampleData[startingPoint].pt );

				const u32 currentNbr = sampleData[startingPoint].nbrList[0];
				sampleData[currentNbr].connected = true;
				BuildTree(builder, builder.CurrentBranch(), startingPoint, currentNbr, sampleData);
			};

			// check that all sample points are connected up
			for(u32 n=0;n<sampleData.size();++n)
			{
				if(!sampleData[n].connected)
					LogImportWarning( String("Sample point has not been added to tree: ") + dec(n+1) );
			}


			// re-organize all trees, for correct soma point ?

		}
		else
		{

/*		SeekLineBeginning(rIn, "Vertices");
		"Vertices { float[3] Coordinates } = @1"
		"Lines { int LineIdx } = @2"*/

		// coordinates
		SeekLineBeginning(rIn, "@1");
		SeekNextItem();
		SeekNextItem();

		while(rIn.remaining() > 0)
		{
			String line = rIn.readLine();

			if(line == "")
				break;

			istringstream ss(line.c_str(), stringstream::in);
			SamplePoint sample;
			ss >> sample.x >> sample.y >> sample.z;

			samplePoints.push_back(sample);
		}

		const u32 numSamples = samplePoints.size();

		// diameters
		SeekLineBeginning(rIn, "@2");
		SeekNextItem();
		SeekNextItem();

		u32 sampleCount = 0;
		while(rIn.remaining() > 0)
		{
			String line = rIn.readLine();

			if(line == "")
				break;

			istringstream ss(line.c_str(), stringstream::in);
			assert(sampleCount < numSamples);
			ss >> samplePoints[sampleCount].d;

			// AmiraMesh uses radius
			samplePoints[sampleCount].d *= 2.0f;

			++sampleCount;
		}

		// connectivity
		SeekLineBeginning(rIn, "@3");
		SeekNextItem();
		SeekNextItem();

		u32 maxSample = -1;
		while(rIn.remaining() > 0)
		{
			String line = rIn.readLine();

			if(line == "")
				break;

			maxSample = ReadBranchConnectivity(line, maxSample);

			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}

		}

		builder.ApplyChanges();

		m_nrn.MergeAllUnaryBranchPoints();
		m_nrn.EnhanceSingleSampleTrees();

		return true;
	}
};

bool ImportAmiraMesh(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	AmiraMeshParser parser(rIn, nrn);
	return parser.Parse();
}

bool ImportRawAmiraMesh(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportAmiraMesh(*pIn, nrn, options);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

static inline void WriteCoordinate(Output& rOut, const SamplePoint& sample)
{
	rOut.writeLine(dec(sample.x) + " " + dec(sample.y) + " " + dec(sample.z));
}

static inline void WriteRadius(Output& rOut, const SamplePoint& sample)
{
	rOut.writeLine(dec(0.5f*sample.d));
}

static inline void WriteInteger(Output& rOut, u32 value)
{
	rOut.writeLine(dec(value));
}

static inline void WriteNeighbourCount(Output& rOut, u32 count)
{
	WriteInteger(rOut, count);
}


static void OutputBranchPositions(Output& rOut, Neuron3D::DendriteTreeConstIterator it)
{
	for(u32 n=1;n<(*it).m_samples.size(); ++n)
		WriteCoordinate(rOut, (*it).m_samples[n]);

	if(it.child())
		OutputBranchPositions(rOut, it.child());

	if(it.peer())
		OutputBranchPositions(rOut, it.peer());
}

static void OutputBranchDiameters(Output& rOut, Neuron3D::DendriteTreeConstIterator it)
{
	for(u32 n=1;n<(*it).m_samples.size(); ++n)
		WriteRadius(rOut, (*it).m_samples[n]);

	if(it.child())
		OutputBranchDiameters(rOut, it.child());

	if(it.peer())
		OutputBranchDiameters(rOut, it.peer());
}

static void OutputBranchNeighbourCount(Output& rOut, Neuron3D::DendriteTreeConstIterator it)
{
	for(u32 n=1;n<(*it).m_samples.size()-1; ++n)
		WriteNeighbourCount(rOut, 2);

	const u32 numChildren = Neuron3D::DendriteTree::countchildren(it);
	const u32 neighbourCount = numChildren+1; // + 1 for parent

	WriteNeighbourCount(rOut, neighbourCount);

	if(it.child())
		OutputBranchNeighbourCount(rOut, it.child());

	if(it.peer())
		OutputBranchNeighbourCount(rOut, it.peer());
}


u32 CountSubTreeSamples(Neuron3D::DendriteTreeConstIterator it)
{
	Neuron3D::SampleCounter counter;
	counter(it);

	if(it.child())
		Neuron3D::DendriteTree::recurse_it( it.child(), counter);

	return counter.GetCount();
}



static int OutputBranchNeighbourList(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int parent, int current)
{
	WriteInteger(rOut, parent);

	for(u32 n=1;n<(*it).m_samples.size()-1; ++n)
	{
		WriteInteger(rOut, current+1);
		++current;
		WriteInteger(rOut, current-1);
	}

	// TODO: prepass phase, would be much faster..

	// for each child.
	Neuron3D::DendriteTreeConstIterator itChild = it.child();
	u32 childSampleOffset = current;
	while(itChild)
	{
		WriteInteger(rOut, childSampleOffset+1);
		childSampleOffset += CountSubTreeSamples(itChild);

		itChild = itChild.peer();
	};
	//

	++current;

	if(it.child())
		current = OutputBranchNeighbourList(rOut, it.child(), current, current);
	if(it.peer())
		current = OutputBranchNeighbourList(rOut, it.peer(), parent, current);

	return current;
}

static int OutputBranchConnectivity(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int parent, int current)
{
	String output = dec(parent);

	for(u32 n=1;n<(*it).m_samples.size(); ++n)
	{
		++current;
		output += " " + dec(current);
	}

	output += " -1";

	rOut.writeLine(output);

	if(it.child())
		current = OutputBranchConnectivity(rOut, it.child(), current, current);

	if(it.peer())
		current = OutputBranchConnectivity(rOut, it.peer(), parent, current);

	return current;
}

bool ExportAmiraMesh_SkeletonGraph(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	const u32 numSamples = nrn.CountAllAxonSamplePoints() + nrn.CountAllDendriteSamplePoints();
	const u32 numTrees = nrn.CountDendrites() + nrn.CountAxons();
	const u32 numEdges = nrn.CountAllAxonNeighbourPoints() + nrn.CountAllDendriteNeighbourPoints();

	// header
	rOut.writeLine("# AmiraMesh 3D ASCII 2.0");

	if(options.identifyNL)
		rOut.writeLine("# Created by " + options.appName + " (version " + options.appVersion + ").");

	if(options.addCommentText)
		rOut.writeLine("# " + options.commentText);

	rOut.writeLine("");
	rOut.writeLine("nVertices " + dec(numSamples));
	rOut.writeLine("nEdges " + dec(numEdges));
	rOut.writeLine("define Origins " + dec(0)); // not sure ! - set 0
	rOut.writeLine("define vertexTypeList " + dec(0));
	rOut.writeLine("");
	rOut.writeLine("Parameters {");
	rOut.writeLine("    Materials {");
	rOut.writeLine("    }");
	rOut.writeLine("    Fitting {");
	rOut.writeLine("    CVFact 0.001,"); // TODO: configurable values ?
    rOut.writeLine("    CRFact 0.001,");
    rOut.writeLine("    OVFact 0.001,");
    rOut.writeLine("    ORFact 0.001,");
    rOut.writeLine("    IVFact 0.5,");
    rOut.writeLine("    IRFact 0.5,");
    rOut.writeLine("    StepSize 0.3");
	rOut.writeLine("    }");
	rOut.writeLine("    ContentType \"SkeletonGraph\"");
	rOut.writeLine("}");
	rOut.writeLine("");
	rOut.writeLine("Vertices { float[3] Coordinates } @1");
	rOut.writeLine("Vertices { int NeighbourCount } @2");
	rOut.writeLine("Vertices { float Radii } @3");
	rOut.writeLine("EdgeData { int NeighbourList } @4");
	rOut.writeLine("Origins { int Origins } @5");
	rOut.writeLine("Vertices { int vertexTypeCounter } @6");
	rOut.writeLine("vertexTypeList { int vertexTypeList } @7");
	rOut.writeLine("");

	// co-ords
	rOut.writeLine("# Data section follows");
	rOut.writeLine("@1");
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		Neuron3D::DendriteTreeConstIterator it = (*d).root();
		WriteCoordinate(rOut, (*it).m_samples[0]);
		OutputBranchPositions( rOut, it);
	}

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		Neuron3D::AxonTreeConstIterator it = (*a).root();
		WriteCoordinate(rOut, (*it).m_samples[0]);
		OutputBranchPositions( rOut, it);
	}

	// neighbour count
	rOut.writeLine("");
	rOut.writeLine("@2");

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		Neuron3D::DendriteTreeConstIterator it = (*d).root();
		WriteNeighbourCount(rOut, 1);
		OutputBranchNeighbourCount( rOut, it );
	}

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		Neuron3D::AxonTreeConstIterator it = (*a).root();
		WriteNeighbourCount(rOut, 1);
		OutputBranchNeighbourCount( rOut, it );
	}
	
	// radii
	rOut.writeLine("");
	rOut.writeLine("@3");

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		Neuron3D::DendriteTreeConstIterator it = (*d).root();
		WriteRadius(rOut, (*it).m_samples[0]);
		OutputBranchDiameters( rOut, it );
	}

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		Neuron3D::AxonTreeConstIterator it = (*a).root();
		WriteRadius(rOut, (*(*a).root()).m_samples[0]);
		OutputBranchDiameters( rOut, it );
	}
	

	// neighbour list
	rOut.writeLine("");
	rOut.writeLine("@4");

	int current = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		WriteInteger(rOut, current+1);

		Neuron3D::DendriteTreeConstIterator it = (*d).root();
		current = OutputBranchNeighbourList( rOut, it, current, current+1);
	}

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		WriteInteger(rOut, current+1);

		Neuron3D::AxonTreeConstIterator it = (*a).root();
		current = OutputBranchNeighbourList( rOut, it, current, current+1);
	}

	if(current != numSamples)
	{
		LogImportFailure("Incorrectly calculated number of Edges!!! something bad happened!!");
		assert(0);
	}


	// origins ??? all 0 for now...
//	rOut.writeLine("");
//	rOut.writeLine("@5");

	// vertex type count...all 0
	rOut.writeLine("");
	rOut.writeLine("@6");
	for(u32 n=0;n<numSamples;++n)
		WriteInteger(rOut, 0);

	// @7 not output... (0 entries)


	return true;
}

bool ExportAmiraMesh_SpatialGraph(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	return false;
}

bool ExportAmiraMesh_HxLineSet(Output& rOut, const Neuron3D &nrn, const StyleOptions& options, bool emitWidths, bool noBlankLines)
{
	const u32 numSamples = nrn.CountAllAxonSamplePoints() + nrn.CountAllDendriteSamplePoints();
	const u32 numTrees = nrn.CountDendrites() + nrn.CountAxons();
	const u32 numTreeElements = nrn.CountDendriteElements() + nrn.CountAxonElements();
	const u32 numLineSegments = numSamples - numTrees + (2*numTreeElements);

	// header
	rOut.writeLine("# AmiraMesh ASCII 1.0");

	if(options.identifyNL)
		rOut.writeLine("# Created by " + options.appName + " (version " + options.appVersion + ").");

	if(options.addCommentText)
		rOut.writeLine("# " + options.commentText);

	if(!noBlankLines) rOut.writeLine("");
	rOut.writeLine("define Lines " + dec(numLineSegments));
	rOut.writeLine("define Vertices " + dec(numSamples));
	if(!noBlankLines) rOut.writeLine("");
	rOut.writeLine("Parameters {");
	rOut.writeLine("\tContentType \"HxLineSet\"");
	rOut.writeLine("}");
	if(!noBlankLines) rOut.writeLine("");
	rOut.writeLine("Vertices { float[3] Coordinates } = @1");
	if(emitWidths)
	{
		rOut.writeLine("Vertices { float Data } = @2");
		rOut.writeLine("Lines { int LineIdx } = @3");
	}
	else
	{
		rOut.writeLine("Lines { int LineIdx } = @2");
	}
	if(!noBlankLines) rOut.writeLine("");

	// todo - optimize this. No need to do so many passes of the tree...

	// co-ords
	rOut.writeLine("@1 #  " + dec(numSamples) + " xyz coordinates");
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		Neuron3D::DendriteTreeConstIterator it = (*d).root();
		WriteCoordinate(rOut, (*it).m_samples[0]);
		OutputBranchPositions( rOut, it);
	}

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		Neuron3D::AxonTreeConstIterator it = (*a).root();
		WriteCoordinate(rOut, (*it).m_samples[0]);
		OutputBranchPositions( rOut, it);
	}

	if(emitWidths)
	{
		// diameters
		if(!noBlankLines) rOut.writeLine("");
		rOut.writeLine("@2 #  " + dec(numSamples) + " width values");
		for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		{
			Neuron3D::DendriteTreeConstIterator it = (*d).root();
			WriteRadius(rOut, (*it).m_samples[0]);
			OutputBranchDiameters( rOut, it );
		}

		for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
		{
			Neuron3D::AxonTreeConstIterator it = (*a).root();
			WriteRadius(rOut, (*(*a).root()).m_samples[0]);
			OutputBranchDiameters( rOut, it );
		}
	}

	// connectivity
	int parent = 0;
	int current = 0;
	if(!noBlankLines) rOut.writeLine("");
	if(emitWidths)
		rOut.writeLine("@3 #  " + dec(numLineSegments) + " line segements"); // intentional mis-spelling
	else
		rOut.writeLine("@2 #  " + dec(numLineSegments) + " line segements"); // intentional mis-spelling

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		Neuron3D::DendriteTreeConstIterator it = (*d).root();
		current = OutputBranchConnectivity( rOut, it, current, current);
		++current;
	}

	/*for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		Neuron3D::AxonTreeConstIterator it = (*a).root();
		current = OutputBranchConnectivity( rOut, it, current, current);
		++current;
	}*/

	return true;
}

bool ExportAmiraMesh(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	switch(options.formatStyle)
	{
	default:
	case kStyleAmiraMesh_LNeuron:
		return ExportAmiraMesh_HxLineSet(rOut, nrn, options, true, false);
//	case kStyleAmiraMesh_FlyCircuit: // ?
//		return ExportAmiraMesh_HxLineSet(rOut, nrn, options, false, true);
	case kStyleAmiraMesh_SkeletonGraph:
		return ExportAmiraMesh_SkeletonGraph(rOut, nrn, options);
//	case kStyleAmiraMesh_HxSpatialGraph:
//		return ExportAmiraMesh_SpatialGraph(rOut, nrn, options);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateAmiraMesh(Input& rIn, const ValidationOptions& options)
{
	String firstLine = rIn.readLine();
	String secondLine = rIn.readLine();

	if(	StringContains(firstLine, "AmiraMesh ASCII 1.0") || 
		StringContains(firstLine, "AmiraMesh 3D ASCII 2.0"))
	{
		return kValidationTrue;
	}

	return kValidationFalse;
}
