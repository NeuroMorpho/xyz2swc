#ifndef __CORE_LOGGER_H__NL__
#define __CORE_LOGGER_H__NL__

#include "Core/String.hpp"

class Logger 
{
public:
	virtual void Log(const String&) = 0;
};


void SetCurrentLogger(Logger* logger);
void WriteToLog(const String& str);
void WriteToLog(const char* str);

#endif // __CORE_LOGGER_H__NL__
