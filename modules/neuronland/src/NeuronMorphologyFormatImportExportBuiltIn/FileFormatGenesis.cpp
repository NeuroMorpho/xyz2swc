//
//
//

#include "ImportExportCommonIncludes.h"

#include <map>

#include "Core/Parser.h"


/**

	Genesis readcell format

	Structure supported ->
		Soma		: SEGMENT
		Dendrite	: YES
		Axon		: YES
		Marker		: NO
		Spine		: NO

		Pending:
		* some sort of spine support..
**/

enum GenesisCommand
{
	// co-ords
	kRelative,			kAbsolute,			kPolar,				kCartesian,
	kLambdaWarn,		kLambdaUnwarn,		kDoubleEndpointOff, kDoubleEndpoint,	

	// shape
	kSpherical,			kCylindrical,		kSymmetric,			kAsymmetric,

	// params
	kSetComptParam,		kSetGlobalVariable,

	// parameters
	kStartCell,			kAppendToCell,		kMakeProto,			kCompt,

	// spines + branches
	kMembFactor,		kAddSpines,			kRandSpines,		kMrandSpines,
	kFixedSpines,		kMfixedSpines,		kRandBranches,

	//
	kNumGenesisCommands,
	kGenesisCommandError
};

const char* gs_genesisCommands[] =
{
	// co-ords
	"*relative",		"*absolute",		"*polar",			"*cartesian",
	"*lambda_warn",		"*lambda_unwarn",	"*double_endpoint_off", "*double_endpoint",	

	// shape
	"*spherical",		"*cylindrical",		"*symmetric",		"*asymmetric",

	// parameters
	"*set_compt_param",	"*set_global variable",

	// prototypes
	"*start_cell",		"*append_to_cell",	"*makeproto",		"*compt",

	// spines + branches
	"*memb_factor",		"*add_spines",		"*rand_spines",		"*mrand_spines",
	"*fixed_spines",	"*mfixed_spines",	"*rand_branches"
};

GenesisCommand GenesisStringToCommand(const String& str)
{
	for(u32 n=0;n<sizeof(gs_genesisCommands)/sizeof(char*);++n)
		if(StringBegins(str, gs_genesisCommands[n]))
			return (GenesisCommand)n;

	return kGenesisCommandError;
}

