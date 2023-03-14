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

	SWC format


	CNIC: http://research.mssm.edu/cnic/swc.html
	VNED: 


	ESWC: http://students.cs.tamu.edu/bbusse/645Project.html

**/


/*

CNIC:

	SWC File Format

	The format of an SWC file is fairly simple. It is a text file consisting of a header with various fields beginning with a # character, and a series of three dimensional points containing an index, radius, type, and connectivity information. The lines in the text file representing points have the following layout.
	n T x y z R P

	n is an integer label that identifies the current point and increments by one from one line to the next.

	T is an integer representing the type of neuronal segment, such as soma, axon, apical dendrite, etc. The standard accepted integer values are given below.

	* 0 = undefined
	* 1 = soma
	* 2 = axon
	* 3 = dendrite
	* 4 = apical dendrite
	* 5 = fork point
	* 6 = end point
	* 7 = custom

	x, y, z gives the cartesian coordinates of each node.

	R is the radius at that node.
	P indicates the parent (the integer label) of the current point or -1 to indicate an origin (soma). 


VNED:

Gulyas:

*/

enum HintStyle
{
	kHintUnknown,
	kHintStandard,
	kHintCNIC,
	kHintGulyas
};

enum SampleLocation
{
	// standard tags
	kLocationUndefined		= 0,
	kLocationSoma			= 1,
	kLocationAxon			= 2,
	kLocationDendriteBasal	= 3,
	kLocationDendriteApical	= 4,

	// standard
	kLocationCustomA = 5,
	kLocationCustomB = 6,
	kLocationCustomC = 7,
	kLocationCustomD = 8,
	kLocationCustomE = 9,

	// cnic tags (also uses 1 for soma)
	kLocationCnicBranch = 5,
	kLocationCnicTerminal = 6,

	// duke-southampton tags
	kLocationRoot = -1,

	// vned tags
	kLocation10 = 10,


	// other tags will be gulyas or unknown !
};

bool IsStandardCompatibleLocationId(SampleLocation location)
{
	return	location == kLocationUndefined ||
			location == kLocationSoma ||
			location == kLocationAxon ||
			location == kLocationDendriteBasal ||
			location == kLocationDendriteApical;
}

bool IsCNICCompatibleLocationId(SampleLocation location)
{
	return	IsStandardCompatibleLocationId(location) ||
			location == kLocationCnicBranch ||
			location == kLocationCnicTerminal;
}

bool IsVNEDCompatileLocationId(SampleLocation location)
{
	return IsStandardCompatibleLocationId(location) /*|| 
			location == */;
}

bool IsSomaLocationId(SampleLocation location)
{
	return	location == kLocationSoma ||
			location == kLocationRoot ||
			location == kLocation10;
}

bool IsTreeLocationId(SampleLocation location)
{
	return	location == kLocationDendriteApical ||
			location == kLocationDendriteBasal ||
			location == kLocationAxon;
}

bool LocationTypesMayBeAttached(HintStyle style, SampleLocation childLocation, SampleLocation parentLocation)
{
	if(style == kHintGulyas)
		return true;
	else
		return	(childLocation == parentLocation) ||
				(IsSomaLocationId(parentLocation) && IsTreeLocationId(childLocation)) ||
				(IsTreeLocationId(parentLocation) && IsTreeLocationId(childLocation)) ;
}

bool IsCompatibleLocationId(HintStyle style, SampleLocation location)
{
	switch(style)
	{
	case kHintStandard:
		return IsStandardCompatibleLocationId(location);
	case kHintCNIC:
		return IsCNICCompatibleLocationId(location);
	default:
	case kHintGulyas:
		return true;
	}
}

