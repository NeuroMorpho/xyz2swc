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

	Amaral format


	http://www.neuron.yale.edu/phpbb/viewtopic.php?f=13&t=305

	Each data line contains 7 entries
            id              index of line (0 is "root" or first measurement)
            par             index of parent measurement
            x, y, z         coords where measurement was made
            len, diam       distance from parent measurement, diameter
    The first line of data actually defines the root of the tree as a cylinder with
    dimensions len0 x diam0

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

bool ImportAmaral(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	String line;
	SamplePoint dataPoint;
	int segment;
	int parent;
	float len;

	MorphologyBuilder builder(nrn);

	const String& kTagSampleNumber	= "samplenum";

	// first line should be number of data points
	int numDataPoints;
	line = rIn.readLine();
	istringstream ss(  line.c_str(), stringstream::in);
	ss >> numDataPoints;

	if(numDataPoints == 0)
		LogImportWarning("Zero data points!");


	//
	int pointsRead = 0;
	while( rIn.remaining() > 0 )
	{
		line = rIn.readLine();

		// some simple checks..
		if(line.length() < 10)
		{
			LogImportWarning("Terminating early due to short non-whitespace line - not long enough to contain all data for a sample point.");
			break;
		}

		//
		istringstream ss(line.c_str(), stringstream::in);
		ss >> segment >> parent >> dataPoint.x >> dataPoint.y >> dataPoint.z >> len >> dataPoint.d;

		if(pointsRead != segment)
		{
			LogImportFailure("Points are not in order: " + std::to_string(segment) + "(expected " + std::to_string(pointsRead) + ")" );
			return false;
		}

		if(pointsRead == 0)
		{
			if(parent != -1)
				LogImportWarning("Initial soma point does not have parent of expected value (-1): " + std::to_string(parent) );

			// this is single soma point
			builder.NewSomaSinglePoint(dataPoint);
		}
		else
		{
			if(parent == 0)
			{
				builder.NewDendriteWithSample(dataPoint);
				builder.CbSetTag(kTagSampleNumber, segment+1);
			}
			else
			{
				if(segment != parent+1)
				{
					MorphologyBuilder::Branch parentBran = builder.CdFindBranchWithTagCondition(kTagSampleNumber, MorphologyBuilder::CheckIfTagInRangeFunctor(parent));
					const bool unacceptableParent = (parentBran == MorphologyBuilder::NullBranch());

					if(unacceptableParent)
					{
						LogImportFailure("Cannot locate parent branch!! Very strange!");
						return false;
					}

					builder.SetCurrentBranch(parentBran);

					const int startsegment = builder.CbGetTag(kTagSampleNumber);
					const int size = builder.CurrentBranch().NumSamples();

					assert(parent <= startsegment + size-2);

					// splitting a branch to make a branch point
					if(parent < startsegment + size-2)
					{
						builder.CbSplit(parent - (startsegment-1));
						builder.CbSetTag(kTagSampleNumber, parent + 1);
						builder.SetCurrentBranch(parentBran);
					}
					// otherwise looks like we have a non-binary tree since this node is already a branch point

					builder.CbNewChildLast(true);
					builder.CbSetTag(kTagSampleNumber, segment);
				}

				builder.CbAddSample(dataPoint);
			}

			
			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}

		}

		++pointsRead;
	}


	if(pointsRead != numDataPoints)
		LogImportWarning( String("Points expected: ") + std::to_string(numDataPoints) + ", points read: " + std::to_string(pointsRead) );


	builder.ApplyChanges();

	nrn.MergeAllUnaryBranchPoints();
	nrn.EnhanceSingleSampleTrees();

	return true;
}

bool ImportRawAmaral(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportAmaral(*pIn, nrn, options);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteAmaralDataPoint(Output& rOut, int segment, int parent, const SamplePoint& sample, float length)
{
	rOut.writeLine(	dec(segment)		+ " " + 
					dec(parent)			+ " " + 
					dec(sample.x, 2)	+ " " +
					dec(sample.y, 2)	+ " " +
					dec(sample.z, 2)	+ " " +
					dec(length, 5)		+ " " +
					dec(sample.d, 2) );
}

void OutputSomaAmaral(Output& rOut, const Neuron3D &nrn, SamplePoint& sample)
{
	v4f center = nrn.CalculateSomaCentre();

	sample.x = center.GetX();
	sample.y = center.GetY();
	sample.z = center.GetZ();
	sample.d = center.GetW();
	float len = 0.0f;

	if(nrn.CountSomas() > 0)
	{
	}
	else
	{
		// 
	}

	WriteAmaralDataPoint(rOut,  0, -1, sample, len);
}


int OutputBranchAmaral(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int branchparent, int segment)
{
	int parent = branchparent;

	const u32 numSamples = (u32) (*it).m_samples.size();

	for(u32 i=1;i<numSamples;++i)
	{
		const float length = (*it).m_samples[i].Distance( (*it).m_samples[i-1] );
		WriteAmaralDataPoint(rOut, segment, parent, (*it).m_samples[i], length );
		parent = segment;
		segment++;
	}

	if(it.child()) segment = OutputBranchAmaral(rOut, it.child(), parent, segment);
	if(it.peer()) segment = OutputBranchAmaral(rOut, it.peer(), branchparent, segment);

	return segment;
}

bool ExportAmaral(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	const int totalSamples = 1 + nrn.CountAllAxonSamplePoints() + nrn.CountAllDendriteSamplePoints();

	rOut.writeLine( dec(totalSamples) );

	SamplePoint somaPoint;
	OutputSomaAmaral(rOut, nrn, somaPoint);

	int segment = 1;
	int branchparent = 0;

/*	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		const SamplePoint rootPoint = (*a).root()->m_samples[0];
		const float length = rootPoint.Distance(somaPoint);
		
		WriteAmaralDataPoint(rOut, segment, 0, rootPoint, length);
		
		branchparent = segment;
		segment++;

		segment = OutputBranchAmaral(rOut, (*a).root(), branchparent, segment);
	}*/

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		const SamplePoint rootPoint = (*d).root()->m_samples[0];
		const float length = rootPoint.Distance(somaPoint);

		WriteAmaralDataPoint(rOut, segment, 0, (*d).root()->m_samples[0], length);
		
		branchparent = segment;
		segment++;

		segment = OutputBranchAmaral(rOut, (*d).root(), branchparent, segment);
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateAmaral(Input& rIn, const ValidationOptions& options)
{
	const int kMaxLines=4;
	int testedLines = 0;

	{
		int sampleCount;
		String line = rIn.readLine();
		istringstream ss(line.c_str(), stringstream::in);
		ss >> sampleCount;

		if(sampleCount == 0 || line.length() > 14)
			return kValidationFalse;
	}

	while(rIn.remaining() && testedLines < kMaxLines)
	{
		int id, par;
		float x,y,z,len,diam;

		String line = rIn.readLine();
		istringstream ss(line.c_str(), stringstream::in);
		ss >> id >> par >> x >> y >> z >> len >> diam;

		if(id != testedLines)
			return kValidationFalse;

		if(testedLines == 0 && par != -1)
			return kValidationFalse;

		++testedLines;
	}


	return kValidationTrue;
}
