
#include "ImportExportLoggingSupport.h"


void LogImportWarning(const String& str)
{
	WriteToLog("Import warning: " + str);
}

void LogImportFailure(const String& str)
{
	WriteToLog("Import failure: " + str);
}

void LogExportWarning(const String& str)
{
	WriteToLog("Export warning: " + str);
}

void LogExportFailure(const String& str)
{
	WriteToLog("Export failure: " + str);
}
