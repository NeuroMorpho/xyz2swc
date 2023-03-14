#ifndef __CORE_VERSION_H__NL__
#define __CORE_VERSION_H__NL__

#include "Core/Types.h"
#include "Core/String.hpp"

class Version
{
public:
	Version(const String& name, u32 major, u32 minor, u32 increment);
	Version(String versionStr);
	Version() {}

	bool operator > (const Version&lhs);

//private:
	String			mName;
	u32				mMajor;
	u32				mMinor;
	u32				mIncrement;
};

#endif // __CORE_VERSION_H__NL__
