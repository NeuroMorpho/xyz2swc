#ifndef HintOptions_HPP_
#define HintOptions_HPP_

#include "StyleOptions.h"
#include "NeuronRepresentation/ArtificialBranchingConfiguration.h"

struct HintOptions
{
	bool hasHintStyle;
	bool hasHintAxon;
	bool hasHintSoma;
	bool hasHintDend;
	bool allContoursAsSoma;
	bool hasEntryProc;

	FormatStyle		hintStyle;
	String			hintAxonSubString;
	String			hintDendSubString;
	String			hintSomaSubString;
	String			entryProc;

	ArtificialBranchingConfiguration artificialBranchingConfig;

	HintOptions()
	{
		hasHintStyle = false;
		hasHintAxon = false;
		hasHintSoma = false;
		hasHintDend = false;
		allContoursAsSoma = false;
		hasEntryProc = false;
	}
};

#endif // HintOptions_HPP_
