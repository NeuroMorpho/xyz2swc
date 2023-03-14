//
//
//

#include "Core/FArray.h"

#include "BuiltInMorphologyFormat.h"

#include "NeuronMorphologyFormat/MorphologyFormatDetails.h"

#include "MorphologyFormatImportExport.h"

/**
	
	Must be in same order as BuiltInMorphologyFormat enum

	@todo temporary - will be data driven

**/


static MorphologyFormatDetails sg_builtInDetails[] =
{
	// 3D
	MorphologyFormatDetails("Amaral"),
	MorphologyFormatDetails("AmiraMesh"),
	MorphologyFormatDetails("ArborVitae"),
	MorphologyFormatDetails("Douglas3D"),
	MorphologyFormatDetails("Eutectics"),
	MorphologyFormatDetails("Genesis"),
	MorphologyFormatDetails("Glasgow"),
	MorphologyFormatDetails("GulyasTree"),
	MorphologyFormatDetails("ImarisHDF"),
	MorphologyFormatDetails("Inventor"),
	MorphologyFormatDetails("Korogod"),
	MorphologyFormatDetails("MaxSim"),
	MorphologyFormatDetails("NeurolucidaASC"),
	MorphologyFormatDetails("NeurolucidaASCClassic"),
	MorphologyFormatDetails("NeurolucidaDAT"),
	MorphologyFormatDetails("NeurolucidaDATClassic"),
	MorphologyFormatDetails("NeurolucidaXML"),
	MorphologyFormatDetails("NeuroML"),
	MorphologyFormatDetails("NeuronHOC"),
	MorphologyFormatDetails("NeuroZoom"),
	MorphologyFormatDetails("Nevin"),
	MorphologyFormatDetails("NINDS3D"),
//	MorphologyFormatDetails("OntoMorphXML"),
	MorphologyFormatDetails("Oxford"),
	MorphologyFormatDetails("PSICS"),
	MorphologyFormatDetails("SimpleNeuriteTracer"),
	MorphologyFormatDetails("SWC"),

	// 2D
	MorphologyFormatDetails("Bannister"),
	MorphologyFormatDetails("NeuronJ"), // 2D

	// 1D
	MorphologyFormatDetails("Douglas1D"),
	MorphologyFormatDetails("GulyasSeg1D"),
	MorphologyFormatDetails("NINDS1D"),
};


static MorphologyFormatHandlerDetails sg_builtInHandlerDetailsAsBuiltIn[] = 
{
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Amaral"),				"BuiltIn Amaral",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("AmiraMesh"),			"BuiltIn AmiraMesh",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("ArborVitae"),			"BuiltIn ArborVitae",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Douglas3D"),			"BuiltIn Douglas3D",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Eutectics"),			"BuiltIn Eutectics",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Genesis"),				"BuiltIn Genesis",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Glasgow"),				"BuiltIn Glasgow",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("GulyasTree"),			"BuiltIn GulyasTree",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("ImarisHDF"),			"BuiltIn ImarisHDF",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Inventor"),				"BuiltIn Inventor",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Korogod"),				"BuiltIn Korogod",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("MaxSim"),				"BuiltIn MaxSim",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeurolucidaASC"),		"BuiltIn NeurolucidaASC",		Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeurolucidaASCClassic"),"BuiltIn NeurolucidaASCClassic",Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeurolucidaDAT"),		"BuiltIn NeurolucidaDAT",		Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeurolucidaDATClassic"),"BuiltIn NeurolucidaDATClassic",Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeurolucidaXML"),		"BuiltIn NeurolucidaXML",		Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeuroML"),				"BuiltIn NeuroML",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeuronHOC"),			"BuiltIn NeuronHOC",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeuroZoom"),			"BuiltIn NeuroZoom",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Nevin"),				"BuiltIn Nevin",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NINDS3D"),				"BuiltIn NINDS3D",				Version(String("JMO:1.0.0"))),
//	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("OntoMorphXML"),			"BuiltIn OntoMorphXML",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Oxford"),				"BuiltIn Oxford",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("PSICS"),				"BuiltIn PSICS",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("SimpleNeuriteTracer"),	"BuiltIn SimpleNeuriteTracer",	Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("SWC"),					"BuiltIn SWC",					Version(String("JMO:1.0.0"))),

	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Bannister"),			"BuiltIn Bannister",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NeuronJ"),				"BuiltIn NeuronJ",				Version(String("JMO:1.0.0"))),

	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("Douglas1D"),			"BuiltIn Douglas1D",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("GulyasSeg1D"),			"BuiltIn GulyasSeg1D",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatBuiltIn, 	MorphologyFormatID("NINDS1D"),				"BuiltIn NINDS1D",				Version(String("JMO:1.0.0"))),
};

