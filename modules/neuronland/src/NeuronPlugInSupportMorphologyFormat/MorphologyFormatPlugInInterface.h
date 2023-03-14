#ifndef MORPHOLOGYFORMATPLUGININTERFACE_HPP_
#define MORPHOLOGYFORMATPLUGININTERFACE_HPP_

#include "Core/String.hpp"

#include "NeuronMorphologyFormat/ValidationResult.h"

class Input;
class Output;
class Neuron3D;
class MorphologyFormatHandlerDetails;

class MorphologyFormatPlugInInterface
{
public:
	virtual const MorphologyFormatHandlerDetails&	GetHandlerDetails() const = 0;

	virtual bool				Import(class Input& rIn, class Neuron3D &nrn) const = 0;
	virtual bool				Export(class Output& rOut, const class Neuron3D &nrn) const = 0;
	virtual ValidationResult	Validate(class Input& rIn) const = 0;

	virtual bool				IsImportable() const = 0;
	virtual bool				IsExportable() const = 0;
	virtual bool				IsValidatable() const = 0;
};


const String& GetMorphologyFormatPlugInStringID();


#endif // MORPHOLOGYFORMATPLUGININTERFACE_HPP_
