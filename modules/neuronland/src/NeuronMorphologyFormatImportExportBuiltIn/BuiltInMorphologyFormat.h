#ifndef BUILTINMORPHOLOGYFORMAT_HPP_
#define BUILTINMORPHOLOGYFORMAT_HPP_

#include "Types.h"

#include "NeuronRepresentation/Neuron3D.h"
#include "NeuronMorphologyFormat/MorphologyFormatDetails.h"
#include "NeuronMorphologyFormat/MorphologyFormatHandlerDetails.h"
#include "NeuronMorphologyFormat/ValidationResult.h"

#include "HintOptions.h"
#include "StyleOptions.h"
#include "ValidationOptions.h"


class Input;
class Output;


/** 

	Just a convenient enum to help organize, efficiently, the built-in code for various file formats.
	In no way do we make any assumptions that these are the only formats which can be supported by external code - plug-ins or scripts.

**/

enum BuiltInMorphologyFormat
{
	kMorphologyFormatFirst,
	// 3D formats
	kMorphologyFormatAmaral			= kMorphologyFormatFirst,
	kMorphologyFormatAmiraMesh,
	kMorphologyFormatArborVitae,
	kMorphologyFormatDouglas3D,
	kMorphologyFormatEutectics,
	kMorphologyFormatGenesis,
	kMorphologyFormatGlasgow,
	kMorphologyFormatGulyasTree,
	kMorphologyFormatImarisHDF,
	kMorphologyFormatInventor,
	kMorphologyFormatKorogod,
	kMorphologyFormatMaxSim,
	kMorphologyFormatNeurolucidaASC,
	kMorphologyFormatNeurolucidaASCClassic,
	kMorphologyFormatNeurolucidaDAT,
	kMorphologyFormatNeurolucidaDATClassic,
	kMorphologyFormatNeurolucidaXML,
	kMorphologyFormatNeuroML,
	kMorphologyFormatNeuronHOC,
	kMorphologyFormatNeuroZoom,
	kMorphologyFormatNevin,
	kMorphologyFormatNINDS3D,
//	kMorphologyFormatOntoMorphXML,
	kMorphologyFormatOxford,
	kMorphologyFormatPSICS,
	kMorphologyFormatSimpleNeuriteTracer,
	kMorphologyFormatSWC,

	kMorphologyFormatBannister,
	kMorphologyFormatNeuronJ,

	kMorphologyFormatDouglas1D,
	kMorphologyFormatGulyasSeg1D,
	kMorphologyFormatNINDS1D,

	kMorphologyFormatBuiltInNumber,
	kMorphologyFormatError,
};

typedef bool (*MorphologyImportFunction)(Input& rIn, Neuron3D &nrn, const HintOptions& options);
typedef bool (*MorphologyImportRawFunction)(const char*, Neuron3D &nrn, const HintOptions& options);
typedef bool (*MorphologyExportFunction)(Output& rOut, const Neuron3D &nrn, const StyleOptions& options);
typedef ValidationResult (*MorphologyValidateFunction)(Input& rIn, const ValidationOptions& options);

BuiltInMorphologyFormat			BuiltInFormatNameToFormatID(const String& name);

MorphologyFormatID MorphologyFormatBuiltIn2ID(BuiltInMorphologyFormat format);
const MorphologyFormatDetails&	GetBuiltInMorphologyFormatDetails(BuiltInMorphologyFormat format);
const MorphologyFormatHandlerDetails&	GetBuiltInMorphologyFormatHandlerDetails(BuiltInMorphologyFormat format);
const MorphologyFormatHandlerDetails&	GetBuiltInMorphologyFormatHandlerDetailsAsPlugIn(BuiltInMorphologyFormat format);


bool 							ImportMorphologyFormat(BuiltInMorphologyFormat format, Input& rIn, Neuron3D &nrn, const HintOptions& options);
bool 							ImportRawMorphologyFormat(BuiltInMorphologyFormat format, const char* file, Neuron3D &nrn, const HintOptions& options);
bool 							ExportMorphologyFormat(BuiltInMorphologyFormat format, Output& rOut, const Neuron3D &nrn, const StyleOptions& options);
ValidationResult				ValidateMorphologyFormat(BuiltInMorphologyFormat format, Input& rIn, const ValidationOptions& options);

bool 							IsImportableMorphologyFormat(BuiltInMorphologyFormat format);
bool 							IsExportableMorphologyFormat(BuiltInMorphologyFormat format);
bool 							IsValidatableMorphologyFormat(BuiltInMorphologyFormat format);

MorphologyImportFunction		GetBuiltInMorphologyImportFunction(BuiltInMorphologyFormat format);
MorphologyImportRawFunction		GetBuiltInMorphologyImportRawFunction(BuiltInMorphologyFormat format);
MorphologyExportFunction		GetBuiltInMorphologyExportFunction(BuiltInMorphologyFormat format);
MorphologyValidateFunction		GetBuiltInMorphologyValidateFunction(BuiltInMorphologyFormat format);

u32								GetNumberOfExportStylesSupported(BuiltInMorphologyFormat format);
u32								GetNumberOfExportVersionsSupported(BuiltInMorphologyFormat format);

enum 
{
	kLocalityNone	= 0,
	kGlobal			= 1<<0,
	kSoma			= 1<<1,
	kBranch			= 1<<2,
	kNamed			= 1<<3,
	kRootSample0	= 1<<4
};

enum SomaSupport
{
	kSomaNone		= 1<<0,		// 0 points
	kSomaPoint		= 1<<1,		// 1 point
	kSomaCylinder	= 1<<2,		// 2 points
	kSomaContour	= (1<<3)|kSomaCylinder|kSomaPoint, // N points: contours encompass points and cylinders
	kSomaMultiple	= 1<<4,		// multiple sets of N points
	kSoma1or2Pts	= 1<<5,		// supports soma (<=2 points) without any other structure present 
	kSomaMixed		= 1<<6		// if kMultiple, can have multi-point and single-point somas present at the same time.

	//kContourOpen
	//kContourClosed
};

//
bool IsFormatBinaryOnly(BuiltInMorphologyFormat format);
bool DoesFormatSupportAxons(BuiltInMorphologyFormat format);
bool DoesFormatSupportDendrites(BuiltInMorphologyFormat format);
bool DoesFormatSupportDendriteSubTypes(BuiltInMorphologyFormat format);
bool DoesFormatSupportSomas(BuiltInMorphologyFormat format, u32 types);
bool DoesFormatSupportMarkers(BuiltInMorphologyFormat format, u32 locality);
bool DoesFormatSupportSpines(BuiltInMorphologyFormat format, u32 locality);
bool DoesFormatSupportTerminalTypes(BuiltInMorphologyFormat format);


u32 GetFormatMarkerSupport(BuiltInMorphologyFormat format);
u32 GetFormatSpineSupport(BuiltInMorphologyFormat format);

bool IsFormatFull3D(BuiltInMorphologyFormat format);

bool IsFormatFullyBufferable(BuiltInMorphologyFormat format);


#endif // BUILTINMORPHOLOGYFORMAT_HPP_
