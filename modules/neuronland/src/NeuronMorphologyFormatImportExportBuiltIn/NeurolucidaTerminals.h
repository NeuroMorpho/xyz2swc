#ifndef _NEUROLUCIDATERMINALS_HPP_
#define _NEUROLUCIDATERMINALS_HPP_

#include "Core/Types.h"
#include "Core/String.hpp"


/** 

	IMPORTANT: numerical order corresponds to NeurolucidaDAT order

*/
enum NeurolucidaTerminalType
{
	kNeurolucidaTerminal_Normal,
	kNeurolucidaTerminal_High,
	kNeurolucidaTerminal_Low,
	kNeurolucidaTerminal_Midpoint,
	kNeurolucidaTerminal_Incomplete,
	kNeurolucidaTerminal_Origin,
	kNeurolucidaTerminal_Generated,
//	kNeurolucidaTerminal_BranchPoint, // DAT only

	kNeurolucidaTerminal_Num,
};

bool						IsNeurolucidaTerminalTypeName(const String& name);
bool						IsNeurolucidaXMLTerminalTypeName(const String& name); // seems to be a slightly more limited selection of terminal names supported...
String						GetNeurolucidaTerminalTypeName(NeurolucidaTerminalType t);
NeurolucidaTerminalType		NeurolucidaTerminalName2TerminalType(const String& name);


#endif // _NEUROLUCIDATERMINALS_HPP_

