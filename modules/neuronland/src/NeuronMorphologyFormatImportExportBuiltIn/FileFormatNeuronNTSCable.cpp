//
// FileFormatNeuronNTSCable.cpp
//

#include "ImportExportCommonIncludes.h"

#include <map>
#include <vector>

using std::vector;

#include <iostream>
#include <sstream>

using std::stringstream;
using std::istringstream;


/**

	NTSCable 2.0 generates a file suitable for inclusion in a Neuron Simulator script

	Here we provide functions for loading/saving this style of script.

	NOTE: import/export code is now called via NeuronHOC.

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

static String SeekLineBeginning(Input& rIn, const String& str)
{
	String line;
	while(rIn.remaining() > 0)
	{
		line = rIn.readLine();

		if(StringBegins(line, str))
			return line;
	}

	return String("");
}

static bool SeekLineAfterText(Input& rIn, const String& text)
{
	String line = SeekLineBeginning(rIn, text);

	return !line.empty();
}

typedef vector<SamplePoint> SampleSet;

void AddSamplesToCurrentBranch(MorphologyBuilder &builder, SampleSet samples)
{
	for(SampleSet::const_iterator it = samples.begin(); it != samples.end();++it)
		builder.CbAddSample(*it);
}

bool ImportNeuronNTSCable(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	// first clear out any structure already in this neuron
	nrn.Clear();

	MorphologyBuilder builder(nrn);

	String line;

	// get number of dendrites
	const String primaryBranchesCell("primary_branches_cell = ");
	line = SeekLineBeginning(rIn, primaryBranchesCell);
	if(line.empty())
		return false;
	const u32 numDendrites = std::stoi( StringAfter(line, primaryBranchesCell) );

	// get number of segments
	const String segmentsCell("nseg_cell = ");
	line = SeekLineBeginning(rIn, segmentsCell);
	if(line.empty())
		return false;
	const u32 numSections = std::stoi(StringAfter(line, segmentsCell) ) - 1; // -1 for soma

	// get number of segments per dendrite
	u32 *segmentsPerDendrite = new u32[numDendrites];
	u32 segmentValidationCount = 0;
	for(u32 n=0;n<numDendrites;++n)
	{
		const String segmentsCell("sections_cell[");
		line = SeekLineBeginning(rIn, segmentsCell);
		if(line.empty())
			return false;
		segmentsPerDendrite[n] = std::stoi(StringAfter(line, "= ") );
		segmentValidationCount += segmentsPerDendrite[n];
	}

	u32 numSegments;

	if(segmentValidationCount == numSections)
	{
		numSegments = numSections;
	}
	else
	{
		const String segmentsCell("nsec_cell = ");
		line = SeekLineBeginning(rIn, segmentsCell);
		if(line.empty())
			return false;
		numSegments = std::stoi(StringAfter(line, segmentsCell)) - 1; // -1 for soma
	}

	assert(segmentValidationCount == numSegments);


	// lets seek to a couple of bits of expected text just to be sure this is a suitable file
	if( !SeekLineAfterText(rIn, "proc geometry() { local i, j") )
		return false;

	if( !SeekLineAfterText(rIn, "geometry()") )
		return false;


	// now should be at the main data
	SamplePoint sample;

	// soma....
	if( !SeekLineAfterText(rIn, "SOMA COORDINATES AND DIAMETERS:") )
		return false;

	// skip empty line
	line = rIn.readLine();
	assert(line.empty());

	// num points
	u32 numSomaPoints;
	line = rIn.readLine();
	istringstream isSoma(line, istringstream::in);
	isSoma >> numSomaPoints;

	if(numSomaPoints)
	{
		builder.NewSomaOpenContour();

		for ( u32 k=0; k<numSomaPoints ; k++ ) 
		{
			line = rIn.readLine();
			istringstream isSample(line, istringstream::in);
			isSample >> sample.x >> sample.y >> sample.z >> sample.d;
			builder.CsAddSample(sample);
		}
	}

	// dendrites....
	if( !SeekLineAfterText(rIn, "NEURITE COORDINATES AND DIAMETERS:") )
		return false;

	// skip empty line
	line = rIn.readLine();
	assert(line.empty());

	vector<SampleSet> *branches = new vector<SampleSet>[numDendrites];

	u32 segmentCount = 0;
	u32 currentDend = 0;
	u32 nextDendriteStartSeg = segmentsPerDendrite[0];

	while ( rIn.remaining() > 0 && segmentCount < numSegments)
	{
		u32 numDendritePoints;
		line = rIn.readLine();
		istringstream isDendrite(line, istringstream::in);
		isDendrite >> numDendritePoints >> numDendritePoints;

		SampleSet branchSamples;
		for (u32 k=0;k<numDendritePoints;k++)
		{
			line = rIn.readLine();
			istringstream isSample(line, istringstream::in);
			isSample >> sample.x >> sample.y >> sample.z >> sample.d;

			branchSamples.push_back(sample);
		}

		assert(numDendritePoints >= 2);
		branches[currentDend].push_back(branchSamples);

		// skip space between segment data
		line = rIn.readLine();
		assert(line.empty());

		++segmentCount;
		if(segmentCount == nextDendriteStartSeg)
		{
			++currentDend;
			nextDendriteStartSeg += segmentsPerDendrite[currentDend];
		}
	}

	for(u32 d=0;d<numDendrites;++d)
	{
		assert(segmentsPerDendrite[d] = branches[d].size());
	}

	// connections....
	if( !SeekLineAfterText(rIn, "CONNECTIONS:") )
		return false;

	// skip empty line
	line = rIn.readLine();
	assert(line.empty());

	u32 dendriteCount = 0;
	u32 segmentsAttachedToDend = 0;

	std::map<int, MorphologyBuilder::Branch> connectionMap;

	while ( rIn.remaining() > 0 && dendriteCount < numDendrites)
	{
		assert( branches[dendriteCount].size() > 0);
		assert( branches[dendriteCount].size() == segmentsPerDendrite[dendriteCount] );

		if(segmentsAttachedToDend == 0)
		{
			builder.NewDendrite();
			AddSamplesToCurrentBranch(builder, branches[dendriteCount][0]);

			++segmentsAttachedToDend;

			connectionMap[0] = builder.CurrentBranch();
		}

		u32 connectedTo;
		u32 one;

		while(rIn.remaining() > 0 && segmentsAttachedToDend < segmentsPerDendrite[dendriteCount])
		{
			line = rIn.readLine();
			istringstream isSample(line, istringstream::in);
			isSample >> connectedTo >> one;
			assert(one == 1);

			builder.SetCurrentBranch(connectionMap[connectedTo]);
			builder.CbNewChildLast(false);
			AddSamplesToCurrentBranch(builder, branches[dendriteCount][segmentsAttachedToDend]);
			
			connectionMap[segmentsAttachedToDend] = builder.CurrentBranch();

			++segmentsAttachedToDend;
		}

		// skip empty line (dendrite with one segment contribute 1 empty line)
		line = rIn.readLine();
		assert(line.empty());

		segmentsAttachedToDend = 0;
		++dendriteCount;

		if(builder.HasFailed())
			return false;
	}

	u32 m=0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d, ++m)
	{
		assert( (*d).size() == segmentsPerDendrite[m]);
	}

	delete[] branches;
	delete[] segmentsPerDendrite;

	builder.ApplyChanges();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//
/*
static String RemovePointZero(const String& str)
{
	if(str.after(".") == "0")
		return str.before(".");

	return str;
}*/

