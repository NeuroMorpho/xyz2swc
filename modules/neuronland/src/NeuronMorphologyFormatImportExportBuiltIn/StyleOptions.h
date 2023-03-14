#ifndef STYLEOPTIONS_HPP_
#define STYLEOPTIONS_HPP_

#include "Core/Types.h"
#include "Core/String.hpp"


// todo: reorganise this and all related code...
enum FormatStyle
{
	kStyle_Default,

	kStyleNLAsc_Modern,			// latest Neurolucida style (default)
	kStyleNLAsc_Verbose,		// old style, each sample has a comment giving full tree path from root branch.
	kStyleNLAsc_ClassicTags,	// old style, with additional comments indicating equivalent NeurolucidaASCClassic tags
	kStyleNLAsc_NoComments,		// 'Modern' with non-header comments removed
	kStyleNLAsc_Compact,		// 'NoComments' with reduced whitespace (tree indentation retained for clarity)
	kStyleNLAsc_NMPML,			// compact, with NMPML header

	kStyleNLDat_Std,
	kStyleNLDat_Corrected,

	kStyleNLXML_3,				// sub-trees use "tree" tag
	kStyleNLXML_4,				// sub-trees use "branch tag

	kStyleSwc_Std,				// Standardised SWC style, as used by NeuroMorpho.org
	kStyleSwc_CNIC,				// Style used by CNIC database
//	kStyleSwc_VNED,
	kStyleSwc_Gulyas,

	kStyleHoc_SWC2HOC,
	kStyleHoc_NTSCable,			// apical[]/basal[]
	kStyleHoc_SLSR,				//
	kStyleHoc_CNIC,				// SWC2HOC, with dendrite[]
	kStyleHoc_Topol,			// commonly used layout
	kStyleHoc_TopolEnumTrees,	// as Topol, but individual trees are named better.
	kStyleHoc_GeometryToNeuronPL,
	kStyleHoc_OzNeuronPL,
	kStyleHoc_MorphMLXSL,		//

	kStyleNeuroZoom_LongPath,
	kStyleNeuroZoom_ShortPath,

	kStyleGenesis_Std,
	kStyleGenesis_MorphMLXSL,

	kStyleAmiraMesh_LNeuron,
//	kStyleAmiraMesh_FlyCircuit, // maybe more general ?
	kStyleAmiraMesh_SkeletonGraph,
//	kStyleAmiraMesh_HxSpatialGraph, // not implemented yet!

	kStyle_Num
};

enum NeuroMLVersion
{
	kVersion_10,
	kVersion_11,
	kVersion_12,
	kVersion_13,
	// note we don't explicitly support 1.3alpha or 1.3beta (though both will probably work import fine..)
	kVersion_14,
	kVersion_15,
	kVersion_16,
	kVersion_17,
	kVersion_171,
	kVersion_172,
	kVersion_173,
	kVersion_180,
	kVersion_181,
	kVersion_182,

	//
	kVersion_200,

	kVersion_Num,

	kVersion_Unknown
};


struct StyleOptions
{
	String appName;
	String appVersion;
	String fileName;

	FormatStyle formatStyle;

	NeuroMLVersion versionId;

	String cellName;
	String commentText;

	bool identifyNL;

	bool addCommentText;
	bool customiseSomaName;
	bool customiseAxonName;
	bool customiseDendName;

	String somaName;
	String axonName;
	String dendName;

	String axonColour;
	String dendColour;
	String somaColour;
	String spineColour;
	String markerColour;

	StyleOptions()
	{
		formatStyle = kStyle_Default;
		versionId = kVersion_182;

		identifyNL = false;
		addCommentText = false;
		customiseSomaName = false;
		customiseAxonName = false;
		customiseDendName = false;

		axonColour = "Red";
		dendColour = "Green";
		somaColour = "Blue";
		spineColour = "Orange";
		markerColour = "Yellow";
	}
};


bool IsExportStyle(const String& str);
bool IsImportStyle(const String& str);


// versions
bool IsNeuroMLVersion(const String& str);
NeuroMLVersion ConvertString2NeuroMLVersion(const String& str);
String ConvertNeuroMLVersion2String(NeuroMLVersion ver);

// styles
FormatStyle ConvertString2FormatStyle(const String& str);
FormatStyle GetFormatStyleStartId(int format);
String GetFormatStyleName(FormatStyle style);

#endif // STYLEOPTIONS_HPP_
