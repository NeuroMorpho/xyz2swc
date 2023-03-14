#include "StyleOptions.h"


const char* formatStyle[] =
{
	"default",

	"modern",
	"verbose",
	"classicTags",
	"noComments",
	"compact",
	"nmpml",

	"badmarkerblocks",
	"corrected",

	"nlxml3",
	"nlxml4",

	"stdswc",
	"cnicswc",
//	"vned",
	"gulyasswc", // not used for export

	"swc2hoc",
	"ntscable",
	"slsr",
	"cnic",
	"topol",
	"topolEnumTrees",
	"GeometryToNeuron",
	"OzNeuron",
	"morphMLXSL",

	"longpath",
	"shortpath",

	"default",
	"morphMLXSL",

	"lneuron",
//	"flycircuit",
	"skeletongraph",
//	"spatialgraph",
};

FormatStyle formatStartIdTable[] = 
{
	kStyle_Default,// kMorphologyFormatAmaral,
	kStyleAmiraMesh_LNeuron,// kMorphologyFormatAmiraMesh,
	kStyle_Default,// kMorphologyFormatArborVitae,
	kStyle_Default,// kMorphologyFormatDouglas3D,
	kStyle_Default,// kMorphologyFormatEutectics,
	kStyleGenesis_Std,// kMorphologyFormatGenesis,
	kStyle_Default,// kMorphologyFormatGlasgow,
	kStyle_Default,// kMorphologyFormatGulyasTree,
	kStyle_Default,// kMorphologyFormatImaris,
	kStyle_Default,// kMorphologyFormatInventor,
	kStyle_Default,// kMorphologyFormatKorogod,
	kStyle_Default,// kMorphologyFormatMaxSim,
	kStyleNLAsc_Modern,// kMorphologyFormatNeurolucidaASC,
	kStyle_Default,// kMorphologyFormatNeurolucidaASCClassic,
	kStyleNLDat_Std,// kMorphologyFormatNeurolucidaDAT,
	kStyle_Default,// kMorphologyFormatNeurolucidaDATClassic,
	kStyleNLXML_3,// kMorphologyFormatNeurolucidaXML,
	kStyle_Default,// kMorphologyFormatNeuroML,
	kStyleHoc_SWC2HOC,// kMorphologyFormatNeuronHOC,
	kStyleNeuroZoom_LongPath,// kMorphologyFormatNeuroZoom,
	kStyle_Default,// kMorphologyFormatNevin,
	kStyle_Default,// kMorphologyFormatNINDS3D,
	//kStyle_Default,// kMorphologyFormatOntoMorphXML,
	kStyle_Default,// kMorphologyFormatOxford,
	kStyle_Default,// kMorphologyFormatPSICS,
	kStyle_Default,// kMorphologyFormatSimpleNeuriteTracer,
	kStyleSwc_Std// kMorphologyFormatSWC,

	// 2D/1D not exported..
};


bool IsExportStyle(const String& str)
{
	for(u32 n=0;n<kStyle_Num;++n)
	{
		if(String(formatStyle[n]) == str)
			return true;
	}

	return false;
}

FormatStyle ConvertString2FormatStyle(const String& str)
{	
	for(u32 n=0;n<kStyle_Num;++n)
	{
		if(String(formatStyle[n]) == str)
			return (FormatStyle)n;
	}

	return kStyle_Default;
}

FormatStyle GetFormatStyleStartId(int format)
{
	return formatStartIdTable[format];
}

String GetFormatStyleName(FormatStyle style)
{
	return formatStyle[style];
}



bool IsImportStyle(const String& str)
{
	FormatStyle style = ConvertString2FormatStyle(str);

	return	style == kStyleSwc_CNIC ||
			style == kStyleSwc_Gulyas ||
			style == kStyleSwc_Std;
}


const char* neuroMLVersion[] =
{
	"1.0",
	"1.1",
	"1.2",
	"1.3",
	"1.4",
	"1.5",
	"1.6",
	"1.7",
	"1.7.1",
	"1.7.2",
	"1.7.3",
	"1.8.0",
	"1.8.1",
	"1.8.2",
	"2.0.0"
};


NeuroMLVersion ConvertString2NeuroMLVersion(const String& str)
{	
	for(u32 n=0;n<kVersion_Num;++n)
	{
		if(String(neuroMLVersion[n]) == str)
			return (NeuroMLVersion)n;
	}

	return kVersion_Unknown;
}

bool IsNeuroMLVersion(const String& str)
{
	for(u32 n=0;n<kVersion_Num;++n)
	{
		if(String(neuroMLVersion[n]) == str)
			return true;
	}

	return false;
}

String ConvertNeuroMLVersion2String(NeuroMLVersion ver)
{
	return neuroMLVersion[ver];
}


