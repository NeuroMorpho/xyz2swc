#ifndef PLUGINDETAILS_HPP_
#define PLUGINDETAILS_HPP_

#include "Core/Version.h"
#include "Core/String.hpp"

class PlugInDetails
{
public:

				PlugInDetails(Version version, const String& type);
				PlugInDetails() {}

	Version		GetVersion() const;
	const String& GetType() const;

private:
	Version		m_version;
	String		m_type;
};

const String& GetCurrentPlugInVersion();

#endif // PLUGINDETAILS_HPP_