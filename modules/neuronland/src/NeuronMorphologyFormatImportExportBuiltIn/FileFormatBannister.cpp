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

	Bannister format


	(From Neil Bannister email, via Jose Ambrose-Ingerson)

----------
Dendritic tree subgrouped into separate dendritic trees (each has a separate
root from the soma).
Apical dendrite is always the last tree.

Arrangement of columns:
1 Dendrite segment ID
2 Length of segment (microns)
3 Diameter of segment (microns) X10  eg 12 = 1.2microns
4 spine density of segment (spines per micron) X10 eg. 8 = 0.8spines per
micron
5 to 9 ignore
10 left daughter
11 right daughter

In some circumstances (particularly with long dendrite segments) the
dendrite tapers in diameter . In such circumstances I have subdivided it
into a number of segments with decreasing diameters.  Where this occurs only
a left daughter is present.
----------


**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

struct SegmentData
{
	int segmentId;
	float length;
	float diameter;
	float spineDensity;
	int leftId;
	int rightId;

	bool used;

	SegmentData() : used(false)
	{
	}
};

bool GetUnusedSegmentAndMarkUsed(std::vector<SegmentData>& segments, u32 segmentId, SegmentData& segment)
{
	for(u32 n=0;n<segments.size();++n)
	{
		if(!segments[n].used && segments[n].segmentId == segmentId)
		{
			segments[n].used = true;
			segment = segments[n];
			return true;
		}
	}
	return false;
}

bool ConnectUpBannisterSubTree(MorphologyBuilder& builder, std::vector<SegmentData>& segments, u32 segmentId, bool root)
{
	assert(segments.size() > 0);
	u32 segmentCount = 0;

	// for now..assume root is first in list..
	if(root)
	{
		segmentId = segments[0].segmentId;
	}
	//

	SegmentData segment;
	bool success = GetUnusedSegmentAndMarkUsed(segments, segmentId, segment);
	if(!success)
	{
		LogImportFailure("Cannot find segment: " + dec(segmentId) );
		return false;
	}

	if(root)
	{
		SamplePoint rt(0.0f, 0.0f, 0.0f, segment.diameter);
		builder.CbAddSample(rt);
	}

	// for now.. no branch angles.. just distance from previous point. 
	// Artificial branching algorithm will be applied later
	SamplePoint pt(segment.length, 0.0f, 0.0f, segment.diameter);
	builder.CbAddSample(pt);
	//


	MorphologyBuilder::Branch thisBranch = builder.CurrentBranch();

	if(segment.leftId)
	{
		builder.CbNewChildFirst(true);
		if(! ConnectUpBannisterSubTree(builder, segments, segment.leftId, false) )
			return false;
		builder.CbToParent();
	}
	if(segment.rightId)
	{
		builder.CbNewChildLast(true);
		if(! ConnectUpBannisterSubTree(builder, segments, segment.rightId, false) )
			return false;
		builder.CbToParent();
	}

	return true;
}

bool ImportBannister(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	bool success = true;
	MorphologyBuilder builder(nrn);

	u32 expectedId = 1;
	u32 currentId;
	u32 segmentCount = 0;

	SamplePoint somaPoint(0.0f, 0.0f, 0.0f, 0.0f);
	builder.NewSomaSinglePoint(somaPoint);

	typedef std::vector< std::vector<SegmentData> > SegmentsVector;
	SegmentsVector dendriteSegmentsVector;
	u32 dendriteCount = 0;

	while(success && rIn.remaining() )
	{
		String line = rIn.readLine();

		if( StringBegins(line, "-"))
		{
			currentId = std::stoi(StringAfter(line, '-') );

			// can cope with unordered dendrites if necessary
			if(currentId != expectedId)
				LogImportWarning( String("Unexpected dendrite id: ") + dec(currentId) + " (expecting " + dec(expectedId) + ")" );
			++expectedId;
			//

			++dendriteCount;

			// init the Segment list for the dendrite
			dendriteSegmentsVector.push_back( std::vector<SegmentData>() );
		}
		else if(line.empty())
		{
			continue;
		}
		else
		{
			if(!StringBegins(line, dec(currentId) ))
				LogImportWarning("Unexpected line format: does start with current dendrite ID (" + dec(currentId) + "): " + line);

			SegmentData segment;
			int unknown1;
			int unknown2;
			int unknown3;
			int unknown4;
			int unknown5;

			istringstream ss(line.c_str(), stringstream::in);
			ss	>> segment.segmentId 
				>> segment.length 
				>> segment.diameter 
				>> segment.spineDensity 
				>> unknown1 >> unknown2 >> unknown3 >> unknown4 >> unknown5 
				>> segment.leftId 
				>> segment.rightId;

			// convert to um
			segment.length *= 0.1f;
			segment.diameter *= 0.1f;

			dendriteSegmentsVector[dendriteCount-1].push_back(segment);
		}
	}

	// now connect up
	for(u32 n=0;n<dendriteCount;++n)
	{
		builder.NewDendrite();

		if( !ConnectUpBannisterSubTree(builder, dendriteSegmentsVector[n], 0, true) )
		{
			LogImportFailure("Could not successfully connect up trees...");
			return false;
		}

		// check all segments
		for(u32 m=0;m<dendriteSegmentsVector[n].size();++m)
		{
			assert( dendriteSegmentsVector[n][m].used );
			if(!dendriteSegmentsVector[n][m].used)
			{
				LogImportWarning("Not all segments could be connected up!");
				return false;
			}
		}

		// set last as apical..
		if(n == dendriteCount-1)
			builder.CdSetApical();
	}


	builder.ApplyChanges();

	// make 1D builder mode ?
	nrn.ApplyArtificialBranching(options.artificialBranchingConfig);
	//

	nrn.MergeAllUnaryBranchPoints();
	nrn.EnhanceSingleSampleTrees();

	return success;
}


