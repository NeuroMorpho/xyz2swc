#ifndef __CORE_HSTRING_H__NL__
#define __CORE_HSTRING_H__NL__

#include "Core/Types.h"
#include "Core/String.hpp"
#include "Core/StringFormatting.h"

class HString
{
	u32 mHash;

	static u32 String2Hash(const String&);

public:

	enum
	{
		EMPTY_STRING_HASH = 0xffffffff
	};

	HString();
	HString(u32 hash);
	HString(const String& string);
	HString(const char *string);

	operator u32 () const;
	u32 hash() const;

	String toStringForm() const;

	HString &operator=(const HString &);
	HString &operator=(const String&);
	HString &operator=(const char *);
	HString &operator=(u32 hash);

	bool operator==(const HString &) const;
	bool operator!=(const HString &) const;
	bool operator==(u32 hash) const;
	bool operator!=(u32 hash) const;
	bool operator==(const String&) const;
	bool operator!=(const String&) const;
	bool operator==(const char *) const;
	bool operator!=(const char *) const;
	
	bool operator <(const HString &) const;		///< Only useful for maps - arbitrary ordering *not* alphabetical
};


inline HString &HString::operator=(const HString &hstring)
{
	mHash=hstring.mHash;
	return *this;
}

inline HString &HString::operator=(const String& string)
{
	mHash= String2Hash(string);
	return *this;
}

inline HString &HString::operator=(const char *string)
{
	mHash=String2Hash(string);
	return *this;
}

inline HString &HString::operator=(u32 hash)
{
	mHash = hash;
	return *this;
}

inline HString::HString(u32 hash)
	: mHash(hash)
{
}

inline HString::HString(const String& string)
	: mHash(String2Hash(string))
{
}

inline HString::HString(const char *string)
	: mHash(String2Hash(string))
{
}

inline HString::HString()
	: mHash((u32) EMPTY_STRING_HASH)
{
}

inline String HString::toStringForm() const
{
	return "#"+hex(mHash, 8);
}

inline bool HString::operator==(u32 hash) const
{
	return mHash == hash;
}

inline bool HString::operator!=(u32 hash) const
{
	return mHash != hash;
}

inline bool HString::operator==(const String& string) const
{
	return mHash == String2Hash(string);
}

inline bool HString::operator!=(const String& string) const
{
	return mHash != String2Hash(string);
}

inline bool HString::operator==(const char *string) const
{
	return mHash == String2Hash(string);
}

inline bool HString::operator!=(const char *string) const
{
	return mHash != String2Hash(string);
}

inline bool HString::operator==(const HString &string) const
{
	return mHash == string.mHash;
}

inline bool HString::operator!=(const HString &string) const
{
	return mHash != string.mHash;
}

inline HString::operator u32 () const
{
	return mHash;
}

inline bool HString::operator <(const HString &hstring) const
{
	return mHash < hstring.mHash;
}

inline u32 HString::hash() const
{
	return mHash;
}

#endif // __CORE_HSTRING_H__NL__
