//
//
//

/**

	GulyasTree Format

	Structure supported ->
		Soma		: NO
		Dendrite	: BINARY
		Axon		: BINARY
		Marker		: NO
		Spine		: NO

	Pending:
	* 

**/


/**
Tree format (.ntr) description
The file completely describes the cells geometry.

* the file starts with header lines describing the types of segment points present in the file
* this section is closed with a line of ********************
* the next line contains the number of points
* then each line contains the description of one point
* Each record consist of the following fields:
o X, Y, Z coordinates (in micrometer)
o diameter (in micrometer)
o type of point
+ 1= tree origin
+ 7=branchpoint
+ 4=continuation point
+ 8=end point
o  type of segment: identifies the segment types described in the header
* the points are described sequentially after an endpoint the next point is connected to the next unfinished branchpoint
* e.g.
o                   6 7 8 9
1 2 3 4 5 <
10 11 12 13 14
o The description is 1,2,3,4,5(bp),6,7,8,9(ep),10,11,12,13,14(ep)
**/

#include "ImportExportCommonIncludes.h"

#include <iostream>
#include <sstream>
#include <functional>

#include <math.h>

#include "Core/Parser.h"

/** 

	GulyasTree format

**/

enum GulyasPointType
{ 
	kPointOrigin			= 1,
	kPointContinuation		= 4,
	kPointBranch			= 7,
	kPointEnd				= 8
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class GulyasTreeParser : public SimpleParser
{
	MorphologyBuilder builder;

public:
	GulyasTreeParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn)
	{
	}

	bool Parse()
	{
		while(rIn.remaining() > 0)
		{
			String line = rIn.readLine();

			if(StringBegins(line, "******"))
				break;
		}

		u32 numSamples;
		String line = rIn.readLine();
		istringstream ss(line, stringstream::in);

		ss >> numSamples;

		u32 count = 0;

		bool doneRootPoints = false;

		// hack to get actual number of samples, so we can work out number of dendrites...
		int remaining = rIn.remaining();

		int sampleCount = 0;
		while(rIn.remaining() > 0)
		{
			line = rIn.readLine();
			if(line == "")
				break;

			++sampleCount;
		}

		while(rIn.remaining() < remaining)
			rIn.rollback();

		int numDendrites = sampleCount - numSamples;
		//

		while(rIn.remaining() > 0)
		{
			SamplePoint sample;
			GulyasPointType pointType;
			String line = rIn.readLine();

			if(line == "")
				break;

			istringstream ss(line, stringstream::in);
			ss >> sample.x;
			ss.get();
			ss >> sample.y;
			ss.get();
			ss >> sample.z;
			ss.get();
			ss >> sample.d;
			ss.get();
			ss >> (int&)pointType;

			sample.x = std::min(sample.x, 99999.0f);
			sample.y = std::min(sample.y, 99999.0f);
			sample.z = std::min(sample.z, 99999.0f);
			sample.d = std::min(sample.d, 99999.0f);

			switch(pointType)
			{
			case kPointOrigin:
				{
					builder.NewSomaSinglePoint(sample);
					builder.NewDendriteWithSample(sample);
					--numDendrites;
				}
				break;
			case kPointContinuation:
				builder.CbAddSample(sample);
				break;
			case kPointBranch:
				//if(sample == builder.CurrentSoma().GetSample(0))
				if(numDendrites > 0)
				{
					builder.NewDendriteWithSample(sample);
					builder.SetCurrentDendrite( builder.DendriteBegin() );
					--numDendrites;
				}
				else
				{
					builder.CbAddSample(sample);
					builder.CbNewChildLast(true);
				}
				break;
			case kPointEnd:
				builder.CbAddSample(sample);
				if(builder.CbAscendToUnaryBranchPoint())
					builder.CbNewChildLast(true);
				else
				{
					builder.NextDendrite();
				}
				break;
			}

			++count;

			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}

		if(count != numSamples + builder.NumDendrites())
		{
			LogImportWarning("Sample count does not match file header sample count!");
		}

		builder.ApplyChanges();
		return true;
	}
};

bool ImportGulyasTree(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	GulyasTreeParser parser(rIn, nrn);
	return parser.Parse();
}


bool ImportRawGulyasTree(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportGulyasTree(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteGulyasDataPoint(Output& rOut, const SamplePoint& sample, GulyasPointType pointType, u32 segmentType)
{
	rOut.writeLine(	PrependSpaces( dec(sample.x, 1), 7)	+ "," +
					PrependSpaces( dec(sample.y, 1), 8) + "," +
					PrependSpaces( dec(sample.z, 1), 8) + "," +
					PrependSpaces( dec(sample.d, 2), 6) + "," +
					PrependSpaces( dec((u32)pointType), 3) + "," + 
					PrependSpaces( dec(segmentType), 4) );
}

void OutputBranchGulyas(Output& rOut, Neuron3D::DendriteTreeConstIterator it)
{
	const u32 numSamples = (*it).m_samples.size();
	for(u32 i=1;i<numSamples;++i)
	{
		GulyasPointType pointType = kPointContinuation;
		if(i == numSamples-1)
		{
			const u32 numchildren = Neuron3D::DendriteTree::countchildren(it);
			if(numchildren > 1)
				pointType = kPointBranch;
			else if(numchildren == 0)
				pointType = kPointEnd;
		}

		WriteGulyasDataPoint(rOut, (*it).m_samples[i], pointType, 1);
	}

	if(it.child()) 
		OutputBranchGulyas(rOut, it.child());

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it)) 
		OutputBranchGulyas(rOut, it.peer());
}

bool ExportGulyasTree(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	rOut.writeLine("Segment types");
	rOut.writeLine("   1\t      1.00\t   Unknown     ");
	rOut.writeLine("****************************");

	const u32 numSamples = nrn.CountAllDendriteSamplePoints() /*+ nrn.CountAllAxonSamplePoints()*/ - (nrn.CountDendrites() /*+ nrn.CountAxons()*/);

	rOut.writeLine(dec(numSamples));

	// do origin and dendrite root points
	bool doneOrigin = false;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		WriteGulyasDataPoint(rOut, (*d).root()->m_samples[0], doneOrigin ? kPointBranch : kPointOrigin, 1);
		doneOrigin = true;
	}

	/*for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		WriteGulyasDataPoint(rOut, (*a).root()->m_samples[0], doneOrigin ? kPointBranch : kPointOrigin, 1);
		doneOrigin = true;
	}*/

	// the rest of the tree
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		OutputBranchGulyas(rOut, (*d).root());
	}

	/*for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		OutputBranchGulyas(rOut, (*a).root());
	}*/

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateGulyasTree(Input& rIn, const ValidationOptions& options)
{
	String line = rIn.readLine();

	if(line == "Segment types")
		return kValidationTrue;

	return kValidationFalse;
}

