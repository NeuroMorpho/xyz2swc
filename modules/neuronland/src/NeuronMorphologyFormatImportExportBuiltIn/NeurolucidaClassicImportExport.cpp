#include <float.h>
#include <math.h>
#include <algorithm>

#include "ImportExportCommonIncludes.h"

#include "NeurolucidaMarkers.h"

#include <float.h>


#include "NeurolucidaClassicImportExport.h"

/**

**/

static const u32 kMajorLineTo			= 1;
static const u32 kMajorMove				= 2;
static const u32 kMajorMarker			= 3;
static const u32 kMajorUnknown1			= 6;
static const u32 kMajorUnknown2			= 7;
static const u32 kMajorSectionChange	= 10;
static const u32 kMajorDepth			= 32;
static const u32 kMajorErrorIgnore		= 33;

static const u32 kMinorDendrite			= 1;
static const u32 kMinorAxon				= 21;
static const u32 kMinorSoma				= 41;
static const u32 kMinorApicalDendrite	= 61;

static const u32 kMinorBranchPoint		= 5;
static const u32 kMinorTerminalNormal	= 101;

static const u32 kMinorLineObjectStart	= 111;


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// Importer
//


bool NeurolucidaClassicImporter::Import(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	NCHeaderDetails details;
	if(! ReadHeader(rIn, details) )
	{
		LogImportFailure("Malformed header!");
		return false;
	}

	// build tree
	u32 major;
	u32 minor;
	SamplePoint sample;
	SamplePoint prevSample;

	u32 numBranchPoints = 0;
	u32 numSecondBranches = 0;
	u32 currentSectionNumber;

	MorphologyBuilder builder(nrn);

	bool expectingNewObjectOrEndOfFile = false;
	bool expectingSoma = false;
	bool expectingSecondBranch = false;
	bool expectingTerminal = false;
	//bool expectingWierdness = false;

	String line;

	while(rIn.remaining() > 0)
	{
		prevSample = sample;

		if( !ReadSample(rIn, major, minor, sample) )
		{
			LogImportFailure("Malformed sample!");
			return false;
		}


		// we ignore any line objects 
		if(minor >= kMinorLineObjectStart)
		{
			// n262.asc - in case terminal point is not properly finished!
			//expectingNewObjectOrEndOfFile = true;
			continue;
		}

		switch(major)
		{
		// trees
		case kMajorLineTo:
			// continuation of current section
			if(minor == kMinorSoma + 1 || minor == kMinorSoma)
			{
				// looks like soma data, but wasn't started with a section change
				if(builder.NumSomas() == 0)
					builder.NewSomaOpenContour();

				builder.CsAddSample(sample);
			}
			else
			{
				builder.CbAddSample(sample);

				if(	minor == currentSectionNumber+1 ||
					minor == currentSectionNumber)
				{
					if(expectingTerminal)
					{
						expectingTerminal = false;

						// point after this should be start of a second branch, or new dendrite, or nothing
						if(numSecondBranches < numBranchPoints)
						{
							expectingSecondBranch = true;
						}
						else
						{
							expectingNewObjectOrEndOfFile = true;
						}
					}
				}
				/*else if(minor == currentSectionNumber)
				{
					// stop tree now ?
					expectingNewObjectOrEndOfFile = true;
				}*/
				else
				{
					assert(0);
					LogImportWarning("Unexpected minor number");
				}
			}
			break;

		case kMajorMove:

			if(expectingSecondBranch)
			{
				expectingSecondBranch = false;
				if(builder.CbAscendToUnaryBranchPoint())
				{
					builder.CbNewChildLast(false);
					builder.CbAddSample(sample);
				}
				++numSecondBranches;
			}
			else if(expectingSoma)
			{
				expectingSoma = false;
				builder.NewSomaOpenContour();
				builder.CsAddSample(sample);

				assert(minor == kMinorSoma);
				currentSectionNumber = minor;
			}
			else if(expectingNewObjectOrEndOfFile)
			{
				expectingNewObjectOrEndOfFile = false;

				currentSectionNumber = minor;

				if(minor == kMinorDendrite || minor == kMinorApicalDendrite)
				{
					builder.NewDendriteWithSample(sample);

					//
					if(minor == kMinorApicalDendrite)
						builder.CdSetApical();
					//
				}
				else if(minor == kMinorAxon)
				{
					builder.NewAxonWithSample(sample);
				}
			}
			else
			{
				//assert(0);
			}
			break;

		case kMajorSectionChange:

			if(expectingTerminal)
			{
				// terminal will be over-ridden by subsequent point.
				expectingTerminal = false;
			}

			if(expectingSecondBranch)
			{
				expectingSecondBranch = false;
				LogImportWarning("Expecting second branch, but found new object instead");
			}

			// branch point
			if(minor == kMinorBranchPoint)
			{
				if(expectingNewObjectOrEndOfFile)
					break;

				// complete current branch
				builder.CbAddSample(sample);

				// start the next branch
				builder.CbNewChildLast(false);

				++numBranchPoints;
			}
			// dendrite number
			else if(minor == kMinorSoma)
			{
				expectingSoma = true;
			}
			else if(minor == kMinorDendrite || minor == kMinorApicalDendrite || minor == kMinorAxon)
			{
				expectingNewObjectOrEndOfFile = true;
			}
			else if(minor >= kMinorTerminalNormal && minor < kMinorLineObjectStart)
			{
				expectingTerminal = true;
			}
			else
			{
				LogImportWarning("Unknown minor: " + dec(minor));
			}
			break;

		// markers
		case kMajorMarker:
			{
				MarkerPoint marker;
				marker.x = sample.x;
				marker.y = sample.y;
				marker.z = sample.z;
				marker.d = sample.d;

				// give marker default name associated with shape, i.e.  "Marker X". nothing else we can do.
				marker.shape = minor;
				String markerName = NeurolucidaMarkerShape2DefaultName(minor);

				if(currentSectionNumber == kMinorSoma)
					builder.CsAddMarker( markerName, marker );
				else if(builder.CurrentBranch() && 
						(currentSectionNumber == kMinorDendrite || 
						currentSectionNumber == kMinorAxon || 
						currentSectionNumber == kMinorApicalDendrite) )
				{
					builder.CbAddMarker( markerName, marker );
				}
				else
					builder.AddMarker( markerName, marker );
			}
			break;

		// ignore
		case kMajorDepth:
		case kMajorErrorIgnore:
			break;

		default:
			LogImportWarning("Ignoring unknown major: " + dec(major));
			break;
		}

		if(builder.HasFailed())
		{
			LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
			return false;
		}
	}

	if(!expectingSecondBranch)
	{
		assert(expectingNewObjectOrEndOfFile || (major == kMajorMarker) || (major == kMajorDepth) || (major == kMajorUnknown1) || (major == kMajorUnknown2) /*|| expectingWierdness*/);
		assert(numSecondBranches == numBranchPoints);
	}
	else
	{
		LogImportWarning("Terminated without finding second branch\n");
	}

	builder.ApplyChanges();

	nrn.MergeAllUnaryBranchPoints();
	nrn.EnhanceSingleSampleTrees();

	return true;
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// Exporter
//

struct NeuronExtentsAccumulator
{
	v4f extmax;
	v4f extmin;

	NeuronExtentsAccumulator() : extmax(v4f::Zero()), extmin(v4f(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX))
	{
	}

	template<typename T>
	void operator() (T it)
	{
		for( u32 n=0;n<(*it).m_samples.size();++n)
		{
			const v4f v = (*it).m_samples[n].tov4f();
			extmax.max(v);
			extmin.min(v);
		}

		for(Branch3D::Markers::const_iterator mit = (*it).m_markers.begin();mit != (*it).m_markers.end();++mit)
		{
			for( u32 n=0;n<(*mit).second.size();++n)
			{
				const v4f v = (*mit).second[n].tov4f();
				extmax.max(v);
				extmin.min(v);
			}
		}
	}
};

static void CollectHeaderDetails(const Neuron3D &nrn, NCHeaderDetails& details, const StyleOptions& options)
{
	// original filename
	details.fileName = options.fileName;

	// calculate the extents
	NeuronExtentsAccumulator extents;
	extents(&nrn.m_globalData);
	nrn.DoForAllTreeElements(extents);
	nrn.DoForAllSomas(extents);

	details.max[0] = extents.extmax.GetX();
	details.max[1] = extents.extmax.GetY();
	details.max[2] = extents.extmax.GetZ();
	details.min[0] = extents.extmin.GetX();
	details.min[1] = extents.extmin.GetY();
	details.min[2] = extents.extmin.GetZ();

	// remaining data is simple
	details.fileType = 0;
	details.id1 = 150;
	details.id2 = 0;
	details.ref[0] = 0.0f;
	details.ref[1] = 0.0f;
	details.ref[2] = 0.0f;
	details.rotation = 0.0f;
	details.gx_alpha = 0;
	details.gx_beta = 0;
	details.gx_gamma = 0;
	//
}

void NeurolucidaClassicExporter::WriteSomas(Output& rOut, const Neuron3D &nrn)
{
	for(Neuron3D::SomaConstIterator sit = nrn.SomaBegin(); sit != nrn.SomaEnd();++sit)
	{
		const SamplePoint start = sit->m_samples[0];

		WriteSample(rOut, kMajorSectionChange, kMinorSoma, start);
		WriteSample(rOut, kMajorMove, kMinorSoma, start);

		for(u32 n=1;n<sit->m_samples.size();++n)
		{
			WriteSample(rOut, kMajorLineTo, kMinorSoma + 1, sit->m_samples[n]);
		}

		//if(sit->somaType == Soma3D::kContourClosed)
		//	WriteSample(rOut, kMajorLineTo, kMinorSoma, start);
	}
}

void NeurolucidaClassicExporter::WriteBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, u32 dendriteId)
{
	if( it.parent() && it.parent().child() != it)
	{
		WriteSample(rOut, kMajorMove, dendriteId+1, (*it).m_samples[0]);
	}

	for(u32 n=1;n<(*it).m_samples.size()-1;++n)
	{
		WriteSample(rOut, kMajorLineTo, dendriteId+1, (*it).m_samples[n]);
	}

	// marker data
	u32 markerId = 1;
	for(Branch3D::Markers::const_iterator mit = (*it).m_markers.begin();mit != (*it).m_markers.end();++mit, ++markerId)
	{
		//u32 markerId = NeurolucidaMarkerName2Id(mit->first);
		for(u32 m=0;m<mit->second.size();++m)
			WriteSample(rOut, kMajorMarker, markerId, mit->second[m]);
	}

	//
	if(it.child())
	{
		// branch
		if(it.child().peer())
		{
			WriteSample(rOut, kMajorSectionChange, kMinorBranchPoint, (*it).m_samples[(*it).m_samples.size()-1]);
			WriteSample(rOut, kMajorLineTo, dendriteId+1, (*it.child()).m_samples[0]);
		}
		// continuation
		else
		{
			WriteSample(rOut, kMajorLineTo, dendriteId+1, (*it).m_samples[(*it).m_samples.size()-1]);
		}
	}
	// termination
	else
	{
		WriteSample(rOut, kMajorSectionChange, kMinorTerminalNormal, (*it).m_samples[(*it).m_samples.size()-1]);
		WriteSample(rOut, kMajorLineTo, dendriteId+1, (*it).m_samples[(*it).m_samples.size()-1]);
	}

	if(it.child()) 
		WriteBranch(rOut, it.child(), dendriteId);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it)) 
		WriteBranch(rOut, it.peer(), dendriteId);
}

