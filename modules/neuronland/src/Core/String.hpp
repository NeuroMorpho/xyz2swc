#ifndef __CORE_STRING_H__NL__
#define __CORE_STRING_H__NL__

/**

	Helper code for transition to new Core library.

*/

#include <string>
#include <vector>
#include <algorithm>

#include <string.h>

// new String types (now using C++ std lib)
typedef std::string					String;
typedef std::string&				StringRef;
typedef std::vector<std::string>	Strings;

// new implementations for string query and manipulation
inline bool StringBegins(const std::string& str, const char* test)
{
	return str.substr(0, strlen(test)) == std::string(test);
}
inline bool StringBegins(const std::string& str, const std::string& test)
{
	return str.substr(0, test.length()) == test;
}

inline String StringBefore(const std::string& str, char test)
{
	return str.substr(0, str.find_first_of(test));
}
inline String StringBefore(const std::string& str, const char* test)
{
	return str.substr(0, str.find(test));
}
inline String StringBefore(const std::string& str, const std::string& test)
{
	return str.substr(0, str.find(test));
}
inline String StringAfter(const std::string& str, char test)
{
	return str.substr(str.find_first_of(test) + 1);
}
inline String StringAfter(const std::string& str, const char* test)
{
	return str.substr(str.find(test)+strlen(test));
}
inline String StringAfter(const std::string& str, const std::string& test)
{
	return str.substr(str.find(test) + test.length());
}

inline String StringAfterLast(const std::string& str, const char* test)
{
	return str.substr(str.rfind(test) + strlen(test));
}
inline String StringAfterLast(const std::string& str, const std::string& test)
{
	return str.substr(str.rfind(test) + test.length());
}

inline bool StringContains(const std::string& str, char c)
{
	return str.find_first_of(c) != std::string::npos;
}

inline bool StringContains(const std::string& str, const char* test)
{
	return str.find(test) != std::string::npos;
}

inline bool StringContains(const std::string& str, const std::string& test)
{
	return str.find(test) != std::string::npos;
}

inline Strings StringSplit(const std::string& str, char splitter)
{
	Strings out;

	auto l = str.length();

	if (l > 0)
	{
		if (str.find_first_of(splitter) == std::string::npos)
		{
			out.push_back(str);
		}
		else
		{
			int i = 0;

			do
			{
				auto j = str.find_first_of(splitter, i);
				if (j == std::string::npos)
					j = l;

				out.push_back(str.substr(i, j-i));
				i = j + 1;
			} while (i<l);
		}
	}

	return out;
}

inline Strings StringExplode(const String& in, const String& delimiters)
{
	String str = in;
	Strings results;

	int found;
	size_t offset = 0;
	found = str.find_first_of(delimiters, offset);
	while (found != std::string::npos) 
	{
		if (found > 0) 
			results.push_back(str.substr(offset, found - offset));

		offset = found + 1;
		found = str.find_first_of(delimiters, offset);
	}
	if (str.length() > 0) 
	{
		results.push_back(str.substr(offset));
	}

	return results;
}


inline void StringsClean(Strings& strings)
{
	for (auto i = strings.begin(); i != strings.end(); )
	{
		if (i->length() == 0)
		{
			i = strings.erase(i);
		}
		else
		{
			++i;
		}
	}
}

inline void StringReplaceLast(String& str, const String& substr, const String& replacement)
{
	auto pos = str.rfind(substr);

	if(pos != std::string::npos)
		str.replace(pos, substr.length(), replacement);
}

inline void StringRemove(String& str, char c)
{
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

inline String StringTrim(const String& s)
{
	static const std::string WHITESPACE = " \n\r\t\f\v";

	String result;

	size_t start = s.find_first_not_of(WHITESPACE);
	if (start != std::string::npos)
	{
		result = s.substr(start);

		size_t end = result.find_last_not_of(WHITESPACE);

		if(end != std::string::npos)
			result = result.substr(0, end + 1);
	}
	return result;
}

uint32_t StringHash(const String& str);

#endif // __CORE_STRING_H__NL__
