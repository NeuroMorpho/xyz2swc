//
//
//

#include "ImportExportCommonIncludes.h"

#include <iostream>
#include <sstream>
#include <functional>

#include <algorithm>
#include <math.h>
#include <float.h>

#include "Core/Parser.h"


/**

	Nevin format

	@status

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class NevinParser : public SimpleParser
{
public:
	NevinParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn), m_nrn(nrn)
	{
	}

	bool Parse()
	{
		//String comment;
		String item;

		// skip comment block (if there is one)
		SeekNextItem();

		u32 remaining = rIn.remaining();
		item = GetNextItem();

		if( StringBegins(item, "/*") )
		{
			if( !SeekPastSubstring("*/") )
			{
				LogImportFailure( String("Unterminated comment - cant find '*/' ") );
				return false;
			}

			// make sure we are at start of data lines
			SeekStartOfNextLine();
		}
		else
		{
			// return to start of line
			//!!
			while(rIn.remaining() < remaining)
				rIn.rollback();
		}

		bool doingSomaAndPrimaries = true;
		bool doneSomaAndPrimaries = false;

		bool addChildToCurrentBranch = false;

		// read them all in and handle appropriately
		while(rIn.remaining() > 0)
		{
			u32 pointNumber;
			char pointType;
			SamplePoint dataPoint;

			pointNumber = std::stoi(GetNextItem());
			SeekNextItem();
			pointType = GetNextItem()[0];
			SeekNextItem();
			dataPoint.x = std::stof(GetNextItem());
			SeekNextItem();
			dataPoint.y = std::stof(GetNextItem());
			SeekNextItem();
			dataPoint.z = std::stof(GetNextItem());
			SeekNextItem();
			dataPoint.d = std::stof(GetNextItem());

			String line = rIn.readLine();



			if(addChildToCurrentBranch)
			{
				builder.CbNewChildLast(true);
				addChildToCurrentBranch = false;
			}


			switch( pointType ) 
			{
			case 'P':
				if(!doingSomaAndPrimaries)
				{
					//printf("Oops - found P when we thought they were finished..removing any previous C/F points!\n");
					SamplePoint firstSample = builder.CurrentBranch().GetSample(0);
					builder.CbClearSamples();
					builder.CbAddSample(firstSample);
					doingSomaAndPrimaries = true;
				}
				else
				{
					builder.NewDendriteWithSample(dataPoint);
					builder.CdMoveToPosition(0);
					//printf("New P - creating Dendrite\n");
				}
				break;

			case 'S':
				assert(doingSomaAndPrimaries);

				if( builder.NumSomas() == 0 )
				{
					builder.NewSomaOpenContour();
					//printf("First S - creating Soma\n");
				}

				builder.CsAddSample(dataPoint);
				break;

			case 'B':
				if(doingSomaAndPrimaries)
				{
					//printf("Terminating S/Ps due to B\n");
					doingSomaAndPrimaries = false;
					if(builder.NumDendrites() == 0)
					{
						//printf("No dendrites, so start one with this B\n");
						builder.NewDendriteWithSample( builder.CurrentSoma().GetLastSample() );
					}
				}

				builder.CbAddSample(dataPoint);
				builder.CbNewChildLast(true);
				break;

			case 'F':
			case 'C':

				// skip any F/C before we get S/P
				if(builder.NumDendrites() == 0 && builder.NumSomas() == 0)
				{
					//printf("Skipping C/F which has appeared too early\n");
					break;
				}

				if(doingSomaAndPrimaries)
				{
					doingSomaAndPrimaries = false;
					if(builder.NumDendrites() == 0)
					{
						//printf("No dendrites, so start one with this C\n");
						builder.NewDendriteWithSample( builder.CurrentSoma().GetLastSample() );
					}
				}

				//
				if(builder.CurrentDendrite() == builder.DendriteEnd())
				{
					//printf("Looks like not enough P points for all the dendrites - creating new dendrite\n");
					builder.NewDendrite();
				}

				builder.CbAddSample(dataPoint);
				break;

			case 'T':
			case 'E':
				if (doingSomaAndPrimaries) 
				{
					if( builder.NumSomas() == 0 )
					{
						builder.NewSomaOpenContour();
						//printf("T, no previous S - creating Soma\n");
					}

					builder.CsAddSample(dataPoint);
					doingSomaAndPrimaries = false;
				}
				else
				{
					// TODO redo this, not quite right...
					//if(builder.CurrentDendrite().GetRoot().NumSamples() == 1)
					//{
					//	builder.CsAddSample(dataPoint);
					//	break;
					//}
					//

					builder.CbAddSample(dataPoint);

					if(builder.CbAscendToUnaryBranchPoint())
						addChildToCurrentBranch = true;
						//builder.CbNewChildLast(true);
					else
					{
						//printf("No more branch points - moving to next dendrite\n");
						builder.NextDendrite();
					}
				}
				break;

				// ignore fiducial points
			case 'c':
				LogImportWarning( String("Ignoring 'c' point type") );
				break;
			case 'f':
				LogImportWarning( String("Ignoring 'f' point type") );
				break;

			default:
				LogImportWarning( String("Ignoring unexpected point type") );
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

private:
	MorphologyBuilder builder;
	Neuron3D& m_nrn;
};

bool ImportNevin(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	NevinParser parser(rIn, nrn);
	return parser.Parse();
}


bool ImportRawNevin(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNevin(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteNevinDataPoint(Output& rOut, u32 segmentcount, String id, const SamplePoint& sample)
{
	String out =	dec(segmentcount) + " \t " + id + 
					PrependSpaces(dec(sample.x, 2), 8, 3) + " " +
					PrependSpaces(dec(sample.y, 2), 5) + " " +
					PrependSpaces(dec(sample.z, 2), 5) + " \t " +
					dec(sample.d, 2);

	rOut.writeLine( out );
}

int ExportNevinSomaAndRootPoints(Output& rOut, const Neuron3D &nrn, int segment)
{
	// write any soma points 
	const u32 numSomaSamples = (nrn.CountSomas() > 0) ? nrn.SomaBegin()->m_samples.size() : 0;
	if(numSomaSamples > 0)
	{
		for(u32 n=0;n<numSomaSamples-1;++n)
			WriteNevinDataPoint(rOut, ++segment, String("S"), nrn.SomaBegin()->m_samples[n] );
	}

	// now the root point of the dendrites (in reverse order, since that is how they get constructed)
	std::vector<SamplePoint> reverseOrderSamples;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		reverseOrderSamples.push_back( (*d).root()->m_samples[0] );

	const u32 numDends = reverseOrderSamples.size();
	for(u32 n=0;n<numDends;++n)
		WriteNevinDataPoint(rOut, ++segment, String("P"), reverseOrderSamples[numDends - (n+1)]);

	// now write T?
	if(numSomaSamples > 0)
		WriteNevinDataPoint(rOut, ++segment, String("T"), nrn.SomaBegin()->m_samples[numSomaSamples-1] );

	return segment;
}

int ExportNevinBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int segment)
{
	String id;

	for(u32 i=1; i<(*it).m_samples.size();++i)
	{
		id = "C";

		if(i == (*it).m_samples.size() - 1)
		{
			if(it.child())
			{
				if(it.child().peer())
					id = "B";
			}
			else
				id = "T";
		}

		++segment;
		WriteNevinDataPoint(rOut, segment, id, (*it).m_samples[i]);
	}

	if(it.child()) 
		segment = ExportNevinBranch(rOut, it.child(), segment);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it)) 
		segment = ExportNevinBranch(rOut, it.peer(), segment);

	return segment;
}