void WriteSamplePoint(Output& rOut, const SamplePoint& sample)
{
	rOut.writeLine(	PrependSpaces( dec(sample.x,2), 8) +
					PrependSpaces( dec(sample.y,2), 9) +
					PrependSpaces( dec(sample.z,2), 9) +
					PrependSpaces( dec(sample.d,2), 9) );
}

struct NeuronDetails
{
	String 	filename;
	String 	timeDate;
	String 	inputFormat;
	String 	outputFormat;
	float 	somaDiam;
	float 	somaLength;
	float 	somaArea;
	u32		soma3DPoints;
	u32 	somaOutlinePoints;
	u32		somaStartPoint;
	float 	somaOutlineDiameter;
	u32 	numPrimaryNeurites;
	u32 	numBranches;
	float 	totalBranchLength;
	float 	totalBranchArea;
	u32 	numTreePoints;
	u32 	numTreeSegments;
	u32		numTreeSegmentsRequested;
	float	segmentConstrainedLength;
	u32 	finalNumPoints;
	u32 	finalNumTrees;
	float	maxDxCell;
	vector<u32> segsPerDend;
};

void CollectNeuronDetails(const Neuron3D &nrn, NeuronDetails& details)
{
	details.filename = "TODO";
	details.timeDate = "TODO";
	details.inputFormat = "TODO";
	details.outputFormat = "NEURON";
	details.somaDiam = 0.0f;
	details.somaLength = 0.0f;
	details.somaArea = 0.0f;
	details.soma3DPoints = (nrn.CountSomas() > 0) ? nrn.SomaBegin()->m_samples.size() : 0;
	details.somaOutlinePoints = 0;
	details.somaStartPoint = 1;
	details.somaOutlineDiameter = 0.0f;
	details.numPrimaryNeurites = nrn.CountDendrites();
	details.numBranches = 0;
	details.totalBranchLength = 0.0f;
	details.totalBranchArea = 0.0f;
	details.numTreePoints = 0;
	details.numTreeSegments = 0;
	details.numTreeSegmentsRequested = 0;
	details.segmentConstrainedLength = 0;
	details.finalNumPoints = 0;
	details.finalNumTrees = 0;
	details.maxDxCell = 0.0f;

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		const u32 numSegs = (*d).size();
		details.numTreeSegments += numSegs;
		details.segsPerDend.push_back( numSegs );

		//details.numTreePoints += CountBranchPoints( (*d).root() );
	}
}


