//
//
//

#include "ImportExportCommonIncludes.h"

#include <float.h>

#include <iostream>
#include <sstream>
#include <functional>

#include <math.h>

#include "Core/Parser.h"


/** 

NeuronJ format

**/

static String njHeader("// NeuronJ Data File - DO NOT CHANGE");
static String njParameters("// Parameters");
static String njTypes("// Type names and colors");
static String njClusters("// Cluster names");
static String njTracing("// Tracing ");
static String njSegment("// Segment ");
static String njOfTracing(" of Tracing ");
static String njFooter("// End of NeuronJ Data File");


static bool IsNeuronJVersionSupported(String version)
{
	return (version == "1.4.0" || StringBegins(version, "1.0"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

static const u32 closeRootThreshold = 5;
static const u32 parentChildThreshold = 7;

static const HString kTagBranchIndex("branchIndex");

typedef std::vector<SamplePoint> SampleList;
typedef std::vector<SampleList>  Tracings;


struct ConnectionData
{
	u32 originalBranchIndex;
	u32 connectionBranchIndex;
	u32 connectionSampleIndex;

	ConnectionData() {}
	ConnectionData(u32 ob, u32 cb, u32 cs) : originalBranchIndex(ob), connectionBranchIndex(cb), connectionSampleIndex(cs) {}

	bool operator<(const ConnectionData& rhs) const
	{
		return	(connectionBranchIndex < rhs.connectionBranchIndex) ||
				(connectionBranchIndex == rhs.connectionBranchIndex && connectionSampleIndex > rhs.connectionSampleIndex) || // note '>' because we want reverse order connection samples...
				(connectionBranchIndex == rhs.connectionBranchIndex && connectionSampleIndex == rhs.connectionSampleIndex && originalBranchIndex < rhs.originalBranchIndex); // just a unique id in order to distinguish two tracings connected to the same point
	}
};

typedef std::map<u32, ConnectionData> ConnectionMap;

class NeuronJParser : public SimpleParser
{
	MorphologyBuilder	builder;

	Neuron3D&			m_nrn;

	Tracings			m_branchTracings;
	ConnectionMap		m_connections;

public:
	NeuronJParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn), m_nrn(nrn)
	{
	}

	void CopyBranchDataToBranch(MorphologyBuilder::Branch branch, SampleList& samples)
	{
		for(u32 n=0;n<samples.size();++n)
			branch.AddSample(samples[n]);
	}

	void BuildTree(MorphologyBuilder::Branch branch)
	{
		const u32 parentBranchIndex = builder.GetTagForBranch(branch, kTagBranchIndex);

		std::vector<ConnectionData> allConnections;
		std::vector<MorphologyBuilder::Branch> newBranches;

		// find all branches which connect to this one.
		for(ConnectionMap::iterator it = m_connections.begin();it != m_connections.end();++it)
		{
			if(	parentBranchIndex == (*it).second.connectionBranchIndex)
				allConnections.push_back( (*it).second );
		}

		// sort list of connections so they attached in order from most distant to nearest to root
		sort(allConnections.begin(), allConnections.end()/*, ConnectOrderSorter()*/ ); // use user-implemented < instead of functor.. 

		// now create all children and split the branch where appropriate
		builder.SetCurrentBranch(branch);
		for(std::vector<ConnectionData>::iterator it = allConnections.begin();it != allConnections.end();++it)
		{
			const u32 parentSampleCount = branch.NumSamples();
			const u32 originalBranchIndex = (*it).originalBranchIndex;
			const u32 connectionSampleIndex = (*it).connectionSampleIndex;

			if(connectionSampleIndex < parentSampleCount-1)
			{
				builder.CbSplit(connectionSampleIndex);
				//builder.CbSetTag(kTagBranchIndex, connectionBranchIndex); // no need for tag
				builder.CbToParent();
			}

			builder.CbNewChildLast(false);
			builder.CbSetTag(kTagBranchIndex, originalBranchIndex);
			CopyBranchDataToBranch(builder.CurrentBranch(), m_branchTracings[originalBranchIndex]);
			newBranches.push_back( builder.CurrentBranch() );
			builder.CbToParent();
			//m_connections.erase(it); // is safe ?
		}

		// now actually build the trees
		for(std::vector<MorphologyBuilder::Branch>::iterator it = newBranches.begin();it!=newBranches.end();++it)
		{
			BuildTree(*it);
		}
	}

	bool Parse()
	{
		String line;

		// First read in all the individual tracings...
		int tracingId = -1;
		line = rIn.readLine();

		do
		{
			if(StringBegins(line, njTypes))
			{
				// get names - if only as hints for structure type (axon, dendrite ...)
			}
			else if(StringBegins(line, njTracing))
			{
				tracingId = std::stoi( StringAfter(line, njTracing) );
				m_branchTracings.push_back(SampleList());
			}
			else if(StringBegins(line, njSegment))
			{
				String segmentName = StringBefore( StringAfter(line, njSegment), njOfTracing);
				const int latestTracingId = std::stoi( StringAfter(line, njOfTracing) );
				assert(latestTracingId == tracingId);

				// read segment samples into current tracing.
				String line1 = rIn.readLine();

				bool firstSampleOfSegment = true;
				while(!StringBegins(line1, "//"))
				{
					String line2 = rIn.readLine();

					int xpos = std::stoi(line1);
					int ypos = std::stoi(line2);

					if(	(!firstSampleOfSegment) ||
						m_branchTracings.back().size() == 0 ||
						m_branchTracings.back().back().x != xpos ||
						m_branchTracings.back().back().y != ypos)
					{
						m_branchTracings.back().push_back( SamplePoint( xpos, ypos, 0.0f, 1.0f) );
						firstSampleOfSegment = false;
					}
					line1 = rIn.readLine();

					if(!rIn.remaining())
						break;
				}

				line = line1;
				continue;
			}

			line = rIn.readLine();
		}
		while(rIn.remaining() > 0);

		// Now connect up all the tracings as effectively as possible..

		// first locate all branches which do not have a parent, and create connection database for the rest..
		for(u32 n=0;n<m_branchTracings.size();++n)
		{
			const SamplePoint& branchRoot = m_branchTracings[n][0];

			bool directConnection = false;
			float nearestDistance = FLT_MAX;
			u32 nearestConnectionBranch = 0;
			u32 nearestConnectionSample = 0;


			for(u32 m=0;m<m_branchTracings.size();++m)
			{
				const u32 lastSampleIndex = m_branchTracings[m].size()-1;

				const bool selfCheck = (m==n);
				const bool closeRoots = branchRoot.Distance(m_branchTracings[m][0]) <= closeRootThreshold;

				if( !(selfCheck || closeRoots) )
				{
					for(u32 a=lastSampleIndex;a>0;--a) // don't check root sample of test branch - we want a true parent, not a peer...don
					{
						// hack so we don't connect to the last but one sample...
						if(a==lastSampleIndex-1)
							continue;

						const float distance = branchRoot.Distance(m_branchTracings[m][a]);

						if (distance < nearestDistance)
						{
							nearestDistance = distance;
							nearestConnectionBranch = m;
							nearestConnectionSample = a;

							if(distance == 0)
							{
								directConnection = true;
								break;
							}
						}
					}
				}

				if(directConnection)
					break;
			}

			if(directConnection || nearestDistance < parentChildThreshold)
			{
				m_connections[n] = ConnectionData(n, nearestConnectionBranch, nearestConnectionSample);
			}
			else
			{
				builder.NewDendrite();
				CopyBranchDataToBranch(builder.CurrentBranch(), m_branchTracings[n]);

				builder.CbSetTag(kTagBranchIndex, n);
			}
		}

		// use connection database to build rest of each tree
		if(builder.NumDendrites() > 0)
		{
			builder.SetCurrentDendrite(builder.DendriteBegin());
			for(;builder.CurrentDendrite() != builder.DendriteEnd();builder.NextDendrite())
			{
				BuildTree(builder.CurrentBranch());
			}
		}

		//  all branches should be connected now - check that it is the case...

		if(builder.HasFailed())
		{
			LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
			return false;
		}

		builder.ApplyChanges();

		m_nrn.MergeAllUnaryBranchPoints();
		//m_nrn.EnhanceSingleSampleTrees();

		return true;
	}
};

bool ImportNeuronJ(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	NeuronJParser parser(rIn, nrn);
	return parser.Parse();
}

bool ImportRawNeuronJ(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeuronJ(*pIn, nrn, options);
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

bool ExportNeuronJ(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// no intention of implementing an exporter
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeuronJ(Input& rIn, const ValidationOptions& options)
{
	String line1 = rIn.readLine();
	String line2 = rIn.readLine();

	if(line1 == njHeader /*&& IsNeuronJVersionSupported(line2)*/ )
		return kValidationTrue;

	return kValidationFalse;
}
