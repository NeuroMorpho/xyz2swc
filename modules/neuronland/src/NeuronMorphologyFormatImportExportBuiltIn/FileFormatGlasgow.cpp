//
// FileFormatGlasgow.cpp
//

#include "ImportExportCommonIncludes.h"

#include <sstream>


/**

	Glasgow Format

	Structure supported ->
		Soma		: MULTIPLE OPEN CONTOUR
		Dendrite	: YES
		Axon		: YES
		Marker		: YES, PER SOMA, PER BRANCH
		Spine		: NO

	Note: implicit connectivity can be ambiguous..

	Pending:
	* add additional data validation

**/

enum GlasgowElementTag
{
	kCellbody,
	kBranch,
	kAxonBranch,
	kMarker
};

const String kSomaName("cellbody");
const String kDendBranchName("branch");
const String kAxonBranchName("axonbranch");
const String kMarkerName("marker");


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

typedef std::vector<SamplePoint> SamplePointList;

struct BranchData
{
	GlasgowElementTag	type;
	SamplePointList		samples;
	Branch3D::Markers	markers;
	bool				done;
};


template<typename T>
bool ReadSamples(Input& rIn, std::vector<T>& samples, u32 number)
{
	String line = rIn.readLine();
	
	if(line != "")
	{
		LogImportFailure("Expecting empty line!");
		return false;
	}

	T sample;

	for(u32 n=0;n<number;++n)
	{
		if(rIn.remaining() == 0)
		{
			LogImportFailure("File terminates prematurely.");
			return false;
		}

		line = rIn.readLine();
		std::istringstream ss(line, std::stringstream::in);
		//std::istringstream ss = std::istringstream((const char*)line, std::stringstream::in); // mac doesnt like..
		ss >> sample.x >> sample.y >> sample.z >> sample.d;
		samples.push_back(sample);
	}

	return true;
}

template<typename T>
void CopyBranchDataToBranch(T branch, const BranchData data)
{
	for(u32 n=0;n<data.samples.size();++n)
		branch.AddSample(data.samples[n]);

	for(Branch3D::Markers::const_iterator it=data.markers.begin();it!=data.markers.end();++it)
	{
		String markerName = it->first;
		for(u32 n=0;n<it->second.size();++n)
			branch.AddMarker(markerName.c_str(), it->second[n]);
	}
}

bool BuildTree(MorphologyBuilder::Branch parent, std::vector<BranchData> &data)
{
	SamplePoint connectionPoint = parent.GetLastSample();

	for(u32 n=0;n<data.size();++n)
	{
		if(data[n].done)
			continue;

		if( data[n].samples[0].Distance(connectionPoint) < 0.01f)
		{
			MorphologyBuilder::Branch newBranch = parent.NewChildLast(false);
			CopyBranchDataToBranch(newBranch, data[n]);
			data[n].done = true;
		}
	}

	if(parent.GetChild())
		BuildTree(parent.GetChild(), data);

	if(parent.GetPeer())
		BuildTree(parent.GetPeer(), data);

	return true;
}