void WriteHeader(Output& rOut, const NeuronDetails& details)
{
	rOut.writeLine("/*----------------------------------------------------------------");
	rOut.writeLine("$Header$");
	rOut.writeLine(details.filename + "  translated " + details.timeDate + " by ntscable 2.0");
	rOut.writeLine("source file syntax: " + details.inputFormat);
	rOut.writeLine("output file syntax: " + details.outputFormat);
	rOut.writeLine("soma: diameter = " + dec(details.somaDiam,4) + " um  length = " + dec(details.somaLength,4) + " um  area = " + dec(details.somaArea,4) + " um2");
	rOut.writeLine( String("      ") + dec(details.soma3DPoints) + " three-D points; " + dec(details.somaOutlinePoints) + " outline points numbered " + dec(details.somaStartPoint) + "-" + dec(details.somaStartPoint + details.somaOutlinePoints -1));
	rOut.writeLine("      outline diameter = " + dec(details.somaOutlineDiameter) + " um" );
	rOut.writeLine(dec(details.numPrimaryNeurites) + " primary neurites" );
	rOut.writeLine(dec(details.numBranches) + " branches totaling " + dec(details.totalBranchLength, 2) + " um in length, " + dec(details.totalBranchArea, 1) + " um2 in area");
	rOut.writeLine(dec(details.numTreePoints) + " tree points translated to " + dec(details.numTreeSegments + 1) + " segments (" + dec(details.numTreeSegmentsRequested) + " requested)");
	rOut.writeLine("Neurites divided into segments of equal dx between adjacent digitized");
	rOut.writeLine("branch points.");
	rOut.writeLine("Segment length constrained to be < " + dec(details.segmentConstrainedLength,2) + " um.");
	rOut.writeLine("No. points" + PrependSpaces(dec(details.finalNumPoints), 7) );
	rOut.writeLine("No. trees" + PrependSpaces(dec(details.finalNumTrees), 8) );
	rOut.writeLine("");
	rOut.writeLine("");
	rOut.writeLine("NOTE - This file is generated by " /*+ GetThisSoftwareName() +*/  " in the format ");
	rOut.writeLine("	   generated by ntscable 2.0, with identical comment layout.");
	rOut.writeLine("	   This file is NOT actually generated by NTSCable");
	rOut.writeLine("----------------------------------------------------------------*/");
	rOut.writeLine("");
}