static MorphologyFormatHandlerDetails sg_builtInHandlerDetailsAsPlugIn[] = 
{
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Amaral"),				"PlugIn Amaral",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("AmiraMesh"),			"PlugIn AmiraMesh",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("ArborVitae"),			"PlugIn ArborVitae",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Douglas3D"),			"PlugIn Douglas3D",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Eutectics"),			"PlugIn Eutectics",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Genesis"),				"PlugIn Genesis",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Glasgow"),				"PlugIn Glasgow",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("GulyasTree"),			"PlugIn GulyasTree",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("ImarisHDF"),			"PlugIn ImarisHDF",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Inventor"),				"PlugIn Inventor",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Korogod"),				"PlugIn Korogod",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("MaxSim"),				"PlugIn MaxSim",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeurolucidaASC"),		"PlugIn NeurolucidaASC",		Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeurolucidaASCClassic"),"PlugIn NeurolucidaASCClassic",	Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeurolucidaDAT"),		"PlugIn NeurolucidaDAT",		Version(String("JMO:1.0.0"))),		
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeurolucidaDATClassic"),"PlugIn NeurolucidaDATClassic",	Version(String("JMO:1.0.0"))),		
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeurolucidaXML"),		"PlugIn NeurolucidaXML",		Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeuroML"),				"PlugIn NeuroML",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeuronHOC"),			"PlugIn NeuronHOC",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeuroZoom"),			"PlugIn NeuroZoom",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Nevin"),				"PlugIn Nevin",					Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NINDS3D"),				"PlugIn NINDS3D",				Version(String("JMO:1.0.0"))),
//	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("OntoMorphXML"),			"PlugIn OntoMorphXML",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Oxford"),				"PlugIn Oxford",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("PSICS"),				"PlugIn PSICS",					Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("SimpleNeuriteTracer"),	"PlugIn SimpleNeuriteTracer",	Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("SWC"),					"PlugIn SWC",					Version(String("JMO:1.0.0"))),

	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Bannister"),			"PlugIn Bannister",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NeuronJ"),				"PlugIn NeuronJ",				Version(String("JMO:1.0.0"))),
	
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("Douglas1D"),			"PlugIn Douglas1D",				Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("GulyasSeg1D"),			"PlugIn GulyasSeg1D",			Version(String("JMO:1.0.0"))),
	MorphologyFormatHandlerDetails(kMorphologyFormatPlugIn, 	MorphologyFormatID("NINDS1D"),				"PlugIn NINDS1D",				Version(String("JMO:1.0.0"))),
};

MorphologyFormatID MorphologyFormatBuiltIn2ID(BuiltInMorphologyFormat format)
{
	return MorphologyFormatID( sg_builtInDetails[format].GetName() );
}

const MorphologyFormatDetails& GetBuiltInMorphologyFormatDetails(BuiltInMorphologyFormat format)
{
	// if not loaded, load all the data!?
	return sg_builtInDetails[format];
}

const MorphologyFormatHandlerDetails&	GetBuiltInMorphologyFormatHandlerDetails(BuiltInMorphologyFormat format)
{
	return sg_builtInHandlerDetailsAsBuiltIn[format];
}

const MorphologyFormatHandlerDetails&	GetBuiltInMorphologyFormatHandlerDetailsAsPlugIn(BuiltInMorphologyFormat format)
{
	return sg_builtInHandlerDetailsAsPlugIn[format];
}

