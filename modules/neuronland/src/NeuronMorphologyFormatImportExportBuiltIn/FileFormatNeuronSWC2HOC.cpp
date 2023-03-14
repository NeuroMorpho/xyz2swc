//
//
//

#include "ImportExportCommonIncludes.h"

#include <map>

#include <algorithm>
#include <math.h>

#include "Core/Parser.h"

/** 

	NeuronSWC2HOC generates a file suitable for inclusion in a Neuron Simulator script

	Here we provide functions for loading/saving this style of script.

**/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class NeuronSWC2HOCParser : public SimpleParser
{
public:
	NeuronSWC2HOCParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn)
	{
	}

	//template<class T>
	void ReadSamples(String name, MorphologyBuilder::Branch& sampleContainer)
	{
		String line;
		SamplePoint sample;

		line = rIn.readLine();
		assert( StringBegins(line, name));

		line = rIn.readLine();
		assert(line == "   pt3dclear()");

		while(1)
		{
			line = rIn.readLine();

			if(StringBegins(line, "   pt3dadd("))
			{
				String sampleDataStr = StringBefore( StringAfter(line, "   pt3dadd("), ")");
				Strings values = StringSplit(sampleDataStr, ',');
				StringsClean(values);

				sample.x = std::stof(values[0]);
				sample.y = std::stof(values[1]);
				sample.z = std::stof(values[2]);
				sample.d = std::stof(values[3]);

				sampleContainer.AddSample(sample);
			}
			else if(StringBegins(line, "   }"))
			{
				break;
			}
			else
			{
				// bad
			}
		}
	}

	void ReadSamples(const String& name, MorphologyBuilder::Soma& sampleContainer)
	{
		String line;
		SamplePoint sample;

		line = rIn.readLine();
		assert( StringBegins(line, name));

		line = rIn.readLine();
		assert(line == "   pt3dclear()");

		while(1)
		{
			line = rIn.readLine();

			if(StringBegins(line, "   pt3dadd("))
			{
				String sampleDataStr = StringBefore( StringAfter(line, "   pt3dadd("), ")");
				Strings values = StringSplit( sampleDataStr, ',');
				StringsClean(values);

				sample.x = std::stof(values[0]);
				sample.y = std::stof(values[1]);
				sample.z = std::stof(values[2]);
				sample.d = std::stof(values[3]);

				sampleContainer.AddSample(sample);
			}
			else if(StringBegins(line, "   }"))
			{
				break;
			}
			else
			{
				// bad
			}
		}
	}

	int ReadBranchSampleData(int current, bool axon)
	{
		auto  cb = builder.CurrentBranch();
		ReadSamples( (axon ? "axon[" : "dendrite[") + dec(current) + "]", cb );
		++current;

		report("Read branch samples - " + dec(current));

		if(builder.CurrentBranch().GetChild())
		{
			builder.CbToChild();
			current = ReadBranchSampleData(current, axon);
			builder.CbToParent();
		}

		if(builder.CurrentBranch().GetPeer())
		{
			builder.CbToPeer();
			current = ReadBranchSampleData(current, axon);
			builder.CbToPeerBack();
		}

		return current;
	}


	bool Parse()
	{
		const HString kTagDendriteCount = "count";
		const HString kTagAxonCount = "axcount";

		// create soma
		String line = rIn.readLine();
		if(line != "create soma")
			return false;

		u32 numDendrites = 0;
		u32 numAxons = 0;
		
		// create dendrites and axons
		line = rIn.readLine();
		if(StringBegins( line, "create dendrite["))
		{
			String numDendritesStr = StringBefore( StringAfter(line, "["), "]");
			numDendrites = std::stoi( numDendritesStr );

			line = rIn.readLine();
		}

		if(StringBegins(line, "create axon["))
		{
			String numAxonsStr = StringBefore(StringAfter(line, "["), "]");
			numAxons = std::stoi(numAxonsStr);

			line = rIn.readLine();
		}

		if(numAxons + numDendrites == 0)
			return false;

		// skip access soma
		if(line == "access soma")
			line = rIn.readLine();

		// create soma - empty for the moment
		builder.NewSomaOpenContour();

		// build tree from connection data
		u32 dendriteCount = 0;
		while(rIn.remaining() && StringBegins(line, "connect dendrite"))
		{
			const String pre = StringAfter(line, "connect dendrite[");
			const String post = StringBefore(pre, "](0),");
			const int current = std::stoi(post);
			assert(current == dendriteCount);

			if(StringContains(line, "soma"))
			{
				builder.NewDendrite();
				builder.CbSetTag(kTagDendriteCount, dendriteCount);
			}
			else
			{
				const int parent = std::stoi( StringBefore( StringAfter(line, ", dendrite["), "](1)") );
				assert(parent < current);

				builder.SetCurrentBranch( builder.CdFindBranchWithTag(kTagDendriteCount, parent) );
				builder.CbNewChildLast(false);
				builder.CbSetTag(kTagDendriteCount, current);
			}

			++dendriteCount;

			if(dendriteCount < numDendrites || numAxons > 0)
				line = rIn.readLine();
			else
				break;

			if(builder.HasFailed())
				return false;
		}

		assert(dendriteCount == numDendrites);

		// build tree from connection data (duplicated code from above...ugh..)
		u32 axonCount = 0;
		while(rIn.remaining() && StringBegins(line, "connect axon"))
		{
			const int current = std::stoi( StringBefore( StringAfter(line, "connect axon["), "](0),") );
			assert(current == axonCount);

			if(StringContains(line, "soma"))
			{
				builder.NewAxon();
				builder.CbSetTag(kTagAxonCount, axonCount);
			}
			else
			{
				const int parent = std::stoi( StringBefore( StringAfter(line, ", axon["), "](1)") );
				assert(parent < current);

				builder.SetCurrentBranch( builder.CdFindBranchWithTag(kTagAxonCount, parent) );
				builder.CbNewChildLast(false);
				builder.CbSetTag(kTagAxonCount, current);
			}

			++axonCount;

			if(axonCount < numAxons)
				line = rIn.readLine();
			else
				break;

			if(builder.HasFailed())
				return false;
		}

		assert(axonCount == numAxons);


		// sample data
		auto cs = builder.CurrentSoma();
		ReadSamples("soma", cs);

		int branchcount = 0;
		if(builder.NumDendrites())
		{
			builder.SetCurrentDendrite(builder.DendriteBegin());
			for(u32 n=0;n<builder.NumDendrites();++n)
			{
				assert(branchcount == builder.CbGetTag(kTagDendriteCount) );
				branchcount = ReadBranchSampleData(branchcount, false);
				builder.NextDendrite();
			}
		}
		assert(branchcount == numDendrites);

		branchcount = 0;
		if(builder.NumAxons())
		{
			builder.SetCurrentAxon(builder.AxonBegin());
			for(u32 n=0;n<builder.NumAxons();++n)
			{
				assert(branchcount == builder.CbGetTag(kTagAxonCount) );
				branchcount = ReadBranchSampleData(branchcount, true);
				builder.NextAxon();
			}
		}

		assert(branchcount == numAxons);


		builder.ApplyChanges();
		return true;
	}

