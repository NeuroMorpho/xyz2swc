//
// FileFormatMaxSim.cpp
//

#include "ImportExportCommonIncludes.h"

#include <sstream>


/**

	MaxSim format

	Structure supported ->
		Soma		: NO
		Dendrite	: NO
		Axon		: BINARY
		Marker		: NO
		Spine		: NO

	Pending:
	*

**/

enum MaxSimIdentifier
{
	kAxon,
	kPIALine,
	kWhiteMatterLine
};

enum AxonNodeType
{
	kAxonStart					= 0,
	kAxonBifurcation			= 1,
	kAxonContinueOrTerminal		= 2
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

bool ImportMaxSim(Input& rIn, Neuron3D &nrn, const HintOptions& options)	
{
	MorphologyBuilder builder(nrn);

	static const String	kTagLastPoint("point");
	String			line;
	u32 			pointNumber;
	u32 			pointType;
	int 			parent;
	int 			daughter1;
	int 			daughter2;
	SamplePoint 	dataPoint;
	bool			doingAxon = false;

	while(rIn.remaining())
	{
		line = rIn.readLine();
		if(StringBegins(line, "# axon"))
		{
			doingAxon = true;
			builder.NewAxon();
		}
		else if(StringBegins(line, "#"))
		{
			doingAxon = false;
		}
		else
		{
			if(doingAxon)
			{
				std::istringstream ss(line, std::stringstream::in);
					ss >> pointNumber >> pointType >> parent >> daughter1 >> daughter2 >> dataPoint.x >> dataPoint.y >> dataPoint.z >> dataPoint.d;

				switch(pointType)
				{
				case kAxonStart:
					if( parent != -1 || daughter2 != -1)
					{
						LogImportFailure("At axon start, unexpected parent or 2nd daughter");
						return false;
					}

					builder.CbAddSample(dataPoint);
					break;

				case kAxonContinueOrTerminal:
					{
						if(parent+1 != pointNumber)
						{
							MorphologyBuilder::Branch parentBran = builder.CdFindBranchWithTag(kTagLastPoint, parent);
							if(parentBran == MorphologyBuilder::NullBranch())
							{
								LogImportFailure("Couldn't find branch parent: " + dec(parent));
								return false;
							}

							builder.SetCurrentBranch(parentBran);
							builder.CbNewChildLast(true);
						}

						builder.CbAddSample(dataPoint);

						if(daughter1 == -1)
						{
							if(daughter2 != -1)
							{
								LogImportFailure("Unexpected structure: daughter branch 2 exists, even though there is no daughter branch 1");
								return false;
							}

							builder.CbSetTag(kTagLastPoint, pointNumber);
						}
					}
					break;

				case kAxonBifurcation:

					if(parent+1 != pointNumber)
					{
						// 2 directly connected branch points - create the short branch
						MorphologyBuilder::Branch parentBran = builder.CdFindBranchWithTag(kTagLastPoint, parent);

						if(parentBran == MorphologyBuilder::NullBranch())
						{
							LogImportFailure("Couldn't find branch parent: " + dec(parent));
							return false;
						}

						builder.SetCurrentBranch(parentBran);
						builder.CbNewChildLast(true);
					}

					dataPoint.d = builder.CurrentBranch().GetLastSample().d;
					builder.CbAddSample(dataPoint);
					builder.CbSetTag(kTagLastPoint, pointNumber);
					builder.CbNewChildLast(true);
					break;

				default:
					LogImportFailure("Unexpected point type: " + dec(pointType));
					return false;
				}
			}
		}

		if(builder.HasFailed())
		{
			LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
			return false;
		}
	}

	builder.ApplyChanges();
	return true;
}


bool ImportRawMaxSim(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportMaxSim(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteMaxSimSampleLine(Output& rOut, u32 count, AxonNodeType type, int parent, int daughter1, int daughter2, const SamplePoint& sample)
{
	rOut.writeLine(		dec(count)		+ " " + 
						dec(type)		+ "   " + 
						dec(parent)		+ " " + 
						dec(daughter1)	+ " " + 
						dec(daughter2)	+ "     " +
						dec(sample.x,2) + " " + 
						dec(sample.y,2) + " " + 
						dec(sample.z,2) + 
						(type == kAxonContinueOrTerminal ?  (String(" ") + dec(sample.d,2)) : String("")) ); 
}

u32 CountNumberOfNodesBetween(Neuron3D::AxonTreeConstIterator it, Neuron3D::AxonTreeConstIterator itTarget, u32 count)
{
	if(it == itTarget)
		return count;

	count += (*it).m_samples.size() - 1;

	if(it.child())	
		count = CountNumberOfNodesBetween(it.child(), itTarget, count);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it))
		count = CountNumberOfNodesBetween(it.peer(), itTarget, count);

	return count;
}

u32 ExportMaxSimBranch(Output& rOut, Neuron3D::AxonTreeConstIterator it, u32 parent, u32 count)
{
	const u32 firstNode = (it.parent() == Neuron3D::AxonTree::null()) ? 0 : 1;
	const u32 lastNode = (*it).m_samples.size()-1;

	// only if this is the root branch of the dendrite
	if(firstNode == 0)
	{
		++count;
		WriteMaxSimSampleLine(rOut, count, kAxonStart, -1, count+1, -1, (*it).m_samples[0]);
	}

	// for branches with 3 or more sample points
	for(u32 n=1;n<lastNode;++n)
	{
		++count;
		WriteMaxSimSampleLine(rOut, count, kAxonContinueOrTerminal, (n == firstNode) ? parent : count-1, count+1, -1, (*it).m_samples[n]);
	}

	// always do the last node
	const bool terminal = ! it.child();
	const AxonNodeType finalType = terminal ? kAxonContinueOrTerminal : kAxonBifurcation;
	++count;
	WriteMaxSimSampleLine(rOut, count, finalType, (firstNode == lastNode) ? parent : count-1, (terminal ? -1 : count+1), (terminal ? -1 : CountNumberOfNodesBetween(it.child(), it.child().peer(), count + 1) ), (*it).m_samples[(*it).m_samples.size()-1] );

	//
	if(it.child())	
		count = ExportMaxSimBranch(rOut, it.child(), count, count);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it))
		count = ExportMaxSimBranch(rOut, it.peer(), parent, count);

	return count;
}

bool ExportMaxSim(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	const u32 totalPoints = nrn.CountAllAxonSamplePoints();
	const u32 numAxons = nrn.CountAxons();

	rOut.writeLine("# number of nodes: " + dec(totalPoints));

	u32 axonCount=0;
	u32 sampleCount=0;
	for(Neuron3D::AxonConstIterator it = nrn.AxonBegin();it != nrn.AxonEnd(); ++it, ++axonCount)
	{
		rOut.writeLine("# axon: " + dec(axonCount) + " " + dec( nrn.CountAxonSamplePoints((*it).root() ) ));
		sampleCount = ExportMaxSimBranch(rOut, (*it).root(), -1, sampleCount);
	}

	if(sampleCount != totalPoints)
	{
		LogExportWarning("Sample count (" + dec(sampleCount) + " is not as expected (" + dec(totalPoints) + ").");
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateMaxSim(Input& rIn, const ValidationOptions& options)
{
	String line1 = rIn.readLine();
//	String line2 = rIn.readLine();

	if(StringBegins(line1, "# number of nodes: ") /*&& line2.begins()*/)
		return kValidationTrue;

	return kValidationFalse;
}