const String kDefaultSomaPrefix("soma");
const String kDefaultDendPrefix("d");
const String kDefaultAxonPrefix("a");


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class GenesisParser : public SimpleParser
{
	enum StructureType
	{
		kNone,
		kSoma, 
		kAxon,
		kDend,
		kUnknown
	};

	String mSomaPrefix;
	String mAxonPrefix;
	String mDendPrefix;

	StructureType GetStructureFromName(String name)
	{
		if(StringBegins(name, mSomaPrefix))
			return kSoma;
		if(StringBegins(name, mDendPrefix))
			return kDend;
		if(StringBegins(name, mAxonPrefix))
			return kAxon;
		if(name == "none")
			return kNone;

		return kUnknown;
	}


	std::map<String, SamplePoint> somaNameSampleMapping;

	void AddSomaSampleByName(String name, SamplePoint& sample)
	{
		somaNameSampleMapping[name] = sample;
	}

	bool GetSomaSampleByName(String name, SamplePoint& sample)
	{
		if(somaNameSampleMapping.find(name) != somaNameSampleMapping.end() )
		{
			sample = somaNameSampleMapping[name];
			return true;
		}
		else
			return false;
	}

public:
	GenesisParser(Input& rIn, Neuron3D &nrn, const HintOptions& options) : SimpleParser(rIn), builder(nrn), m_nrn(nrn)
	{
		m_polarCoords = false;
		m_relativeCoords = false;
		m_doubleEndpoint = false;
		m_shape = kCylindrical;

		mLastPointType = kUnknown;

		mSomaPrefix = options.hasHintSoma ? options.hintSomaSubString : kDefaultSomaPrefix;
		mDendPrefix = options.hasHintDend ? options.hintDendSubString : kDefaultDendPrefix;
		mAxonPrefix = options.hasHintAxon ? options.hintAxonSubString : kDefaultAxonPrefix;
	}

	bool ReadCompartment(const String& line, String& name, String& parent, SamplePoint &sample)
	{
		assert(!m_doubleEndpoint);

		Strings items = StringExplode(line," \t");
		StringsClean(items);

		if(items.size() < 6)
		{
			LogImportFailure("Too few compartment elements: " + dec((u32)items.size()) );
			return false;
		}

		name		= items[0];
		parent		= items[1];
		sample.x	= std::stof( items[2] );
		sample.y	= std::stof( items[3] );
		sample.z	= std::stof( items[4] );
		sample.d	= std::stof( items[5] );

		// translate co-ords here if necessary
		if(m_polarCoords)
		{
		}

		return true;
	}

	bool ReadCompartmentDouble(const String& line, String& name, String& parent, SamplePoint &sample0, SamplePoint &sample)
	{
		assert(m_doubleEndpoint);

		Strings items = StringExplode(line, " \t");
		StringsClean(items);

		if(items.size() < 9)
		{
			LogImportFailure("Too few compartment elements: " + dec((u32)items.size()) );
			return false;
		}

		name		= items[0];
		parent		= items[1];
		sample0.x	= std::stof( items[2] );
		sample0.y	= std::stof( items[3] );
		sample0.z	= std::stof( items[4] );
		sample.x	= std::stof( items[5] );
		sample.y	= std::stof( items[6] );
		sample.z	= std::stof( items[7] );
		sample.d	= std::stof( items[8] );

		// translate co-ords here if necessary
		if(m_polarCoords)
		{
		}

		return true;
	}

	bool ReadCompartmentData(const String& line)
	{
		const HString kCompartmentNameTag = "compartment";

		String name;
		String parent;
		SamplePoint sample0;
		SamplePoint sample;

		bool ok;
		if(m_doubleEndpoint)
			ok = ReadCompartmentDouble(line, name, parent, sample0, sample);
		else
			ok = ReadCompartment(line, name, parent, sample);

		if(!ok)
		{
			LogImportFailure("Couldn't read compartment data");
			return false;
		}


		//

		StructureType type			= GetStructureFromName(name);
		StructureType parentType	= GetStructureFromName(parent);

		if(parentType == kNone && type != kSoma)
			LogImportWarning("Root structure is not identified as soma.");

		// default to dendrite, if we can't identify..
		if(type == kUnknown)
			type = kDend;
		if(parentType == kUnknown)
			parentType = kDend;
		//

		if(parent == ".")
		{
			if(mLastPointType == kUnknown)
			{
				LogImportFailure("Cannot have parent '.', if no previous points!");
				return false;
			}

			if( (type != mLastPointType) )
			{
				LogImportWarning("Continuation type is not the same as the previous point type. Assuming last point type (connectivity overrides any name-derived assumptions about compartment type)");
			}

			switch(mLastPointType)
			{
			case kAxon:
			case kDend:
				{
					MorphologyBuilder::Branch parentBranch = builder.CurrentBranch();
					if(m_relativeCoords)
						sample += parentBranch.GetLastSample();

					builder.SetCurrentBranch(parentBranch);
					builder.CbNewChildLast(true);

					builder.CbAddSample(sample);
					builder.CbSetTag(kCompartmentNameTag, HString(name).hash());
				}
				break;
			case kSoma:
				if(m_relativeCoords)
					sample += builder.CurrentSoma().GetLastSample();

				builder.CsAddSample(sample);
				AddSomaSampleByName(name, sample);

				break;
			default:
				LogImportFailure("Trying to continue unknown compartment.");
				return false;
			}
		}
		else
		{
			switch(type)
			{
			case kSoma:
				{
					if(parentType != kNone && m_relativeCoords)
					{
						const SamplePoint lastSample = builder.CurrentSoma().GetLastSample();

						sample += lastSample;
						if(m_doubleEndpoint)
							sample0 += lastSample;
					}

					if(parentType == kNone)
					{
						builder.NewSomaSegment();
						if(m_doubleEndpoint)
							builder.CsAddSample(sample0);
					}

					builder.CsAddSample(sample);
					AddSomaSampleByName(name, sample);
					mLastPointType = kSoma;
				}
				break;

			default:
			case kAxon:
			case kDend:
				{
					MorphologyBuilder::Branch parentBranch = builder.CdFindBranchWithTag(kCompartmentNameTag, HString(parent).hash());

					if(!parentBranch)
					{
						if(parentType == kNone)
						{
							if(type == kAxon)
								builder.NewAxon();
							else
								builder.NewDendrite();

							if(m_doubleEndpoint)
								builder.CbAddSample(sample0);
						}
						else 
						{
							if(parentType != kSoma)
								LogImportWarning("No parent branch, but parent type doesn't seem to be soma: " + parent + ".");

							SamplePoint attachPoint;

							if(m_doubleEndpoint)
								attachPoint = sample0;
							else
							{
								bool ok = GetSomaSampleByName(parent, attachPoint);
								if(!ok)
								{
									LogImportFailure("Cannot find sample with id: " + parent);
									return false;
								}
							}

							if(type == kAxon)
								builder.NewAxonWithSample( attachPoint );
							else
								builder.NewDendriteWithSample( attachPoint );
						}
					}
					else
					{
						if(parentType == kNone || parentType == kSoma)
							LogImportWarning("Non-tree parent section, for non-root tree section!!!");

						if(type != parentType)
							LogImportWarning("Tree point name suggests inconsistent parent type, continuing anyway, using parent type.");

						if(!parentBranch)
						{
							LogImportFailure("Cannot find parent branch for segment: " + name + " (parent: " + parent + ")");
							return false;
						}

						if(m_relativeCoords)
							sample += parentBranch.GetLastSample();

						builder.SetCurrentBranch(parentBranch);
						builder.CbNewChildLast(true);
					}

					builder.CbAddSample(sample);
					builder.CbSetTag(kCompartmentNameTag, HString(name).hash());
					mLastPointType = (type == kAxon ? kAxon : kDend);
				}
				break;

			}
		}

		return true;
	}

	bool Parse()
	{
		String line;
		while(rIn.remaining() > 0)
		{
			line = rIn.readLine();

			if(line == "" || StringBegins(line, "\t") || StringBegins(line, "//"))
				continue;

			GenesisCommand command = GenesisStringToCommand(line);

			switch(command)
			{
			// these affect how we interpret the compartment positional data
			case kRelative:
				m_relativeCoords = true;
				break;
			case kAbsolute:
				m_relativeCoords = false;
				break;

			case kPolar:
				// not supported yet.
				m_polarCoords = true;
				LogImportFailure("Polar coordinates not supported");
				return false;

			case kCartesian:
				m_polarCoords = false;
				break;

			case kDoubleEndpoint:
				m_doubleEndpoint = true;
				break;

			case kDoubleEndpointOff:
				m_doubleEndpoint = false;
				break;

			case kSpherical:
				m_shape = kSpherical;
				break;

			case kCylindrical:
				m_shape = kCylindrical;
				break;


			// some branch data
			case kCompt:
				break;

			case kMembFactor:
			case kAddSpines:
			case kRandSpines:
			case kMrandSpines:
			case kFixedSpines:
			case kMfixedSpines:
			case kRandBranches:
				LogImportWarning("Ignoring spine-related command: [" + line + "]");
				break;

			case kSymmetric:
			case kAsymmetric:
			case kLambdaWarn:
			case kLambdaUnwarn:
			case kSetComptParam:
				// silently ignore these
				break;

			case kSetGlobalVariable:
			case kStartCell:
			case kAppendToCell:
			case kMakeProto:
				LogImportWarning("Ignoring unsupported command: [" + line + "]");
				break;

			// ignore everything else for the moment
			default:

				if( StringBegins(line, "*"))
				{
					LogImportFailure("Unknown command: " + line);
					return false;
				}

				// must be compartment data..
				if(!ReadCompartmentData(line))
					return false;

				break;
			}

			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}

		// simplify tree 

		builder.ApplyChanges();

		m_nrn.MergeAllUnaryBranchPoints();
		m_nrn.EnhanceSingleSampleTrees();

		return true;
	}

private:
	MorphologyBuilder builder;
	Neuron3D &m_nrn;

	bool m_polarCoords;
	bool m_relativeCoords;
	bool m_doubleEndpoint;
	GenesisCommand m_shape;

	StructureType mLastPointType;

};

