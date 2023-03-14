
#include "NeurolucidaTerminals.h"


/**
	These names appear in NeurolucidaASC and NeurolucidaXML files.
*/
const char* terminalNames[kNeurolucidaTerminal_Num] = 
{
	"Normal",
	"High",
	"Low",
	"Midpoint",
	"Incomplete",
	"Origin",
	"Generated",
};

bool IsNeurolucidaTerminalTypeName(const String& name)
{
	for(u32 n=0;n<kNeurolucidaTerminal_Num;++n)
		if(name == String(terminalNames[n]))
			return true;

	return false;
}

bool IsNeurolucidaXMLTerminalTypeName(const String& name)
{
	if(	name != terminalNames[kNeurolucidaTerminal_Generated] && 
		name != terminalNames[kNeurolucidaTerminal_Incomplete] )
	{
		return IsNeurolucidaTerminalTypeName(name);
	}

	return false;
}

NeurolucidaTerminalType NeurolucidaTerminalName2TerminalType(const String& name)
{
	for(u32 n=0;n<kNeurolucidaTerminal_Num;++n)
		if(name == String(terminalNames[n]))
			return (NeurolucidaTerminalType)n;

	return kNeurolucidaTerminal_Normal;
}

String GetNeurolucidaTerminalTypeName(NeurolucidaTerminalType t)
{
	return terminalNames[t];
}