void WriteExplicitGeometry(Output& rOut, const NeuronDetails& details)
{
	u32 numDends = details.numPrimaryNeurites;
	u32 numSegments = details.numTreeSegments;

	rOut.writeLine("\t\t\t\t\t/* make geometry explicit */");
	rOut.writeLine("primary_branches_cell = " + dec(numDends));
	rOut.writeLine("branches_cell = " + dec(details.numBranches));
	rOut.writeLine("max_dx_cell = " + dec(details.maxDxCell,2));
	rOut.writeLine("points_cell = " + dec(details.numTreePoints));
	rOut.writeLine("nseg_cell = " + dec(details.numTreeSegments + 1));
	rOut.writeLine("double sections_cell[" + dec(numDends) + "]");

	for(u32 i=0;i<numDends;++i)
		rOut.writeLine("sections_cell[" + dec(i) + "] = " + dec(details.segsPerDend[i]));

	rOut.writeLine("nsec_cell = " + dec(details.numTreeSegments+1));
	rOut.writeLine("");
	rOut.writeLine("strdef name_soma, name_section");
	rOut.writeLine("name_soma = \"soma\"");
	rOut.writeLine("name_section = \"dend\"");
	rOut.writeLine("use_axon = 0\t\t\t\t\t/* create sections */");
	rOut.writeLine("create    soma,\\");

	for(u32 i=0;i<numDends;++i)
		rOut.writeLine("          dend" + dec(i+1) + "[" + dec(details.segsPerDend[i]) + "],\\"); // wrong

	rOut.writeLine("");
	rOut.writeLine("");
	rOut.writeLine("");
}

void WriteGeometryProcedure(Output& rOut, u32 numDends, vector<u32> segsPerDend)
{
	rOut.writeLine("/*----------------------------------------------------------------*/");
	rOut.writeLine("proc geometry() { local i, j");
	rOut.writeLine("");

	// soma
	rOut.writeLine("\t\t\t\t\t\t/* soma geometry */");
	rOut.writeLine("    soma {");
	rOut.writeLine("        nseg = 1");
	rOut.writeLine("        pt3dclear()");
	rOut.writeLine("        for j = 1, fscan() {");
	rOut.writeLine("            pt3dadd(fscan(),fscan(),fscan(),fscan())");
	rOut.writeLine("        }");
	rOut.writeLine("    }");
	rOut.writeLine("");

	// soma - dendrite
	rOut.writeLine("\t\t\t\t\t/* connect primary neurites */");
	for(u32 i=0;i<numDends;++i)
	{
		rOut.writeLine("    soma connect dend" + dec(i+1) + "[0] (0), 0.5");
	}
	rOut.writeLine("");
	rOut.writeLine("");

	// dendrites
	rOut.writeLine("\t\t\t\t\t/* neurite geometry*/");
	for(u32 i=0;i<numDends;++i)
	{
		rOut.writeLine("    for i = 0," + dec(segsPerDend[i] - 1) + " {");
		rOut.writeLine("        dend" + dec(i+1) + "[i] {");
		rOut.writeLine("            nseg = fscan()");
		rOut.writeLine("            pt3dclear()");
		rOut.writeLine("            for j = 1, fscan() {");
		rOut.writeLine("                pt3dadd(fscan(),fscan(),fscan(),fscan())");
		rOut.writeLine("            }");
		rOut.writeLine("        }");
		rOut.writeLine("    }");
		rOut.writeLine("");
	}
	rOut.writeLine("");
	rOut.writeLine("");

	// connections
	rOut.writeLine("\t\t\t\t\t/* branching topology*/");
	for(u32 i=0;i<numDends;++i)
	{
		rOut.writeLine("    for i = 1," + dec(segsPerDend[i] - 1) + " {");
		rOut.writeLine("        dend" + dec(i+1) + "[fscan()] connect dend" + dec(i) + "[i] (0), fscan()");
		rOut.writeLine("    }");
		rOut.writeLine("");
	}

	rOut.writeLine("}");
	rOut.writeLine("");
	rOut.writeLine("geometry()");
	rOut.writeLine("");
	rOut.writeLine("");
}

