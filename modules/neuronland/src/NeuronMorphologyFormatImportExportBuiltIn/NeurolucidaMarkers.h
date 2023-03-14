#ifndef _NEUROLUCIDAMARKERS_HPP_
#define _NEUROLUCIDAMARKERS_HPP_

#include "Core/String.hpp"
#include "Core/Types.h"

/**

	There are 50 different marker shapes supported by Neurolucida. 
	
	These are present, in some form, in all Neurolucida formats - Classic, ASC, DAT, and XML. 

	Classic:		appear as [3,N], where N is the corresponding shape id.
	ASC, DAT, XML:	appear as shape name, e.g. OpenCircle, FilledStar

	Note that the default name for a marker is "Marker N" (where 1<=N<=50).

	This (incomplete) list is compiled from publicly available NeurolucidaASC and NeurolucidaDAT files.

	Really should just ask someone for the full list...

*/

enum MarkerShape
{
	kMarker_Invalid,
	kMarker_Dot,
	kMarker_OpenCircle,
	kMarker_Cross,
	kMarker_Plus,
	kMarker_OpenTriangle, // GUESS
	kMarker_6,
	kMarker_OpenSquare,
	kMarker_Asterisk,
	kMarker_9,
	kMarker_FilledStar,
	kMarker_FilledCircle,
	kMarker_FilledUpTriangle,
	kMarker_13,
	kMarker_FilledSquare,
	kMarker_FilledDiamond,
	kMarker_Flower,
	kMarker_OpenStar,
	kMarker_DoubleCircle,
	kMarker_Circle1,
	kMarker_Circle2,
	kMarker_Circle3,
	kMarker_Circle4,
	kMarker_Circle5,
	kMarker_Circle6,
	kMarker_Flower2, // GUESS
	kMarker_CircleCross, // GUESS
	kMarker_27,
	kMarker_28,
	kMarker_SnowFlake,
	kMarker_30,
	kMarker_31,
	kMarker_MalteseCross,
	kMarker_FilledQuadStar,
	kMarker_OpenQuadStar,
	kMarker_Flower3,
	kMarker_Pinwheel,
	kMarker_TexacoStar,
	kMarker_38,
	kMarker_39,
	kMarker_40,
	kMarker_ThinArrow,
	kMarker_42,
	kMarker_43,
	kMarker_44,
	kMarker_45,
	kMarker_46,
	kMarker_47,
	kMarker_Splat,
	kMarker_CircleArrow,
	kMarker_50,

	kMarker_Num = kMarker_50
};

bool			IsNeurolucidaMarkerShapeKnown(u32 id);

String			GetNeurolucidaMarkerShapeName(MarkerShape id);
bool			IsNeurolucidaMarkerShapeName(const String& name);
MarkerShape		NeurolucidaShapeName2MarkerShape(const String& name);
String			NeurolucidaMarkerShape2ShapeName(u32 id);

String			NeurolucidaMarkerShape2DefaultName(u32 id);

u32				NeurolucidaDefaultNameToMarkerShape(const String& name);

#endif // _NEUROLUCIDAMARKERS_HPP_

