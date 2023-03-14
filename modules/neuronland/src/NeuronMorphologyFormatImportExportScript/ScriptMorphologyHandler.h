#ifndef SCRIPTMORPHOLOGYHANDLER_HPP_
#define SCRIPTMORPHOLOGYHANDLER_HPP_

#include "Core/String.hpp"
#include "Core/Input.h"
#include "Core/Output.h"

#include "NeuronMorphologyFormat/MorphologyFormatHandler.h"


typedef Neuron3D NeuronType;

/**

	@brief 

**/

class ScriptMorphologyHandler : public MorphologyFormatHandler
{
public:
								ScriptMorphologyHandler(MorphologyFormatID id, String name, Version version);

	virtual bool				Import(Input& rIn, NeuronType& nrn) const;
	virtual bool				Export(Output& rOut, const NeuronType& nrn) const;
	virtual ValidationResult 	Validate(Input& rIn) const;

	virtual bool 				IsImportable() const;
	virtual bool 				IsExportable() const;
	virtual bool 				IsValidatable() const;

private:
};


inline ScriptMorphologyHandler::ScriptMorphologyHandler(MorphologyFormatID id, String name, Version version)
{
	m_details.SetHandlerType(kMorphologyFormatScript);
	m_details.SetMorphologyFormat(id);
	m_details.SetHandlerName(name);
	m_details.SetHandlerVersion(version);
}

inline bool ScriptMorphologyHandler::Import(Input& rIn, NeuronType& nrn) const
{
	return false;
}

inline bool ScriptMorphologyHandler::Export(Output& rOut, const NeuronType& nrn) const
{
	return false;
}

inline ValidationResult ScriptMorphologyHandler::Validate(Input& rIn) const
{
	return kValidationFalse;
}

inline bool ScriptMorphologyHandler::IsImportable() const
{
	return false;
}

inline bool ScriptMorphologyHandler::IsExportable() const
{
	return false;
}

inline bool ScriptMorphologyHandler::IsValidatable() const
{
	return false;
}

#endif // SCRIPTMORPHOLOGYHANDLER_HPP_