#ifndef MORPHOLOGYFORMATSCRIPTMANAGER_HPP_
#define MORPHOLOGYFORMATSCRIPTMANAGER_HPP_

#include <list>

#include "NeuronMorphologyFormat/MorphologyFormatHandler.h"

class ScriptMorphologyHandler;

class MorphologyFormatScriptManager : public std::list<MorphologyFormatHandler *>
{
public:
	void	Scan(const String& location);

	void	RegisterScript(ScriptMorphologyHandler* pHandler);
};

#endif // MORPHOLOGYFORMATSCRIPTMANAGER_HPP_