// temp
bool iequals(const String& a, const String& b)
{
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
		return tolower(a) == tolower(b);
	});
}
//

BuiltInMorphologyFormat	BuiltInFormatNameToFormatID(const String& name)
{
	for(u32 n=0;n<kMorphologyFormatBuiltInNumber;++n)
		if(iequals(sg_builtInDetails[n].GetName(), name))
			return (BuiltInMorphologyFormat)n;

	return kMorphologyFormatError;
}

struct FormatFunctions
{
	MorphologyImportFunction	Import;
	MorphologyImportRawFunction	ImportRaw;
	MorphologyExportFunction	Export;
	MorphologyValidateFunction	Validate;
};


#define FORMATFUNCTION(name)  { &(Import##name), &(ImportRaw##name), &(Export##name), &(Validate##name) }

FormatFunctions gs_formatFunctions[] = 
{
	FORMATFUNCTION(Amaral),
	FORMATFUNCTION(AmiraMesh),
	FORMATFUNCTION(ArborVitae),
	FORMATFUNCTION(Douglas3D),
	FORMATFUNCTION(Eutectics),
	FORMATFUNCTION(Genesis),
	FORMATFUNCTION(Glasgow),
	FORMATFUNCTION(GulyasTree),
	FORMATFUNCTION(ImarisHDF),
	FORMATFUNCTION(Inventor),
	FORMATFUNCTION(Korogod),
	FORMATFUNCTION(MaxSim),
	FORMATFUNCTION(NeurolucidaASC),
	FORMATFUNCTION(NeurolucidaASCClassic),
	FORMATFUNCTION(NeurolucidaDAT),
	FORMATFUNCTION(NeurolucidaDATClassic),
	FORMATFUNCTION(NeurolucidaXML),
	FORMATFUNCTION(NeuroML),
	FORMATFUNCTION(NeuronHOC),
	FORMATFUNCTION(NeuroZoom),
	FORMATFUNCTION(Nevin),
	FORMATFUNCTION(NINDS3D),
	//FORMATFUNCTION(OntoMorphXML),
	FORMATFUNCTION(Oxford),
	FORMATFUNCTION(PSICS),
	FORMATFUNCTION(SimpleNeuriteTracer),
	FORMATFUNCTION(SWC),

	FORMATFUNCTION(Bannister),
	FORMATFUNCTION(NeuronJ),

	FORMATFUNCTION(Douglas1D),
	FORMATFUNCTION(GulyasSeg1D),
	FORMATFUNCTION(NINDS1D),
};

bool ImportMorphologyFormat(BuiltInMorphologyFormat format, Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	return (*(gs_formatFunctions[format].Import))(rIn, nrn, options);
}

bool ImportRawMorphologyFormat(BuiltInMorphologyFormat format, const char* file, Neuron3D &nrn, const HintOptions& options)
{
	return (*(gs_formatFunctions[format].ImportRaw))(file, nrn, options);
}

bool ExportMorphologyFormat(BuiltInMorphologyFormat format, Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	return (*(gs_formatFunctions[format].Export))(rOut, nrn, options);
}

ValidationResult ValidateMorphologyFormat(BuiltInMorphologyFormat format, Input& rIn, const ValidationOptions& options)
{
	return (*(gs_formatFunctions[format].Validate))(rIn, options);
}

MorphologyImportFunction		GetBuiltInMorphologyImportFunction(BuiltInMorphologyFormat format)
{
	return gs_formatFunctions[format].Import;
}

MorphologyImportRawFunction		GetBuiltInMorphologyImportRawFunction(BuiltInMorphologyFormat format)
{
	return gs_formatFunctions[format].ImportRaw;
}

MorphologyExportFunction		GetBuiltInMorphologyExportFunction(BuiltInMorphologyFormat format)
{
	return gs_formatFunctions[format].Export;
}

MorphologyValidateFunction		GetBuiltInMorphologyValidateFunction(BuiltInMorphologyFormat format)
{
	return gs_formatFunctions[format].Validate;
}