private:
	MorphologyBuilder builder;
};

bool ImportNeuronSWC2HOC(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	NeuronSWC2HOCParser parser(rIn, nrn);
	return parser.Parse();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

static int WriteBranchConnection(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int parent, int	branchcount, bool axon)
{
	String connectee;
	if(parent == -1)
		connectee = "soma(" + dec(0.5f, 1) + ")";
	else
		connectee = (axon ? "axon[" : "dendrite[") + dec(parent) + "](1)";

	rOut.writeLine("connect " + String(axon ? "axon[" : "dendrite[") + dec(branchcount) + "](0), " + connectee);

	++branchcount;

	if(it.child()) branchcount = WriteBranchConnection(rOut, it.child(), branchcount-1, branchcount, axon);
	if(it.peer()) branchcount = WriteBranchConnection(rOut, it.peer(), parent, branchcount, axon);

	return branchcount;
}

static void WriteConnections(Output& rOut, const Neuron3D &nrn)
{
	int branchcount = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		branchcount = WriteBranchConnection(rOut, (*d).root(), -1, branchcount, false);
	}

	branchcount = 0;
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		branchcount = WriteBranchConnection(rOut, (*a).root(), -1, branchcount, true);
	}
}

void WriteSamples(Output& rOut, const String& name, const vector<SamplePoint>& samples)
{
	rOut.writeLine(name + " {");
	rOut.writeLine("   pt3dclear()");

	for(u32 n=0;n<samples.size();++n)
	{
		rOut.writeLine( "   pt3dadd(" +	dec(samples[n].x,6) + "," + 
										dec(samples[n].y,6) + "," + 
										dec(samples[n].z,6) + "," + 
										dec(samples[n].d,6) + ")"  );
	}

	rOut.writeLine("   }");
}

int WriteBranchSamples(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int current, bool axon)
{
	WriteSamples(rOut, (axon ? "axon[" : "dendrite[") + dec(current) + "]", (*it).m_samples);

	++current;

	if(it.child()) current = WriteBranchSamples(rOut, it.child(), current, axon);
	if(it.peer()) current = WriteBranchSamples(rOut, it.peer(), current, axon);

	return current;
}

void WriteSamplePoints(Output& rOut, const Neuron3D &nrn)
{
	if(nrn.CountSomas() > 0)
		WriteSamples(rOut, "soma", nrn.SomaBegin()->m_samples);
	else
		WriteSamples(rOut, "soma", std::vector<SamplePoint>());

	int branchcount = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		branchcount = WriteBranchSamples(rOut, (*d).root(), branchcount, false);
	}

	branchcount = 0;
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		branchcount = WriteBranchSamples(rOut, (*a).root(), branchcount, true);
	}
}

bool ExportNeuronSWC2HOC(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	rOut.writeLine("create soma");
	if(nrn.CountDendrites() > 0)
		rOut.writeLine("create dendrite[" + dec(nrn.CountDendriteElements()) + "]");
	if(nrn.CountAxons() > 0)
		rOut.writeLine("create axon[" + dec(nrn.CountAxonElements()) + "]");
	rOut.writeLine("access soma");

	WriteConnections(rOut, nrn);
	WriteSamplePoints(rOut, nrn);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

static char* gs_lineBeginnings[] =
{
	"create soma",
	"create dendrite",
	"create axon",
	"access soma",
	"connect",
	"soma {",
	"   pt3dclear()",
	"   pt3dadd(",
	"   }",
	"dendrite[",
	"axon["
};

static bool IsValidLineBeginning(String line)
{
	for(u32 n=0;n< sizeof(gs_lineBeginnings)/sizeof(char*);++n)
	{
		if(StringBegins(line, gs_lineBeginnings[n] ))
			return true;
	}

	return false;
}

ValidationResult ValidateNeuronSWC2HOC(Input& rIn, const ValidationOptions& options)
{
	u32 lineCount = 0;
	const u32 maxLines = 8;
	String line;

	while(rIn.remaining() > 0 && lineCount < maxLines)
	{
		line = rIn.readLine();
		if( IsValidLineBeginning(line) )
		{
			++lineCount;
		}
		else
			return kValidationFalse;
	}

	return (lineCount > 4 ? kValidationTrue : kValidationFalse);
}
