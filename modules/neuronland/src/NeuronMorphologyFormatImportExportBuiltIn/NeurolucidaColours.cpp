
#include "NeurolucidaColours.h"
#include "Core/Debug.h"

struct colourData
{
	const char* name;
	u8 r,g,b;
};

// TODO - check that RGBs match up with actual Neurolucida RGBs...
const colourData colours[kColour_Num] = 
{
	{"Red",			0xff, 0x00, 0x00},
	{"Green",		0x00, 0xff, 0x00},
	{"Blue",		0x00, 0x00, 0xff},
	{"Magenta", 	0xff, 0x00, 0xff},
	{"Cyan",		0x00, 0xff, 0xff},
	{"Yellow",		0xff, 0xff, 0x00},
	{"Orange",		0xff, 0xA5, 0x00},
	{"White",		0xff, 0xff, 0xff},
	{"DarkRed",		0x8b, 0x00, 0x00},
	{"DarkGreen",	0x01, 0x32, 0x20},
	{"DarkBlue",	0x00, 0x00, 0x8b},
	{"DarkMagenta", 0x8b, 0x00, 0x8b},
	{"DarkCyan",	0x00, 0x8b, 0x8b},
	{"DarkYellow",	0x8b, 0x8b, 0x00},
	{"BrightGray",	0xd3, 0xd3, 0xd3},
	{"MediumGray",	0x80, 0x80, 0x80},
	{"SkyBlue",		0x87, 0xce, 0xeb},
	{"MoneyGreen",	0x21, 0x6c, 0x2a},
	{"Cream",		0xff, 0xfd, 0xd0}
};

NeurolucidaColour NeurolucidaColourString2Id(const String& name)
{
	for(u32 n=0;n<kColour_Num;++n)
	{
		if(name == colours[n].name)
			return (NeurolucidaColour)n;
	}

	return kColour_Unknown;
}

void GetNeurolucidaColourRGB(NeurolucidaColour c, u8& r, u8& g, u8& b)
{
	assert(c != kColour_Unknown);
	r = colours[c].r;
	g = colours[c].g;
	b = colours[c].b;
}