SampleLocation StructureNumberToLocation(HintStyle hintStyle, SampleLocation location)
{
	switch(hintStyle)
	{
	case kHintGulyas:
		return kLocationDendriteApical;
	default:
		return location;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class SWCNeuronParser : public SimpleParser
{
	MorphologyBuilder builder;
	Neuron3D& m_nrn;

	const HintOptions& mOptions;

public:
	SWCNeuronParser(Input& rIn, Neuron3D &nrn, const HintOptions& options) : SimpleParser(rIn), builder(nrn), m_nrn(nrn), mOptions(options)
	{
	}

	struct branchinfo
	{
		int startnode;
		int nodes;
	};

	bool Parse()
	{
		//
		HintStyle hintStyle;

		switch(mOptions.hintStyle)
		{
			case kStyleSwc_Std:
				hintStyle = kHintStandard;
				break;
			case kStyleSwc_CNIC:
				hintStyle = kHintCNIC;
				break;
			case kStyleSwc_Gulyas:
				hintStyle = kHintGulyas;
				break;
			default:
				hintStyle = kHintUnknown;
				break;
		}

		bool cannotBeStandard	= false;
		bool cannotBeCNIC		= false;
		bool cannotBeGulyas		= false;
		//

		String line;
		SamplePoint dataPoint;
		int segment;
		int location;
		int parent;

		const String kTagSampleNumber	= "samplenum";
		const String kTagSampleType		= "sampletype";

		u32 previousLocation = kLocationSoma;

		std::map<int, int> somaSampleSegments;

		while( rIn.remaining() > 0 )
		{
			line = rIn.readLine();

			if(StringBegins(line, "# SEGMENT_TYPE"))
			{
				// extra gulyas check
				hintStyle = kHintGulyas;
			}

			if( StringBegins(line, "#") ||	// ignore comments
				StringBegins(line, "*") ||
				StringTrim(line) == "")
				continue;

			if(line.length() < 10)
			{
				LogImportWarning("Terminating early due to short non-whitespace line - not long enough to contain all data for a sample point.");
				break;
			}

			// Get rid of commas in floating point representation (>= 1,000.00) - only appears in a small number of files...
			StringRemove(line, ',');
			//

			istringstream ss(line, stringstream::in);
			ss >> segment >> location >> dataPoint.x >> dataPoint.y >> dataPoint.z >> dataPoint.d >> parent;

			// check if we are consistent with any format hint...
			if(	hintStyle != kHintUnknown && !IsCompatibleLocationId(hintStyle, (SampleLocation) location) )
			{
				switch(hintStyle)
				{
				case kHintStandard:
					LogImportWarning( String("Not pure Standard SWC: resetting import, and making less assumptions") );
					cannotBeStandard = true;
					break;
				case kHintCNIC:
					LogImportWarning( String("Not pure CNIC SWC: resetting import, and making less assumptions") );
					cannotBeCNIC = true;
					break;
				case kHintGulyas:
					LogImportWarning( String("Not pure Gulyas SWC: resetting import, and making less assumptions") );
					cannotBeGulyas = true;
					break;
				}

				hintStyle = kHintUnknown;

				// clear neuron, and start again !
				builder.DeleteAll();
				rIn.seek(0, IoBase::Absolute);
				previousLocation = kLocationSoma;
				somaSampleSegments.clear();
				continue;
			}

			// SWC uses radius - so double it
			dataPoint.d *= 2.0f;

			// to cope with dodgy Gulyas data - would be better to 
			dataPoint.x = std::min(dataPoint.x, 99999.0f);
			dataPoint.y = std::min(dataPoint.y, 99999.0f);
			dataPoint.z = std::min(dataPoint.z, 99999.0f);
			dataPoint.d = std::min(dataPoint.d, 99999.0f);
			//

			// SOMA DATA

			// Gulyas special treatment!!
			if(hintStyle == kHintGulyas)
			{
				// gulyas data is treated as dendrite data.
				// generate a point soma, for point with parent -1
				if(parent == -1)
				{
					builder.NewSomaSinglePoint(dataPoint);
					somaSampleSegments[segment] = builder.CurrentSoma().NumSamples()-1;

					continue;
				}
			}

			// Other formats should be manageable with the same algorithm
			else
			{
				if(IsSomaLocationId( (SampleLocation) location) )
				{
					if(builder.NumSomas() == 0)
						builder.NewSomaSinglePoint(dataPoint);
					else
						builder.CsAddSample(dataPoint);

					somaSampleSegments[segment] = builder.CurrentSoma().NumSamples()-1;

					continue;
				}
			}


			// TREE DATA

			// is this a tree connected to the soma?
			// or is this the first sample, but there was no soma data to connect to?
			// then create new tree structure...
			if(	parent == -1 || somaSampleSegments.find(parent) != somaSampleSegments.end() )
			{
				SampleLocation loc = StructureNumberToLocation(hintStyle, (SampleLocation) location) ;
				if(loc == kLocationAxon)
					builder.NewAxon();
				else
				{
					builder.NewDendrite();

					if(loc == kLocationDendriteApical)
						builder.CdSetApical();
				}

				// +1, so that we don't find this branch for binary root points..
				builder.CbSetTag(kTagSampleNumber, segment+1);
				builder.CbSetTag(kTagSampleType, location);
			}
			else
			{
				MorphologyBuilder::Branch parentBran = builder.CdFindBranchWithTagCondition(kTagSampleNumber, MorphologyBuilder::CheckIfTagInRangeFunctor(parent));

				const bool unacceptableParent = (parentBran == MorphologyBuilder::NullBranch());

				// if the branch cant be found - must be branching root point..
				if(unacceptableParent && segment != parent+1)
				{
					const bool axonExists		= (builder.NumAxons() > 0);
					const bool dendriteExists	= (builder.NumDendrites() > 0);

					LogImportWarning("Found what is probably a branching root point: treating as separate trees.");

					if(hintStyle == kHintGulyas)
					{
						builder.NewDendrite();
					}
					else
					{
						if(location == kLocationAxon)
						{
							if(location == previousLocation)
							{
								assert(axonExists);
								if(axonExists)
								{
									const SamplePoint startPointAxon = builder.CurrentAxon().GetRoot().GetSample(0);
									builder.NewAxonWithSample(startPointAxon);
								}
								else
								{
									builder.NewAxon();
								}
							}
							else
							{
								if(dendriteExists)
								{
									const SamplePoint startPointDend = builder.CurrentDendrite().GetRoot().GetSample(0);
									builder.NewAxonWithSample(startPointDend);
								}
								else
								{
									builder.NewAxon();
								}
							}
						}
						else
						{
							if(location == previousLocation)
							{
								if(dendriteExists)
								{
									const SamplePoint startPointDend = builder.CurrentDendrite().GetRoot().GetSample(0);
									builder.NewDendriteWithSample(startPointDend);
								}
								else
								{
									builder.NewDendrite();
								}
							}
							else
							{
								if(axonExists)
								{
									const SamplePoint startPointAxon = builder.CurrentAxon().GetRoot().GetSample(0);
									builder.NewDendriteWithSample(startPointAxon);
								}
								else
								{
									builder.NewDendrite();
								}
							}
						}
					}

					builder.CbSetTag(kTagSampleNumber, segment);
					builder.CbSetTag(kTagSampleType, location);
				}
				else
				{
					// new branch connected to current dendrite ?
					if(segment != parent+1)
					{
						const int parentLocation = builder.GetTagForBranch(parentBran, kTagSampleType);

						// expect non-soma attached structure to have consistent location tag
						if( !LocationTypesMayBeAttached(hintStyle, (SampleLocation) location, (SampleLocation) parentLocation ) )
							LogImportWarning( String("Unexpected connectivity between incompatible structure: will try to continue.") );


						builder.SetCurrentBranch(parentBran);

						const int startsegment = builder.CbGetTag(kTagSampleNumber);
						const int size = builder.CurrentBranch().NumSamples();


						assert(parent <= startsegment + size-2);

						// splitting a branch to make a branch point
						if(parent < startsegment + size-2)
						{
							builder.CbSplit(parent - (startsegment-1));
							builder.CbSetTag(kTagSampleNumber, parent + 1);
							builder.CbSetTag(kTagSampleType, parentLocation);
							builder.SetCurrentBranch(parentBran);
						}
						// otherwise looks like we have a non-binary tree since this node is already a branch point

						builder.CbNewChildLast(true);
						builder.CbSetTag(kTagSampleNumber, segment);
						builder.CbSetTag(kTagSampleType, location);
					}
				}
			}
			
			builder.CbAddSample(dataPoint);

			previousLocation = location;
			
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
};

bool ImportSWC(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	SWCNeuronParser parser(rIn, nrn, options);
	return parser.Parse();
}


bool ImportRawSWC(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportSWC(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteSWCDataPoint(Output& rOut, int segment, int location, const SamplePoint& sample, int parent)
{
	rOut.writeLine(" " +	dec(segment)		+ " " + 
							dec(location)		+ " " + 
							dec(sample.x, 3)	+ " " +
							dec(sample.y, 3)	+ " " +
							dec(sample.z, 3)	+ " " +
							dec(0.5f*sample.d, 3)	+ " " +
							dec(parent) );
}

int OutputSomaSWC(Output& rOut, const Neuron3D &nrn, int segment, int parent)
{
	if(nrn.CountSomas() > 0)
	{
		const Soma3D& soma = *nrn.SomaBegin();
		if(soma.m_samples.size() > 0)
		{
			for(u32 n=0;n<soma.m_samples.size();++n)
			{
				WriteSWCDataPoint(rOut, segment, kLocationSoma, soma.m_samples[n], parent);
				parent = segment;
				++segment;
			}
		}
	}

	return segment;
}


int OutputBranchSWC(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int branchparent, int segment, int location, bool cnic)
{
	int parent = branchparent;

	const bool endsInBranchPoint	= (it.child() && it.child().peer());
	const bool endsInTerminal		= !it.child();

	const u32 numSamples = (*it).m_samples.size();

	for(u32 i=1;i<numSamples;++i)
	{
		int locationMod = location;
		if(cnic && (i == numSamples-1) )
		{
			if(endsInBranchPoint)
				locationMod = kLocationCnicBranch;
			else if(endsInTerminal)
				locationMod = kLocationCnicTerminal;
		}

		WriteSWCDataPoint(rOut, segment, locationMod, (*it).m_samples[i], parent);
		parent = segment;
		segment++;
	}

	if(it.child()) segment = OutputBranchSWC(rOut, it.child(), parent, segment, location, cnic);
	if(it.peer()) segment = OutputBranchSWC(rOut, it.peer(), branchparent, segment, location, cnic);

	return segment;
}

bool ExportSWC(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// configure
	FormatStyle swcFormat = options.formatStyle;

	const bool cnic = (swcFormat == kStyleSwc_CNIC);

	if(options.identifyNL)
		rOut.writeLine("# File written by " + options.appName + " (version " + options.appVersion + ").");
	if(options.addCommentText)
		rOut.writeLine("# " + options.commentText);
	//


	//rOut.writeLine("# Data History - ");
	rOut.writeLine("#");

	int segment = OutputSomaSWC(rOut, nrn, 1, -1);

	const bool noSoma		= (segment == 1);
	int lastSomaSegment		= (noSoma ? -1 : segment-1);

	int branchparent = lastSomaSegment;

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		// todo - find nearest soma point?
		WriteSWCDataPoint(rOut, segment, kLocationAxon, (*a).root()->m_samples[0], lastSomaSegment);
		branchparent = segment;
		segment++;

		segment = OutputBranchSWC(rOut, (*a).root(), branchparent, segment, kLocationAxon, cnic);
	}

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		const SampleLocation loc =  ( (*d).GetTreeType() == Neuron3D::DendriteTree::Apical) ? kLocationDendriteApical : kLocationDendriteBasal;
		// todo - find nearest soma point?
		WriteSWCDataPoint(rOut, segment, loc, (*d).root()->m_samples[0], lastSomaSegment);
		branchparent = segment;
		segment++;

		segment = OutputBranchSWC(rOut, (*d).root(), branchparent, segment, loc, cnic);
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateSWC(Input& rIn, const ValidationOptions& options)
{
	u32 numChecked = 0;
	const u32 maxCheck = 4;

	int segment = 9999999;
	int location = 9999999;
	SamplePoint dataPoint;
	int parent = 9999999;

	String line;

	while(rIn.remaining() && numChecked < maxCheck)
	{
		line = rIn.readLine();

		if(StringBegins(line, "#") || StringBegins(line, "*") || StringTrim(line) == "")
			continue;

		++numChecked;
		istringstream ss(line, stringstream::in);
		ss >> segment >> location >> dataPoint.x >> dataPoint.y >> dataPoint.z >> dataPoint.d >> parent;

		if(segment != numChecked)
			return kValidationFalse;

		if(numChecked == 1 && parent != -1)
			return kValidationFalse;
	}

	return kValidationTrue;
}
