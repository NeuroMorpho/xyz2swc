#ifndef PLUGINMORPHOLOGYHANDLER_HPP_
#define PLUGINMORPHOLOGYHANDLER_HPP_

#include "Core/String.hpp"
#include "Core/Input.h"
#include "Core/Output.h"

#include "NeuronMorphologyFormat/MorphologyFormatHandler.h"

#include "NeuronPlugInSupportMorphologyFormat/MorphologyFormatPlugInInterface.h"

#include "NeuronSystemPlugIns/PlugInHandler.h"

typedef Neuron3D NeuronType;

/**

	@brief 

**/

class PlugInMorphologyHandler : public MorphologyFormatHandler
{
public:
								PlugInMorphologyHandler(PlugInHandler* pPlugInHandler, const MorphologyFormatPlugInInterface& formatInterface);

	virtual bool				Import(Input& rIn, NeuronType& nrn) const;
	virtual bool				Export(Output& rOut, const NeuronType& nrn) const;
	virtual ValidationResult 	Validate(Input& rIn) const;

	virtual bool 				IsImportable() const;
	virtual bool 				IsExportable() const;
	virtual bool 				IsValidatable() const;

private:

	PlugInHandler*							m_pPlugInHandler;
	const MorphologyFormatPlugInInterface&	m_interface;
};


PlugInMorphologyHandler::PlugInMorphologyHandler(PlugInHandler* pPlugInHandler, const MorphologyFormatPlugInInterface& formatInterface) :
	MorphologyFormatHandler(formatInterface.GetHandlerDetails()),
	m_pPlugInHandler(pPlugInHandler),
	m_interface(formatInterface)
{
}

inline bool PlugInMorphologyHandler::Import(Input& rIn, NeuronType& nrn) const
{
	return m_interface.Import(rIn, nrn);
}

inline bool PlugInMorphologyHandler::Export(Output& rOut, const NeuronType& nrn) const
{
	return m_interface.Export(rOut, nrn);
}

inline ValidationResult PlugInMorphologyHandler::Validate(Input& rIn) const
{
	return m_interface.Validate(rIn);
}

inline bool PlugInMorphologyHandler::IsImportable() const
{
	return m_interface.IsImportable();
}

inline bool PlugInMorphologyHandler::IsExportable() const
{
	return m_interface.IsExportable();
}

inline bool PlugInMorphologyHandler::IsValidatable() const
{
	return m_interface.IsValidatable();
}


#endif // PLUGINMORPHOLOGYHANDLER_HPP_