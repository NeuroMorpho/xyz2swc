#include "Core/Logger.h"

static Logger* g_pLogger;

void SetCurrentLogger(Logger* logger)
{
	g_pLogger = logger;
}


void WriteToLog(const String& str)
{
	if(g_pLogger)
		g_pLogger->Log(str);
}

void WriteToLog(const char* str)
{
	WriteToLog(String(str) );
}