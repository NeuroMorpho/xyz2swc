
#include "NeurolucidaMarkers.h"
#include "Core/StringFormatting.h"


const char* shapeNames[kMarker_Num+1] = 
{
	"Invalid",
	"Dot",
	"OpenCircle",
	"Cross",
	"Plus",
	"OpenTriangle",
	"Unknown6",
	"OpenSquare",
	"Asterisk",
	"Unknown9",
	"FilledStar",
	"FilledCircle",
	"FilledUpTriangle",
	"Unknown13",
	"FilledSquare",
	"FilledDiamond",
	"Flower",
	"OpenStar",
	"DoubleCircle",
	"Circle1",
	"Circle2",
	"Circle3",
	"Circle4",
	"Circle5",
	"Circle6",
	"Flower2",
	"CircleCross",
	"Unknown27",
	"Unknown28",
	"SnowFlake",
	"Unknown30",
	"Unknown31",
	"MalteseCross",
	"FilledQuadStar",
	"OpenQuadStar",
	"Flower3",
	"Pinwheel",
	"TexacoStar",
	"Unknown38",
	"Unknown39",
	"Unknown40",
	"ThinArrow",
	"Unknown42",
	"Unknown43",
	"Unknown44",
	"Unknown45",
	"Unknown46",
	"Unknown47",
	"Splat",
	"CircleArrow",
	"Unknown50"
};

bool IsNeurolucidaMarkerShapeKnown(u32 id)
{
	return	id>0 && id<=50 && 
			id != 6 &&
			id != 9 &&
			id != 13 &&
			id != 27 &&
			id != 28 &&
			id != 30 &&
			id != 31 &&
			id != 38 &&
			id != 39 &&
			id != 40 &&
			id != 42 &&
			id != 43 &&
			id != 44 &&
			id != 45 &&
			id != 46 &&
			id != 47 &&
			id != 50;
}

String	GetNeurolucidaMarkerShapeName(MarkerShape id)
{
	return String(shapeNames[id]);
}


String	NeurolucidaMarkerShape2ShapeName(u32 id)
{
	if(id > 0 && id <= 50)
		return GetNeurolucidaMarkerShapeName( (MarkerShape) id);
	else
		return GetNeurolucidaMarkerShapeName(kMarker_Dot);
}

MarkerShape	NeurolucidaShapeName2MarkerShape(const String& name)
{
	for(u32 n=1;n<=kMarker_Num;++n)
		if(name == String(shapeNames[n]))
			return (MarkerShape)n;

	return kMarker_Dot;
}

bool	IsNeurolucidaMarkerShapeName(const String& name)
{
	for(u32 n=1;n<=kMarker_Num;++n)
		if(name == String(shapeNames[n]))
			return true;

	return false;
}


String	NeurolucidaMarkerShape2DefaultName(u32 id)
{
	return String( "Marker " + dec(id) );
}


bool NeurolucidaDefaultNameToMarkerShape(const String& name, u32& result)
{
	if(StringBegins(name, "Marker "))
	{
		u32 id = std::atoi( StringAfter(name, "Marker ").c_str() );
		if(id > 0 && id <= 50)
		{
			result = id;
			return true;
		}
	}

	return false;
}

