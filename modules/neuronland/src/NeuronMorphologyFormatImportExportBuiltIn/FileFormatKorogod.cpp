//
//
//
#include "ImportExportCommonIncludes.h"

#include <iostream>
#include <sstream>
#include <functional>

#include <math.h>


using namespace std;

/** 

	Korogod format

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

namespace
{
	const String kTagBranchId("branchId");

	bool ReadAndConnectBranch(Input& rIn, MorphologyBuilder& builder, bool soma = false)
	{
		String line = rIn.readLine();

		String comment;
		u32 id;
		u32 numSamples;
		u32 parentId;
		u32 numSegments;
		String name;

		istringstream ss(line, stringstream::in);
		ss >> comment >> id >> numSamples >> parentId >> numSegments >> name;

		if(name != "Branch")
		{
			LogImportFailure(String("Unexpected structure name (expecting Branch): ") + name.c_str());
			return false;
		}

		if(parentId != 0)
		{
			MorphologyBuilder::Branch parentBran = builder.CdFindBranchWithTag(kTagBranchId, parentId);
			const bool unacceptableParent = (parentBran == MorphologyBuilder::NullBranch());

			if(unacceptableParent)
			{
				LogImportFailure("Import assumes parent branch is read before children!");
				return false;
			}

			builder.SetCurrentBranch(parentBran);
			builder.CbNewChildLast(false);
		}

		if(!soma)
			builder.CbSetTag(kTagBranchId, id);

		for(u32 n=0;n<numSamples;++n)
		{
			SamplePoint sample;

			String line = rIn.readLine();
			istringstream ss(line, stringstream::in);
			ss >> sample.x >> sample.y >> sample.z >> sample.d;

			if(soma)
				builder.CsAddSample(sample);
			else
				builder.CbAddSample(sample);
		}

		return true;
	}

	bool ReadSoma(Input& rIn, MorphologyBuilder& builder)
	{
		builder.NewSomaOpenContour();
		ReadAndConnectBranch(rIn, builder, true);

		return true;
	}

	bool ReadTree(Input& rIn, MorphologyBuilder& builder, u32 treeId)
	{
		String line = rIn.readLine();

		String comment;
		u32 id;
		u32 numBranches;
		String name;

		istringstream ss(line, stringstream::in);
		ss >> comment >> id >> numBranches >> name;

		if(name == "Axon")
		{
			builder.NewAxon();
		}
		else if(name == "Dendrite")
		{
			builder.NewDendrite();
		}
		else
		{
			LogImportFailure(String("Unknown tree type: ") + name.c_str());
			return false;
		}

		builder.ClearTags();

		for(u32 n=0;n<numBranches;++n)
			ReadAndConnectBranch(rIn, builder);

		return true;
	}

	bool ReadTrees(Input& rIn, MorphologyBuilder& builder, u32 numTrees)
	{
		for(u32 n=0;n<numTrees;++n)
		{
			bool ok = ReadTree(rIn, builder, n);

			if(!ok)
				return false;
		}

		return true;
	}
}

bool ImportKorogod(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	// comment
	rIn.readLine();

	bool success = true;
	MorphologyBuilder builder(nrn);

	while(success && rIn.remaining())
	{
		String line = rIn.readLine();

		if( StringContains(line, "Soma"))
		{
			String comment;
			u32 id;
			u32 numBranches;

			istringstream ss(line, stringstream::in);
			ss >> comment >> id >> numBranches;

			if(id != 0)
				LogImportWarning("Soma id not 0 (expected)");
			if(numBranches != 1)
				LogImportWarning("Soma branch count not 1 (expected)");

			success = ReadSoma(rIn, builder);
		}
		else if(StringContains(line, "Dendrites"))
		{
			String comment;
			u32 numTrees;
			u32 numBranches;
			u32 numSamples;

			istringstream ss(line, stringstream::in);
			ss >> comment >> numTrees >> numBranches >> numSamples;

			success = ReadTrees(rIn, builder, numTrees);
		}
		else
		{
			LogImportFailure("Unexpected section headers: " + line);
			success = false;
		}
	}

	builder.ApplyChanges();

	nrn.MergeAllUnaryBranchPoints();
	nrn.EnhanceSingleSampleTrees();

	return success;
}

bool ImportRawKorogod(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportKorogod(*pIn, nrn, options);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteKorogodSample(Output& rOut, const SamplePoint& sample)
{
	rOut.writeLine( PrependSpaces(dec(sample.x, 4), 8) + 
					PrependSpaces(dec(sample.y, 4), 9, 1) + 
					PrependSpaces(dec(sample.z, 4), 9, 1) + 
					PrependSpaces(dec(sample.d, 1), 9, 1) + " " );
}

void WriteKorogodBranchData(Output& rOut, u32 currentId, const std::vector<SamplePoint>& samples, u32 parentId, u32 unknown)
{
	rOut.writeLine( String("# ") + dec(currentId) + " " + dec( (u32)samples.size() ) + " " + dec(parentId) + " " + dec(unknown) + " Branch" );
	for(u32 n=0;n<samples.size();++n)
	{
		WriteKorogodSample(rOut, samples[n]);
	}
}

u32 WriteKorogodBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, u32 currentId, u32 parentId)
{
	const u32 numNeuronSegments = (*it).m_samples.size() - 1; // max, for now..

	WriteKorogodBranchData(rOut, currentId, (*it).m_samples, parentId, numNeuronSegments);

	if(it.child())	currentId = WriteKorogodBranch(rOut, it.child(), currentId+1, currentId);
	if(it.peer())	currentId = WriteKorogodBranch(rOut, it.peer(), currentId+1, parentId);

	return currentId;
}

bool ExportKorogod(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	String firstComment = "# neuron reconstruction";
	if(options.addCommentText)
		firstComment += ": " + options.commentText;

	rOut.writeLine(firstComment);

	// Soma
	if(nrn.CountSomas() > 0)
	{
		rOut.writeLine( String("# ") + dec(0) + " " + dec(1) + " Soma" );
		WriteKorogodBranchData(rOut, 1, nrn.SomaBegin()->m_samples, 0, 2);
	}


	// Trees
	const u32 numTrees		= nrn.CountAxons() + nrn.CountDendrites();
	const u32 numBranches	= nrn.CountAxonElements() + nrn.CountDendriteElements();
	const u32 numSamples	= nrn.CountAllAxonSamplePoints() + nrn.CountAllDendriteSamplePoints() + (numBranches-1);

	rOut.writeLine( String("# ") + dec(numTrees) + " " + dec(numBranches) + " " + dec(numSamples) + "  Dendrites" );

	u32 treeCount = 0;
	for(Neuron3D::AxonConstIterator it=nrn.AxonBegin();it != nrn.AxonEnd();++it, ++treeCount)
	{
		const u32 branchCount = (*it).size();
		rOut.writeLine( String("# ") + dec(treeCount) + " " + dec(branchCount) + " Axon" );
		WriteKorogodBranch(rOut, (*it).root(), 1, 0);
	}
	for(Neuron3D::DendriteConstIterator it=nrn.DendriteBegin();it != nrn.DendriteEnd();++it, ++treeCount)
	{
		const u32 branchCount = (*it).size();
		rOut.writeLine( String("# ") + dec(treeCount) + " " + dec(branchCount) + " Dendrite" );
		WriteKorogodBranch(rOut, (*it).root(), 1, 0);
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateKorogod(Input& rIn, const ValidationOptions& options)
{
	// slightly hacky validation for now...
	String line1 = rIn.readLine();
	String line2 = rIn.readLine();
	String line3 = rIn.readLine();

	if(! StringBegins(line1, "#") || !StringBegins(line2, "#") || !StringBegins(line3, "#"))
		return kValidationFalse;

	if(!StringContains(line2, "Soma") && !StringContains(line2, "Dendrite") && !StringContains(line2, "Axon") && !StringContains(line2, "Branch"))
		return kValidationFalse;

	if(!StringContains(line3, "Soma") && !StringContains(line3, "Dendrite") && !StringContains(line3, "Axon") && !StringContains(line3, "Branch"))
		return kValidationFalse;

	return kValidationTrue;
}
