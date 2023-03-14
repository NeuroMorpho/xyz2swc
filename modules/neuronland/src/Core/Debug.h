#ifndef __CORE_DEBUG_H__NL__
#define __CORE_DEBUG_H__NL__

#include "Core/String.hpp"

void assert(bool);
void report(const String&);
void fatal(const String&);

#endif // __CORE_DEBUG_H__NL__
