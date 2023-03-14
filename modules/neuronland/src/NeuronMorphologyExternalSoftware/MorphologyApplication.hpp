#ifndef MORPHOLOGYFORMATSDETAILS_HPP_
#define MORPHOLOGYFORMATSDETAILS_HPP_

#include "CoreSL/String.hpp"

/**

	@brief Contains some fundamental information about other software applications which deal with neuron morphology file formats

	For the moment - just enough information for us to be able to tell if we can support import and export of its formats.


	@todo - much fuller description of software capabilities so we can call upon them if we want... !!!!

**/

class MorphologyApplication
{
public:

	MorphologyApplication(StringRef name, StringRef link, StringRef nativeFormat, StringRef importFormats, StringRef exportFormats);

	StringRef	GetStandardName() const;				///< How the format is identified internally
	StringRef	GetStandardExtensions() const;			///< Typical file extension(s) used (e.g. "swc", "txt;asc", "" if none);
	StringRef	GetAssociatedSoftware() const;			///< The software (if any) which is associated with the file format
	StringRef	GetAdditionalInformation() const;		///< Anything else it is useful to know about the format


private:
	String		m_name;
	String		m_extensions;
	String		m_software;
	String		m_information;

};

inline StringRef	MorphologyFormatDetails::GetStandardName() const				{ return m_name; }
inline StringRef	MorphologyFormatDetails::GetStandardExtensions() const			{ return m_extensions; }
inline StringRef	MorphologyFormatDetails::GetAssociatedSoftware() const			{ return m_software; }
inline StringRef	MorphologyFormatDetails::GetAdditionalInformation() const		{ return m_information; }

#endif // MORPHOLOGYFORMATSDETAILS_HPP_