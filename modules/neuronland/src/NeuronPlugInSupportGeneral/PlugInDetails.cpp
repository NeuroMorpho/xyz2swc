//
//
//

#include "PlugInDetails.h"

PlugInDetails::PlugInDetails(Version version, const String& type) : m_version(version), m_type(type)
{

}

const String& GetCurrentPlugInVersion()
{
	static String ver("pf-1.0");
	return ver;
}

Version PlugInDetails::GetVersion() const
{
	return m_version;
}

const String& PlugInDetails::GetType() const
{
	return m_type;
}
