#ifndef __CORE_STRINGFORMATTING_H__NL__
#define __CORE_STRINGFORMATTING_H__NL__

#include "Core/Types.h"
#include "Core/String.hpp"

String AppendSpaces(const String& str, u32 totalSize);
String PrependSpaces(const String& str, u32 totalSize, u32 minSpaces = 0);

String dec(u32 i, int digits = 0);
String dec(int i, int digits = 0);
String dec(float i, int digits = 0);

String hex(u32 number, int padfrontzero=0);
String hex(u16 number, int padfrontzero = 0);
String hex(u8 number, int padfrontzero = 0);
String hex(const void* ptr, int padfrontzero = 0);

#endif // __CORE_STRINGFORMATTING_H__NL__