bool ImportGenesis(Input& rIn, Neuron3D &nrn, const HintOptions& options)	
{
	GenesisParser parser(rIn, nrn, options);
	return parser.Parse();
}


bool ImportRawGenesis(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportGenesis(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

static const int tabLength = 4;
const char* tabs = "\t\t\t\t";

String AppendTabs(const String& str, u32 maxTabs)
{
	return str + String(tabs, (maxTabs*tabLength - str.length())/tabLength);
}

void WriteComment(Output& rOut, const String& filename, const String& appName, const String& appVersion, const String& comment)
{
	rOut.writeLine("// Genesis readcell Morphology file");
	rOut.writeLine("// Generated by " + appName + "(version " + appVersion + ")" );
	rOut.writeLine("// Original filename: " + filename);
	rOut.writeLine("// " + comment);
	rOut.writeLine("//");
	rOut.writeLine("");
}

void WriteCompartmentParameters(Output& rOut)
{
	rOut.writeLine("*absolute");
	rOut.writeLine("*cartesian");
	rOut.writeLine("*asymmetric");
	rOut.writeLine("");
	rOut.writeLine("*compt /library/compartment");
}

void WriteCompartment(Output& rOut, const String& compartmentName, const String& parentName, const SamplePoint& sample)
{
	rOut.writeLine(	compartmentName + " \t" + 
					parentName + "  \t" + 
					dec(sample.x, 3) + " \t" +
					dec(sample.y, 3) + " \t" +
					dec(sample.z, 3) + " \t" +
					dec(sample.d, 3));
}

void WriteCompartmentDouble(Output& rOut, const String& compartmentName, const String& parentName, const SamplePoint& sample0, const SamplePoint& sample)
{
	rOut.writeLine(	compartmentName + " \t" +
					parentName + " \t" + 
					dec(sample0.x, 3) + " \t" +
					dec(sample0.y, 3) + " \t" +
					dec(sample0.z, 3) + " \t" +
					dec(sample.x, 3) + " \t" +
					dec(sample.y, 3) + " \t" +
					dec(sample.z, 3) + " \t" +
					dec(sample.d, 3) );
}

void WriteGenesisSoma(Output& rOut, Neuron3D::SomaConstIterator sit, const String& nameprefix)
{
	for(u32 n=0;n<(*sit).m_samples.size();++n)
	{
		String name = String(nameprefix + "[" + dec(n) + "]");
		String parentName = (n == 0 ? "none" : ".");
		WriteCompartment(rOut, name, parentName, (*sit).m_samples[n]);
	}
}

int WriteGenesisBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int branchcount, const String& parent, const String& nameprefix)
{
	String branchName;

	const bool startWithDoubleEndpoint = !(it.parent());

	for(u32 n=1;n<(*it).m_samples.size();++n)
	{
		branchName = String(nameprefix + dec(branchcount) + "[" + dec(n-1) + "]");
		String parentName = (n == 1 ? parent : ".");

		if(n == 1 && startWithDoubleEndpoint)
		{
			rOut.writeLine("*double_endpoint");
			WriteCompartmentDouble(rOut, branchName, parentName, (*it).m_samples[n-1], (*it).m_samples[n]);
			rOut.writeLine("*double_endpoint_off");
		}
		else
			WriteCompartment(rOut, branchName, parentName, (*it).m_samples[n]);
	}

	++branchcount;

	if(it.child()) branchcount = WriteGenesisBranch(rOut, it.child(), branchcount, branchName, nameprefix);
	if(it.peer()) branchcount = WriteGenesisBranch(rOut, it.peer(), branchcount, parent, nameprefix);

	return branchcount;
}

void WriteGenesisTrees(Output& rOut, const Neuron3D &nrn, const String& somaname, const String& axonPrefix, const String& dendPrefix)
{
	int branchcount = 0;

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		branchcount = WriteGenesisBranch(rOut, (*d).root(), branchcount, somaname, dendPrefix);
	}

	for(Neuron3D::AxonConstIterator d = nrn.AxonBegin(); d != nrn.AxonEnd(); ++d)
	{
		branchcount = WriteGenesisBranch(rOut, (*d).root(), branchcount, somaname, axonPrefix);
	}
}

