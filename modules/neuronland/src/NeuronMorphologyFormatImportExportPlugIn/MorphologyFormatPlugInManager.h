#ifndef MORPHOLOGYFORMATPLUGINMANAGER_HPP_
#define MORPHOLOGYFORMATPLUGINMANAGER_HPP_

#include <list>

#include "Core/String.hpp"

#include "NeuronMorphologyFormat/MorphologyFormatHandler.h"


class MorphologyFormatPlugInManager : public std::list<MorphologyFormatHandler *>
{
public:
	void	Scan(const String& location);
};


#endif // MORPHOLOGYFORMATPLUGINMANAGER_HPP_
