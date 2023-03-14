#ifndef _NEUROLUCIDACOLOURS_HPP_
#define _NEUROLUCIDACOLOURS_HPP_

#include "Core/String.hpp"
#include "Core/Types.h"

/** 

	Colour names which have been observed in real NeurolucidaASC files..

**/

enum NeurolucidaColour
{
	kColour_Red,
	kColour_Green,
	kColour_Blue,
	kColour_Magenta,
	kColour_Cyan,
	kColour_Yellow,
	kColour_Orange,
	kColour_White,

	kColour_DarkRed,
	kColour_DarkGreen,
	kColour_DarkBlue,
	kColour_DarkMagenta,
	kColour_DarkCyan,
	kColour_DarkYellow,

	kColour_BrightGray,
	kColour_MediumGray,

	kColour_SkyBlue,
	kColour_MoneyGreen,

	kColour_Cream,

	kColour_Num,

	kColour_Unknown
};

NeurolucidaColour NeurolucidaColourString2Id(const String& name);
void GetNeurolucidaColourRGB(NeurolucidaColour colour, u8& r, u8& g, u8& b);

#endif // _NEUROLUCIDACOLOURS_HPP_