// Older version: doesn't cope too well with out of order data..
#if 0
bool ImportBannister(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	bool success = true;
	MorphologyBuilder builder(nrn);

	u32 expectedId = 1;
	u32 currentId;
	u32 segmentCount = 0;

	SamplePoint somaPoint(0.0f, 0.0f, 0.0f, 0.0f);
	builder.NewSomaSinglePoint(somaPoint);

	while(success && rIn.remaining())
	{
		String line = rIn.readLine();

		if(line.begins("-"))
		{
			currentId = line.after("-").toInt();

			// can cope with unordered dendrites if necessary
			if(currentId != expectedId)
				LogImportWarning( String("Unexpected dendrite id: ") + dec(currentId) + " (expecting " + dec(expectedId) + ")" );
			++expectedId;
			//

			segmentCount = 0;

			builder.NewDendrite();
		}
		else if(line.begins( dec(currentId) ))
		{
			int segmentId;
			float length;
			float diameter;
			float spineDensity;
			int unknown1;
			int unknown2;
			int unknown3;
			int unknown4;
			int unknown5;
			int leftId;
			int rightId;

			istringstream ss((const char *)line, stringstream::in);
			ss >> segmentId >> length >> diameter >> spineDensity >> unknown1 >> unknown2 >> unknown3 >> unknown4 >> unknown5 >> leftId >> rightId;

			// convert to um
			length *= 0.1f;
			diameter *= 0.1f;

			if(segmentCount > 0)
			{
				MorphologyBuilder::Branch parentBranL = builder.CdFindBranchWithTag("left", segmentId);
				MorphologyBuilder::Branch parentBranR = builder.CdFindBranchWithTag("right", segmentId);

				// some validation..
				if(	parentBranL == MorphologyBuilder::NullBranch() && 
					parentBranR == MorphologyBuilder::NullBranch() )
				{
					LogImportFailure(String("Cannot find parent: out of order data...!") );
					success = false;
					break;
				}
				if(	parentBranL != MorphologyBuilder::NullBranch() && 
					parentBranR != MorphologyBuilder::NullBranch() )
				{
					LogImportFailure(String("More than 1 parent segment!") );
					success = false;
					break;
				}
				//

				if(parentBranL == MorphologyBuilder::NullBranch())
				{
					builder.SetCurrentBranch(parentBranR);
					builder.CbNewChildLast(true);
				}
				else
				{
					builder.SetCurrentBranch(parentBranL);
					builder.CbNewChildFirst(true);
				}
			}
			else
			{
				String expectedFirstId = dec(currentId) + String("01");
				if( !line.begins(expectedFirstId) )
				{
					LogImportFailure( String("Unexpected first ID: ") + expectedFirstId);
					return false;
				}
				SamplePoint rt(0.0f, 0.0f, 0.0f, diameter);
				builder.CbAddSample(rt);
			}

			// for now.. no branch angles.. just distance from previous point. 
			// Artificial branching algorithm will be applied later
			SamplePoint pt(length, 0.0f, 0.0f, diameter);
			builder.CbAddSample(pt);

			builder.CbSetTag("left", leftId);
			builder.CbSetTag("right", rightId);

			++segmentCount;
		}
		else if(line.empty())
		{
			continue;
		}
		else
		{
			LogImportFailure("Unexpected line format: does start with '-' or current dendrite ID (" + dec(currentId) + "): " + line);
			success = false;
		}
	}

	// set last as apical.. ?
	if(builder.NumDendrites() > 0)
	{
		//builder.CdSetApical();
	}


	builder.ApplyChanges();

	// make 1D builder mode ?
	nrn.ApplyArtificialBranching(options.artificialBranchingConfig);

	nrn.MergeAllUnaryBranchPoints();
	nrn.EnhanceSingleSampleTrees();

	return success;
}
#endif

bool ImportRawBannister(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportBannister(*pIn, nrn, options);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

bool ExportBannister(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// unsupported
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateBannister(Input& rIn, const ValidationOptions& options)
{
	// hacky, temp..
	String line1 = rIn.readLine();
	String line2 = rIn.readLine();
	String line3 = rIn.readLine();

	if(StringBegins(line1, "-"))
	{
	//	String idStr = line1.after("-");
		//if(line2.begins(idStr) && line3.begins(idStr))
			return kValidationTrue;
	}

	return kValidationFalse;
}
