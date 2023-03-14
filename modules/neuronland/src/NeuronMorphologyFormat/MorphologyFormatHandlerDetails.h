#ifndef MORPHOLOGYFORMATHANDLERDETAILS_HPP_
#define MORPHOLOGYFORMATHANDLERDETAILS_HPP_

#include "Core/Version.h"

#include "NeuronMorphologyFormat/MorphologyFormatID.h"
#include "NeuronMorphologyFormat/MorphologyHandlerID.h"

/**
**/


// @@@ todo - rename - more generic functionality source..
enum MorphologyFormatHandlerType
{
	kMorphologyFormatBuiltIn,
	kMorphologyFormatPlugIn,
	kMorphologyFormatScript
};


String HandlerTypeToString(MorphologyFormatHandlerType IStringd);

/** 
**/

class MorphologyFormatHandlerDetails
{
public:

									MorphologyFormatHandlerDetails();
									MorphologyFormatHandlerDetails(MorphologyFormatHandlerType type, MorphologyFormatID id, const String& name, Version version);

	MorphologyFormatHandlerType		GetHandlerType() const;
	MorphologyFormatID				GetMorphologyFormat() const;
	String							GetHandlerName() const;
	Version							GetHandlerVersion() const;

	void							SetHandlerType(MorphologyFormatHandlerType type);
	void							SetMorphologyFormat(MorphologyFormatID id);
	void							SetHandlerName(String name);
	void							SetHandlerVersion(Version version);

private:
	MorphologyFormatHandlerType		m_type;
	MorphologyFormatID				m_formatId;
	String							m_name;
	Version							m_version;
};

// inlines

inline MorphologyFormatHandlerDetails::MorphologyFormatHandlerDetails()														{ }

inline MorphologyFormatHandlerType		MorphologyFormatHandlerDetails::GetHandlerType() const								{ return m_type; }
inline MorphologyFormatID				MorphologyFormatHandlerDetails::GetMorphologyFormat() const							{ return m_formatId; }
inline String							MorphologyFormatHandlerDetails::GetHandlerName() const								{ return m_name; }
inline Version							MorphologyFormatHandlerDetails::GetHandlerVersion() const							{ return m_version; }

inline void 							MorphologyFormatHandlerDetails::SetHandlerType(MorphologyFormatHandlerType type)	{ m_type = type; }
inline void 							MorphologyFormatHandlerDetails::SetMorphologyFormat(MorphologyFormatID id)			{ m_formatId = id; }
inline void 							MorphologyFormatHandlerDetails::SetHandlerName(String name)							{ m_name = name; }
inline void 							MorphologyFormatHandlerDetails::SetHandlerVersion(Version version)					{ m_version = version; }

#endif // MORPHOLOGYFORMATHANDLERDETAILS_HPP_