bool ExportNevin(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	rOut.writeLine("/*");
	rOut.writeLine("DIG version");
	rOut.writeLine("Comments: Generated by " /*+ GetThisSoftwareName()*/ );
	rOut.writeLine("");
	rOut.writeLine(" */ ");

	int segmentcount = 0;

	segmentcount = ExportNevinSomaAndRootPoints(rOut, nrn, segmentcount);

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		segmentcount = ExportNevinBranch(rOut, (*d).root(), segmentcount);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

static bool IsValidNevinPointType(char sampleType)
{
	if(	sampleType == 'P' ||
		sampleType == 'S' ||
		sampleType == 'B' ||
		sampleType == 'F' ||
		sampleType == 'C' ||
		sampleType == 'T' ||
		sampleType == 'E' ||
		sampleType == 'f' ||
		sampleType == 'c')
	{
		return true;
	}

	return false;
}

ValidationResult ValidateNevin(Input& rIn, const ValidationOptions& options)
{
	bool hasDIG = false;
	bool hasSpaces = false;
	bool hasSpace = false;
	bool hasNED = false;

	u32 numChecked = 0;
	u32 maxCheck = 4;

	String line;

	while(rIn.remaining() && numChecked < maxCheck)
	{
		line = rIn.readLine();

		if(line == "")
		{
			continue;
		}
		else if(StringBegins(line, "/*"))
		{
			while(rIn.remaining() > 0)
			{
				line = rIn.readLine();
				if( StringContains(line, "DIG") )
					hasDIG = true;
				if(StringContains(line, " */ ") )
					hasSpaces = true;
				if(StringContains(line, " */") )
					hasSpace = true;
				if(StringContains(line, "NED Version") )
					hasNED = true;

				if(hasDIG && hasSpaces)
					return kValidationTrue;

				if(hasDIG && hasNED && hasSpace)
					return kValidationTrue;

				if(StringContains(line, "*/"))
					break;
			}
		}
		else
		{
			int sampleId;
			char sampleType;
			float x, y, z, d;

			// check line format
			istringstream ss(line, stringstream::in);
			ss >> sampleId >> sampleType >> x >> y >> z >> d;

			if(ss.fail() || ss.bad())
				return kValidationFalse;

			if(!IsValidNevinPointType(sampleType))
				return kValidationFalse;

		}

		++numChecked;
	}

	return kValidationTrue;
}