bool IsImportableMorphologyFormat(BuiltInMorphologyFormat format)
{
	return gs_formatFunctions[format].Import != 0;
}

bool IsExportableMorphologyFormat(BuiltInMorphologyFormat format)
{
	return gs_formatFunctions[format].Export != 0;
}

bool IsValidatableMorphologyFormat(BuiltInMorphologyFormat format)
{
	return gs_formatFunctions[format].Validate != 0;
}


struct FormatProperties
{
	u8		somaSupport;
	bool	dendriteSupport;
	bool	dendriteSubTypeSupport;
	bool	axonSupport;
	bool	binaryTreesOnly;
	bool	neurolucidaTerminalTypes;
	u8		markerSupport;
	u8		spineSupport;


	// temp - should be somewhere else.
	u8		numVersions;
	u8		numStyles;
};

// info which should really be found in format nmf files..
FormatProperties properties[kMorphologyFormatBuiltInNumber] =
{
	// soma																dends	subtype	axons	binary	NLterm	markers							spines    
	{ kSomaPoint,														true,	false,	false,	false,	false,	0,								0,								1,	1}, // kMorphologyFormatAmaral,
	{ kSomaNone,														true,	false,	false,	false,	false,	0,								0,								1,	1}, // kMorphologyFormatAmiraMesh,
	{ kSomaNone,														true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatArborVitae,
	{ kSomaNone|kSomaContour,											true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatDouglas3D,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple|kSomaMixed,		true,	false,	true,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatEutectics,
	{ kSoma1or2Pts|kSomaContour,										true,	false,	true,	false,	false,	0,								0,								1,	1}, // kMorphologyFormatGenesis,
	{ kSoma1or2Pts|kSomaNone|kSomaContour,								true,	false,	true,	false,	false,	0,								0,								1,	1}, // kMorphologyFormatGlasgow,
	{ kSomaNone,														true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatGulyasTree,
	{ kSomaNone,														true,	false,	false,	false,	false,	0,								kBranch,						1,	1}, // kMorphologyFormatImarisHDF,
	{ kSomaNone,														true,	false,	false,	false,	false,	0,								kBranch,						1,	1}, // kMorphologyFormatInventor,
	{ kSoma1or2Pts|kSomaNone|kSomaContour,								true,	false,	true,	false,	false,	0,								0,								1,	1}, // kMorphologyFormatKorogod,
	{ kSomaNone,														false,	false,	true,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatMaxSim,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple|kSomaMixed,		true,	true,	true,	false,	true,	kGlobal|kSoma|kBranch|kNamed,	kBranch|kRootSample0,			1,	6}, // kMorphologyFormatNeurolucidaASC,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple|kSomaMixed,		true,	true,	true,	true,	false,	kSoma|kBranch,					0,								1,	1}, // kMorphologyFormatNeurolucidaASCClassic,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple|kSomaMixed,		true,	true,	true,	false,	true,	kGlobal|kSoma|kBranch|kNamed,	kBranch|kRootSample0,			1,	2}, // kMorphologyFormatNeurolucidaDAT,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple|kSomaMixed,		true,	true,	true,	true,	false,	kSoma|kBranch,					0,								1,	1}, // kMorphologyFormatNeurolucidaDATClassic,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple|kSomaMixed,		true,	true,	true,	false,	true,	kGlobal|kSoma|kBranch|kNamed,	kBranch|kRootSample0,			1,	1}, // kMorphologyFormatNeurolucidaXML,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple,				true,	false,	true,	false,	false,	kGlobal|kNamed,					kGlobal|kBranch,				1, 1}, // kMorphologyFormatNeuroML,
	{ kSoma1or2Pts|kSomaContour,										true,	false,	true,	false,	false,	0,								0,								1,	2}, // kMorphologyFormatNeuronHOC,
	{ kSoma1or2Pts|kSomaNone|kSomaContour|kSomaMultiple|kSomaMixed,		true,	false,	true,	false,	false,	kGlobal|kNamed,					kGlobal,						1,	2}, // kMorphologyFormatNeuroZoom,
	{ kSoma1or2Pts|kSomaNone|kSomaContour,								true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatNevin,
	{ kSoma1or2Pts|kSomaNone|kSomaPoint,								true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatNINDS3D,
//	{ kSomaNone,														true,	true,	true,	false,	true,	kGlobal|kSoma|kBranch|kNamed,	kBranch,						1,	1}, // kMorphologyFormatOntoMorphXML,
	{ kSomaNone,														true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatOxford,
	{ kSoma1or2Pts|kSomaNone|kSomaContour,								true,	false,	true,	false,	false,	0,								0,								1,	1}, // kMorphologyFormatPSICS,
	{ kSoma1or2Pts|kSomaNone|kSomaContour,								true,	true,	true,	false,	false,	0,								0,								1,	2}, // kMorphologyFormatSimpleNeuriteTracer,
	{ kSoma1or2Pts|kSomaNone|kSomaContour,								true,	true,	true,	false,	false,	0,								0,								1,	2}, // kMorphologyFormatSWC,

	{ kSomaNone,														true,	true,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatBannister,
	{ kSomaNone,														true,	false,	true,	false,	false,	0,								0,								1,	1}, // kMorphologyFormatNeuronJ,

	{ kSomaNone|kSomaPoint,												true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatDouglas1D,
	{ kSomaNone,														true,	false,	false, 	true,	false,	0,								0,								1,	1}, // kMorphologyFormatGulyasSeg1D,
	{ kSoma1or2Pts|kSomaNone|kSomaPoint,								true,	false,	false,	true,	false,	0,								0,								1,	1}, // kMorphologyFormatNINDS1D,
	// soma																dends	subtype	axons	binary	markers							spines
};


bool IsFormatBinaryOnly(BuiltInMorphologyFormat format)
{
	return properties[format].binaryTreesOnly;
}

bool DoesFormatSupportAxons(BuiltInMorphologyFormat format)
{
	return properties[format].axonSupport;
}

bool DoesFormatSupportDendrites(BuiltInMorphologyFormat format)
{
	return properties[format].dendriteSupport;
}

bool DoesFormatSupportDendriteSubTypes(BuiltInMorphologyFormat format)
{
	return properties[format].dendriteSubTypeSupport;
}

bool DoesFormatSupportTerminalTypes(BuiltInMorphologyFormat format)
{
	return properties[format].neurolucidaTerminalTypes;
}

bool DoesFormatSupportSomas(BuiltInMorphologyFormat format, u32 types)
{
	return (properties[format].somaSupport & types) == types;
}

bool DoesFormatSupportMarkers(BuiltInMorphologyFormat format, u32 locality)
{
	return (properties[format].markerSupport & locality) == locality;
}

bool DoesFormatSupportSpines(BuiltInMorphologyFormat format, u32 locality)
{
	return (properties[format].spineSupport & locality) == locality;
}

u32 GetFormatMarkerSupport(BuiltInMorphologyFormat format)
{
	return properties[format].markerSupport;
}

u32 GetFormatSpineSupport(BuiltInMorphologyFormat format)
{
	return properties[format].spineSupport;
}




bool IsFormatFull3D(BuiltInMorphologyFormat format)
{
	return	format != kMorphologyFormatBannister &&
			format != kMorphologyFormatNINDS1D &&
			format != kMorphologyFormatDouglas1D &&
			format != kMorphologyFormatGulyasSeg1D &&
			format != kMorphologyFormatNeuronJ;
}

bool IsFormatFullyBufferable(BuiltInMorphologyFormat format)
{
	return format != kMorphologyFormatImarisHDF;
}

u32 GetNumberOfExportStylesSupported(BuiltInMorphologyFormat format)
{
	return (properties[format].numStyles);
}

u32 GetNumberOfExportVersionsSupported(BuiltInMorphologyFormat format)
{
	return (properties[format].numVersions);
}