void NeurolucidaClassicExporter::WriteDendrites(Output& rOut, const Neuron3D &nrn)
{
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		const u32 dendriteMinor =  ((*d).GetTreeType() == Neuron3D::DendriteTree::Apical) ? kMinorApicalDendrite : kMinorDendrite;
		const SamplePoint rootPoint = (*(*d).root()).m_samples[0];
		WriteSample(rOut, kMajorSectionChange, dendriteMinor, rootPoint);
		WriteSample(rOut, kMajorMove, dendriteMinor, rootPoint);
		WriteBranch(rOut, (*d).root(), dendriteMinor);
	}
}

void NeurolucidaClassicExporter::WriteAxons(Output& rOut, const Neuron3D &nrn)
{
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		const SamplePoint rootPoint = (*(*a).root()).m_samples[0];
		WriteSample(rOut, kMajorSectionChange, kMinorAxon, rootPoint);
		WriteSample(rOut, kMajorMove, kMinorAxon, rootPoint);
		WriteBranch(rOut, (*a).root(), kMinorAxon);
	}
}

bool NeurolucidaClassicExporter::Export(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	NCHeaderDetails details;

	CollectHeaderDetails(nrn, details, options);

	WriteHeader(rOut, details);
	WriteSomas(rOut, nrn);
	WriteDendrites(rOut, nrn);
	WriteAxons(rOut, nrn);

	return true;
}
