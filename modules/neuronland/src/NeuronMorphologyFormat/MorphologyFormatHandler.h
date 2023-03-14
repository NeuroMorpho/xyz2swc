#ifndef MORPHOLOGYFORMATHANDLER_HPP_
#define MORPHOLOGYFORMATHANDLER_HPP_

#include "Core/Input.h"
#include "Core/Output.h"

#include "NeuronRepresentation/Neuron3D.h"

#include "MorphologyFormatHandlerDetails.h"

#include "ValidationResult.h"


typedef Neuron3D NeuronType;

/** 

	@brief  Abstract Base class for Morphology Format handlers

**/

class MorphologyFormatHandler 
{
public:

											MorphologyFormatHandler();
											MorphologyFormatHandler(const MorphologyFormatHandlerDetails& rDetails);
	virtual									~MorphologyFormatHandler() {}

	virtual bool 							Import(Input& rIn, NeuronType &nrn) const = 0;
	virtual bool 							Export(Output& rOut, const NeuronType &nrn) const = 0;
	virtual ValidationResult 				Validate(Input& rIn) const = 0;

	virtual bool 							IsImportable() const = 0;
	virtual bool 							IsExportable() const = 0;
	virtual bool 							IsValidatable() const = 0;

	void									SetDetails(const MorphologyFormatHandlerDetails& details);
	const MorphologyFormatHandlerDetails&	GetDetails() const;

protected:
	MorphologyFormatHandlerDetails m_details;
};

// inlines
inline MorphologyFormatHandler::MorphologyFormatHandler()
 {
}

inline MorphologyFormatHandler::MorphologyFormatHandler(const MorphologyFormatHandlerDetails& rDetails) : m_details(rDetails)
{	
}

inline void MorphologyFormatHandler::SetDetails(const MorphologyFormatHandlerDetails& details)
{
	m_details = details;
}

inline const MorphologyFormatHandlerDetails& MorphologyFormatHandler::GetDetails() const
{
	return m_details;
}


#endif // MORPHOLOGYFORMATHANDLER_HPP_