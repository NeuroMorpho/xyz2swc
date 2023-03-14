//
// StringFormatting.cpp
//

/** 

	@todo - error checking

**/

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "StringFormatting.h"

//static const char* blankSpaces = "                                                                                                                                                                                                                                                                        ";
char blankSpaces[2056];

String AppendSpaces(const String& str, u32 totalSize)
{
	memset(blankSpaces, ' ', totalSize);
	return str + String(blankSpaces, totalSize - str.length() );
}

String PrependSpaces(const String& str, u32 totalSize, u32 minSpaces/* = 0*/)
{
	memset(blankSpaces, ' ', totalSize + minSpaces);

	u32 adjustedTotalSize = std::max(totalSize, (u32)str.length());
	return String(blankSpaces, std::max(minSpaces, adjustedTotalSize - (u32)str.length()) ) + str;
}

String dec(u32 i, int digits)
{
	return std::to_string(i);
}

String dec(int i, int digits)
{
	return std::to_string(i);
}

String dec(float f, int digits /*= 0*/)
{
	if (digits == 0)
	{
		return std::to_string(f);
	}
	else
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(digits) << f;
		return stream.str();
	}
}

String hex(u32 number, int padfrontzero/* = 0*/)
{
	std::stringstream stream;
	stream << std::hex << number;
	return stream.str();
}
String hex(u16 number, int padfrontzero/* = 0*/)
{
	std::stringstream stream;
	stream << std::hex << number;
	return stream.str();
}
String hex(u8 number, int padfrontzero/* = 0*/)
{
	std::stringstream stream;
	stream << std::hex << number;
	return stream.str();
}
String hex(const void* ptr, int padfrontzero/* = 0*/)
{
	std::stringstream stream;
	stream << std::hex << (uintptr_t)ptr;
	return stream.str();
}


#if 0

String hex(u64 num, int padfrontzero)
{
	char result[17];
	int shift = 60;
	u64 mask = u64(0xf) << shift;
	int i = 16;

	for (; i>1; i--)
	{
		if ((num & mask) || (i <= padfrontzero))
			break;

		mask >>= 4;
		shift -= 4;
	}

	char *dest = result;
	for (; i>0; i--)
	{
		assert(shift >= 0);

		int ch = (int)((num & mask) >> shift);
		*dest++ = hexChars[ch];

		mask >>= 4;
		shift -= 4;
	}
	*dest++ = 0;
	return String(result);
}



void dec(String &res, const s32 number, int padfrontzero)
{
	if (padfrontzero == 0) padfrontzero = 1;
	assert(padfrontzero >= 1 && padfrontzero < 32);
	char format[12];
	sprintf(format, "%%.%dd", padfrontzero);
	char result[32];
	sprintf(result, format, number);
	res = result;
}

String dec(s32 number, int padfrontzero)
{
	String result;
	dec(result, number, padfrontzero);
	return String(result);
}

///////////////////////////////////////////////////////////////////

void dec(String &res, u32 number, int padfrontzero)
{
	if (padfrontzero == 0) padfrontzero = 1;
	assert(padfrontzero >= 1 && padfrontzero < 32);
	char format[12];
	sprintf(format, "%%.%du", padfrontzero);
	char result[32];
	sprintf(result, format, number);
	res = result;
}

String dec(u32 number, int padfrontzero)
{
	String result;
	dec(result, number, padfrontzero);
	return String(result);
}

///////////////////////////////////////////////////////////////////

void dec(String &res, float number, int precision)
{
	assert(precision >= 0 && precision < 12);
	char format[12];
	sprintf(format, "%%.%df", precision);
	char result[64];
	sprintf(result, format, number);
	res = result;
}

String dec(float number, int precision)
{
	String result;
	dec(result, number, precision);
	return String(result);
}

#ifdef _WIN32
void dec(String &res, double number, int precision)
{
	assert(precision >= 0 && precision < 12);
	char format[12];
	sprintf(format, "%%.%df", precision);
	char result[64];
	sprintf(result, format, number);
	res = result;
}

String dec(double number, int precision)
{
	String result;
	dec(result, number, precision);
	return String(result);
}
#endif

///////////////////////////////////////////////////////////////////

String dec(u64 number, int /* pad */)
{
	char result[40];
#if defined _WIN32
	sprintf(result, "%I64u", number);
#else
	sprintf(result, "%lu", number);
#endif
	return String(result);
}


///////////////////////////////////////////////////////////////////

String dec(s64 number, int /* pad */)
{
	char result[40];
#if defined _WIN32
	sprintf(result, "%I64d", number);
#else
	sprintf(result, "%ld", number);
#endif
	return String(result);
}

#endif

