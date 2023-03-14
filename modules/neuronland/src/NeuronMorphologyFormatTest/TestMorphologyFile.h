#ifndef __TESTMORPHOLOGYFORMAT_HPP
#define __TESTMORPHOLOGYFORMAT_HPP

#include "Core/String.hpp"

#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"

namespace Core
{
	class Output;
}

void TestNeuronMorphologyFile(Output* pOut, const String& filename, BuiltInMorphologyFormat origFormat);

#endif // __TESTMORPHOLOGYFORMAT_HPP