bool ExportGenesis(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	const bool identifyNL = options.identifyNL;

	String somaPrefix = options.customiseSomaName ? options.somaName : kDefaultSomaPrefix;
	String dendPrefix = options.customiseDendName ? options.dendName : kDefaultDendPrefix;
	String axonPrefix = options.customiseAxonName ? options.axonName : kDefaultAxonPrefix;

	WriteComment(rOut, options.fileName, identifyNL ? options.appName : "", identifyNL ? options.appVersion : "", options.commentText);
	WriteCompartmentParameters(rOut);

	// only write the first soma
	String somaConnection = somaPrefix;
	if(nrn.CountSomas() > 0)
	{
		somaConnection += "[" + dec((u32)nrn.SomaBegin()->m_samples.size()-1) + "]";
		WriteGenesisSoma(rOut, nrn.SomaBegin(), somaPrefix);
	}
	else
		WriteCompartment(rOut, somaConnection, "none", SamplePoint(0,0,0,0) );
	//

	WriteGenesisTrees(rOut, nrn, somaConnection, axonPrefix, dendPrefix);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateGenesis(Input& rIn, const ValidationOptions& options)
{
	String line;
	const int maxLineCount = 8;
	int commentCount = 0;
	int commandCount = 0;
	int lineCount = 0;

	while(lineCount < maxLineCount && rIn.remaining() > 0)
	{
		line = rIn.readLine();

		if( StringBegins(line, "//"))
		{
			++commentCount;
			continue;
		}

		if(line == "")
			continue;

		++lineCount;

		if(StringBegins(line, "*"))
		{
			++commandCount;
			if(GenesisStringToCommand(line) == kGenesisCommandError)
				return kValidationFalse;
			else
				return kValidationTrue;
		}
	}

	return kValidationFalse;
}


#if 0
// This is a GENESIS representation of the morphological components of a single cell from a MorphML file.
// Note: this mapping is only for Level 1 NeuroML morphologies. To convert a Level 2+ file (e.g. including channel densities)
// to GENESIS, use a NeuroML compliant application/metasimulator like neuroConstruct.



*absolute
*cartesian
*asymmetric

// Cell Name : L23PyrFRB

*compt /library/compartment

// Adding compartment for Segment: Seg0_comp_1 ID: 0

*double_endpoint
Seg0_comp_1 none 0.0 0.0 0.0 0.0 7.5 0.0 16.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_1 ID: 1
Seg1_comp_1 Seg0_comp_1 -0.0 15.0 0.0 16.0

// Adding compartment for Segment: Seg0_comp_69 ID: 136

*double_endpoint
Seg0_comp_69 Seg0_comp_1 0.0 0.0 0.0 -0.0 -12.5 0.0 1.8
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_69 ID: 137
Seg1_comp_69 Seg0_comp_69 -0.0 -25.0 0.0 1.8

// Adding compartment for Segment: Seg0_comp_38 ID: 74

*double_endpoint
Seg0_comp_38 Seg1_comp_1 -0.0 15.0 0.0 -0.0 40.0 0.0 8.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_38 ID: 75
Seg1_comp_38 Seg0_comp_38 -0.0 65.0 0.0 8.0

// Adding compartment for Segment: Seg0_comp_9 ID: 16

*double_endpoint
Seg0_comp_9 Seg1_comp_1 -0.0 15.0 0.0 -0.0 -5.0 -15.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_9 ID: 17
Seg1_comp_9 Seg0_comp_9 0.0 -25.0 -30.0 1.0

// Adding compartment for Segment: Seg0_comp_8 ID: 14

*double_endpoint
Seg0_comp_8 Seg1_comp_1 -0.0 15.0 0.0 15.0 -5.0 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_8 ID: 15
Seg1_comp_8 Seg0_comp_8 30.0 -25.0 0.0 1.0

// Adding compartment for Segment: Seg0_comp_7 ID: 12

*double_endpoint
Seg0_comp_7 Seg1_comp_1 -0.0 15.0 0.0 -0.0 -5.0 15.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_7 ID: 13
Seg1_comp_7 Seg0_comp_7 0.0 -25.0 30.0 1.0

// Adding compartment for Segment: Seg0_comp_6 ID: 10

*double_endpoint
Seg0_comp_6 Seg1_comp_1 -0.0 15.0 0.0 -15.0 -5.0 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_6 ID: 11
Seg1_comp_6 Seg0_comp_6 -30.0 -25.0 0.0 1.0

// Adding compartment for Segment: Seg0_comp_5 ID: 8

*double_endpoint
Seg0_comp_5 Seg1_comp_1 -0.0 15.0 0.0 17.320509 10.0 -17.320509 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_5 ID: 9
Seg1_comp_5 Seg0_comp_5 34.641018 5.0 -34.64101 1.0

// Adding compartment for Segment: Seg0_comp_4 ID: 6

*double_endpoint
Seg0_comp_4 Seg1_comp_1 -0.0 15.0 0.0 17.320509 10.0 17.320509 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_4 ID: 7
Seg1_comp_4 Seg0_comp_4 34.640957 5.0 34.640995 1.0

// Adding compartment for Segment: Seg0_comp_3 ID: 4

*double_endpoint
Seg0_comp_3 Seg1_comp_1 -0.0 15.0 0.0 -17.320509 10.0 -17.320509 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_3 ID: 5
Seg1_comp_3 Seg0_comp_3 -34.641 5.0 -34.641014 1.0

// Adding compartment for Segment: Seg0_comp_2 ID: 2

*double_endpoint
Seg0_comp_2 Seg1_comp_1 -0.0 15.0 0.0 -17.320509 10.0 17.320509 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_2 ID: 3
Seg1_comp_2 Seg0_comp_2 -34.64103 5.0 34.64107 1.0

// Adding compartment for Segment: Seg0_comp_70 ID: 138

*double_endpoint
Seg0_comp_70 Seg1_comp_69 -0.0 -25.0 0.0 -0.0 -50.0 0.0 1.4
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_70 ID: 139
Seg1_comp_70 Seg0_comp_70 -0.0 -75.0 0.0 1.4

// Adding compartment for Segment: Seg0_comp_39 ID: 76

*double_endpoint
Seg0_comp_39 Seg1_comp_38 -0.0 65.0 0.0 -0.0 90.0 0.0 7.2
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_39 ID: 77
Seg1_comp_39 Seg0_comp_39 -0.0 115.0 0.0 7.2

// Adding compartment for Segment: Seg0_comp_13 ID: 24

*double_endpoint
Seg0_comp_13 Seg1_comp_38 -0.0 65.0 0.0 -0.0 65.0 -25.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_13 ID: 25
Seg1_comp_13 Seg0_comp_13 -0.0 65.0 -50.0 1.0

// Adding compartment for Segment: Seg0_comp_12 ID: 22

*double_endpoint
Seg0_comp_12 Seg1_comp_38 -0.0 65.0 0.0 25.0 65.0 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_12 ID: 23
Seg1_comp_12 Seg0_comp_12 50.0 65.0 0.0 1.0

// Adding compartment for Segment: Seg0_comp_11 ID: 20

*double_endpoint
Seg0_comp_11 Seg1_comp_38 -0.0 65.0 0.0 -0.0 65.0 25.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_11 ID: 21
Seg1_comp_11 Seg0_comp_11 0.0 65.0 50.0 1.0

// Adding compartment for Segment: Seg0_comp_10 ID: 18

*double_endpoint
Seg0_comp_10 Seg1_comp_38 -0.0 65.0 0.0 -25.0 65.0 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_10 ID: 19
Seg1_comp_10 Seg0_comp_10 -50.0 65.0 0.0 1.0

// Adding compartment for Segment: Seg0_comp_21 ID: 40

*double_endpoint
Seg0_comp_21 Seg1_comp_9 0.0 -25.0 -30.0 0.0 -45.0 -45.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_21 ID: 41
Seg1_comp_21 Seg0_comp_21 0.0 -65.0 -60.0 1.0

// Adding compartment for Segment: Seg0_comp_20 ID: 38

*double_endpoint
Seg0_comp_20 Seg1_comp_8 30.0 -25.0 0.0 45.0 -45.0 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_20 ID: 39
Seg1_comp_20 Seg0_comp_20 60.0 -65.0 0.0 1.0

// Adding compartment for Segment: Seg0_comp_19 ID: 36

*double_endpoint
Seg0_comp_19 Seg1_comp_7 0.0 -25.0 30.0 0.0 -45.0 45.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_19 ID: 37
Seg1_comp_19 Seg0_comp_19 0.0 -65.0 60.0 1.0

// Adding compartment for Segment: Seg0_comp_18 ID: 34

*double_endpoint
Seg0_comp_18 Seg1_comp_6 -30.0 -25.0 0.0 -45.0 -45.0 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_18 ID: 35
Seg1_comp_18 Seg0_comp_18 -60.0 -65.0 0.0 1.0

// Adding compartment for Segment: Seg0_comp_17 ID: 32

*double_endpoint
Seg0_comp_17 Seg1_comp_5 34.641018 5.0 -34.64101 51.961525 0.0 -51.961517 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_17 ID: 33
Seg1_comp_17 Seg0_comp_17 69.28174 -5.0 -69.28177 1.0

// Adding compartment for Segment: Seg0_comp_16 ID: 30

*double_endpoint
Seg0_comp_16 Seg1_comp_4 34.640957 5.0 34.640995 51.961487 0.0 51.961525 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_16 ID: 31
Seg1_comp_16 Seg0_comp_16 69.28227 -5.0 69.2822 1.0

// Adding compartment for Segment: Seg0_comp_15 ID: 28

*double_endpoint
Seg0_comp_15 Seg1_comp_3 -34.641 5.0 -34.641014 -51.961494 0.0 -51.961517 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_15 ID: 29
Seg1_comp_15 Seg0_comp_15 -69.2822 -5.0 -69.282326 1.0

// Adding compartment for Segment: Seg0_comp_14 ID: 26

*double_endpoint
Seg0_comp_14 Seg1_comp_2 -34.64103 5.0 34.64107 -51.961548 0.0 51.961563 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_14 ID: 27
Seg1_comp_14 Seg0_comp_14 -69.281975 -5.0 69.282005 1.0

// Adding compartment for Segment: Seg0_comp_73 ID: 144

*double_endpoint
Seg0_comp_73 Seg1_comp_70 -0.0 -75.0 0.0 -9.735465 -98.0265 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_73 ID: 145
Seg1_comp_73 Seg0_comp_73 -19.470894 -121.05301 0.0 1.0

// Adding compartment for Segment: Seg0_comp_71 ID: 140

*double_endpoint
Seg0_comp_71 Seg1_comp_70 -0.0 -75.0 0.0 9.735455 -98.0265 0.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_71 ID: 141
Seg1_comp_71 Seg0_comp_71 19.470905 -121.05301 0.0 1.0

// Adding compartment for Segment: Seg0_comp_40 ID: 78

*double_endpoint
Seg0_comp_40 Seg1_comp_39 -0.0 115.0 0.0 0.0 140.0 0.0 6.4
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_40 ID: 79
Seg1_comp_40 Seg0_comp_40 -0.0 165.0 0.0 6.4

// Adding compartment for Segment: Seg0_comp_25 ID: 48

*double_endpoint
Seg0_comp_25 Seg1_comp_13 -0.0 65.0 -50.0 0.0 65.0 -75.0 1.0
*double_endpoint_off

// Adding compartment for Segment: Seg1_comp_25 ID: 49
Seg1_comp_25 Seg0_comp_25 -0.0 65.0 -100.0 1.0

// Adding compartment for Segment: Seg0_comp_24 ID: 46

*double_endpoint
Seg0_comp_24 Seg1_comp_12 50.0 65.0 0.0 75.0 65.0 0.0 1.0
*double_endpoint_off

#endif