bool ImportGlasgow(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	MorphologyBuilder builder(nrn);
	String line;

	std::vector<BranchData> branchSampleData;
	std::vector<BranchData>::iterator currentBranchIt = branchSampleData.end();

	// stage 1 - read all the sample data 
	while(rIn.remaining())
	{
		BranchData branchData;
		GlasgowElementTag	m_elementType;
		int numSamples;
		int markerId;

		line = rIn.readLine();

		if(line == "")
			continue;

		numSamples = std::stoi( StringAfterLast(line, " ") );

		if(StringContains(line, kSomaName))
		{
			m_elementType = kCellbody;

			BranchData somaSamples;

			if(!ReadSamples(rIn, somaSamples.samples, numSamples))
				return false;

			builder.NewSomaOpenContour();
			CopyBranchDataToBranch(builder.CurrentSoma(), somaSamples);
		}
		else if(StringContains(line, kAxonBranchName))
		{
			m_elementType = kAxonBranch;
			if(!ReadSamples(rIn, branchData.samples, numSamples))
				return false;

			branchData.done = false;
			branchData.type = kAxonBranch;
			currentBranchIt = branchSampleData.insert(branchSampleData.end(), branchData);
		}
		else if(StringContains(line, kDendBranchName))
		{
			m_elementType = kBranch;
			if(!ReadSamples(rIn, branchData.samples, numSamples))
				return false;

			branchData.done = false;
			branchData.type = kBranch;
			currentBranchIt = branchSampleData.insert(branchSampleData.end(), branchData);
		}
		else if(StringContains(line, kMarkerName))
		{
			markerId = std::stoi( StringAfterLast( StringAfter(line, kMarkerName), " ") );

			String markerName = "Marker " + dec(markerId);

			if(!ReadSamples(rIn, branchData.markers[markerName], numSamples))
				return false;

			if(m_elementType == kCellbody)
			{
				// add directly to current soma contour
				for(Branch3D::Markers::iterator it = branchData.markers.begin(); it != branchData.markers.end();++it)
					for(Branch3D::MarkerSet::iterator it2 = it->second.begin(); it2 != it->second.end();++it2)
						builder.CsAddMarker(it->first, *it2);
			}
			else
			{
				for(Branch3D::Markers::iterator it = branchData.markers.begin(); it != branchData.markers.end();++it)
					for(Branch3D::MarkerSet::iterator it2 = it->second.begin(); it2 != it->second.end();++it2)
						currentBranchIt->markers[markerName].push_back(*it2);
			}
		}
		else
		{
			LogImportFailure("Unexpected structure name: " + line);
			return false;
		}
	}

	// stage 2 - now we can connect up all the branches - first find all the root branches
	u32 branchesRemoved = 0;
	for(u32 n=0;n<branchSampleData.size();++n)
	{
		bool connected = false;

		// assume connected segments are before (helps to avoid problems with duplicate sample point positions)
		for(u32 m=0;m<n;++m)
		{
			//if(m != n)
			{
				if( branchSampleData[n].samples[0].Distance(branchSampleData[m].samples[branchSampleData[m].samples.size()-1]) < 0.01f)
				{
					connected = true;
					break;
				}
			}
		}

		if(!connected)
		{
			if(branchSampleData[n].type == kAxonBranch)
				builder.NewAxon();
			else
				builder.NewDendrite();

			CopyBranchDataToBranch(builder.CurrentBranch(), branchSampleData[n]);
			branchSampleData[n].done = true;
		}
	}

	// stage 3 - now connect up all the rest of the branches, starting with the root of each dendrite
	if(builder.NumDendrites() > 0)
	{
		builder.SetCurrentDendrite(builder.DendriteBegin());
		for(;builder.CurrentDendrite() != builder.DendriteEnd();builder.NextDendrite())
		{
			BuildTree(builder.CurrentBranch(), branchSampleData);
		}
	}

	if(builder.NumAxons() > 0)
	{
		builder.SetCurrentAxon(builder.AxonBegin());
		for(;builder.CurrentAxon() != builder.AxonEnd();builder.NextAxon())
		{
			BuildTree(builder.CurrentBranch(), branchSampleData);
		}
	}

	for(u32 n=0;n<branchSampleData.size();++n)
	{
		if(!branchSampleData[n].done)
		{
			LogImportWarning("Branch data could be integrated into neuron: branch #" + dec(n));
		}
	}

	if(builder.HasFailed())
	{
		LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
		return false;
	}
	
	//
	builder.ApplyChanges();
	return true;
}


bool ImportRawGlasgow(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportGlasgow(*pIn, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

// temporary global (no good for re-entrancy...)
static std::map<String, u32> g_markerNameIndex;

u32 MarkerNameToIndex(const String& name)
{
	std::map<String, u32>::iterator it = g_markerNameIndex.find(name);
	if(it != g_markerNameIndex.end())
	{
		return it->second;
	}
	else
	{
		const u32 index = g_markerNameIndex.size();
		g_markerNameIndex[name] = index;
		return index;
	}
}

template<typename T>
void WriteSamples(Output& rOut, const String& name, const std::vector<T>& samples, int type = -1)
{
	rOut.writeLine("");
	rOut.writeLine("  " + name + " " + dec((u32)samples.size()) + (type != -1 ? dec(type) : String("")) );
	rOut.writeLine("");
	for(u32 n=0;n<samples.size();++n)
	{
		rOut.writeLine( PrependSpaces(dec(samples[n].x,2), 9) +
						PrependSpaces(dec(samples[n].y,2), 9) +
						PrependSpaces(dec(samples[n].z,2), 9) +
						PrependSpaces(dec(samples[n].d,2), 9) );
	}
}

static void WriteMarkers(Output& rOut, const Branch3D::Markers& markers)
{
	for(Branch3D::Markers::const_iterator mit=markers.begin();mit!=markers.end();++mit)
	{
		const u32 markerId = MarkerNameToIndex(mit->first);
		WriteSamples(rOut, kMarkerName, (*mit).second, markerId);
	}
}

static void WriteSoma(Output& rOut, Neuron3D::SomaConstIterator it)
{
	WriteSamples(rOut, kSomaName, (*it).m_samples);
	WriteMarkers(rOut, (*it).m_markers);
}

static void WriteBranch(Output& rOut, const String& name, Neuron3D::DendriteTreeConstIterator it)
{
	WriteSamples(rOut, name, (*it).m_samples, -1);
	WriteMarkers(rOut, (*it).m_markers);

	if(it.child()) WriteBranch(rOut, name, it.child());
	if(it.peer()) WriteBranch(rOut, name, it.peer());
}

bool ExportGlasgow(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	g_markerNameIndex.clear();

	// soma
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s!=nrn.SomaEnd(); ++s)
		WriteSoma(rOut, s);

	// dendrites
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		WriteBranch(rOut, kDendBranchName, (*d).root());

	// axons
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
		WriteBranch(rOut, kAxonBranchName, (*a).root());

	g_markerNameIndex.clear();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateGlasgow(Input& rIn, const ValidationOptions& options)
{
	String line1 = rIn.readLine();
	String line2 = rIn.readLine();

	if(line1 == "" && ( StringContains(line2, kSomaName) || StringContains(line2, kAxonBranchName) || StringContains(line2, kDendBranchName) ) )
		return kValidationTrue;

	return kValidationFalse;
}