// soma
void WriteSomaNTSCable(Output& rOut, const Neuron3D& nrn)
{
	// soma
	rOut.writeLine("SOMA COORDINATES AND DIAMETERS:");
	rOut.writeLine("");

	bool haveSoma = nrn.CountSomas() > 0;
	rOut.writeLine("    " + dec(haveSoma ? (u32)nrn.SomaBegin()->m_samples.size() : 1) );

	if(haveSoma)
	{
		const vector<SamplePoint> &samples = nrn.SomaBegin()->m_samples;
		vector<SamplePoint>::const_iterator it;

		for(it = samples.begin(); it != samples.end(); ++it)
			WriteSamplePoint(rOut, (*it));

		rOut.writeLine("");
		rOut.writeLine("");
	}
	else
	{
		WriteSamplePoint(rOut, SamplePoint(0.0f, 0.0f, 0.0f, 1.0f) );

		rOut.writeLine("");
		rOut.writeLine("");
	}
}

// dendrites
static bool WriteBranchData(Output& rOut, Neuron3D::DendriteTreeConstIterator it)
{
	const SampleSet &samples = (*it).m_samples; 
	assert(samples.size());

	rOut.writeLine("    1 " + dec( (u32)samples.size()));

	for(SampleSet::const_iterator p = samples.begin(); p != samples.end(); ++p)
		WriteSamplePoint(rOut, *p);
	
	rOut.writeLine("");

	if( it.child() )	WriteBranchData(rOut, it.child());
	if( it.peer() )		WriteBranchData(rOut, it.peer());

	return true;
}

void WriteDendrites(Output& rOut, const Neuron3D& nrn)
{
	// dendrites
	rOut.writeLine("NEURITE COORDINATES AND DIAMETERS:");
	rOut.writeLine("");

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		WriteBranchData( rOut, (*d).root());

	rOut.writeLine("");
	rOut.writeLine("");
}

// connections
static int WriteBranchConnections(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int parent, int total)
{
	rOut.writeLine(	PrependSpaces(dec(parent), 9) + String("    1") );

	++total;

	if( it.child() ) total = WriteBranchConnections(rOut, it.child(), total, total);
	if( it.peer() ) total = WriteBranchConnections(rOut, it.peer(), parent, total);

	return total;
}

static void WriteConnections(Output& rOut, const Neuron3D& nrn)
{
	rOut.writeLine("CONNECTIONS:");

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		rOut.writeLine("");

		if((*d).root().child())
			WriteBranchConnections(rOut, (*d).root().child(), 0, 0);
	}
}

void WriteFooter(Output& rOut)
{
	rOut.writeLine("");
	rOut.writeLine("");
	rOut.writeLine("/*----------------------------------------------------------------*/");
	rOut.writeLine("proc geometry() { ");
	rOut.writeLine("");
	rOut.writeLine("\t/* NULL geometry procedure: keeps the user from");
	rOut.writeLine("\tcalling the geometry procedure in isolation. */");
	rOut.writeLine("");
	rOut.writeLine("    printf(\"You must re-read the entire geometry\\n\")");
	rOut.writeLine("    printf(\"file to execute geometry().\\n\\n\")");
	rOut.writeLine("}");
	rOut.writeLine("");
	rOut.writeLine("");
}


bool ExportNeuronNTSCable(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	NeuronDetails details;
	CollectNeuronDetails(nrn, details);

	WriteHeader(rOut, details);
	WriteExplicitGeometry(rOut, details);
	WriteGeometryProcedure(rOut, details.numPrimaryNeurites, details.segsPerDend);
	WriteSomaNTSCable(rOut, nrn);
	WriteDendrites(rOut, nrn);
	WriteConnections(rOut, nrn);
	WriteFooter(rOut);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeuronNTSCable(Input& rIn, const ValidationOptions& options)
{
	// soma....
	if( !SeekLineAfterText(rIn, "SOMA COORDINATES AND DIAMETERS:") )
		return kValidationFalse;

	// dendrites....
	if( !SeekLineAfterText(rIn, "NEURITE COORDINATES AND DIAMETERS:") )
		return kValidationFalse;

	// connections....
	if( !SeekLineAfterText(rIn, "CONNECTIONS:") )
		return kValidationFalse;

	return kValidationTrue;
}
