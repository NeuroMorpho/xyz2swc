//
//
//

#include "MorphologyFormatHandlerDetails.h"



MorphologyFormatHandlerDetails::MorphologyFormatHandlerDetails(MorphologyFormatHandlerType type, MorphologyFormatID id, const String& name, Version version) :
	m_type(type), m_formatId(id), m_name(name), m_version(version)
{
}



const char* gs_formatTypes[] = 
{
	"BuiltIn",
	"PlugIn",
	"Script"
};

String HandlerTypeToString(MorphologyFormatHandlerType index)
{
	return String(gs_formatTypes[index]);
}
