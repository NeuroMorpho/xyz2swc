
#include "Core/String.hpp"
#include "Core/Debug.h"

#if defined(__linux__)
#include <signal.h>
#endif

void assert(bool condition)
{
	if (!condition)
	{
	#if defined(__linux__)
		raise(SIGTRAP);
	#else
		__debugbreak();
	#endif
	}
}

void report(const String& str)
{
	// TODO
}

void fatal(const String& str)
{
	printf( ("Fatal Error: " + str).c_str() );
	while (1)
	{
	};
}
