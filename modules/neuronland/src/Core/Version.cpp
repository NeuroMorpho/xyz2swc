//
//
//

#include "Core/Version.h"

static const char namedelimiter = ':';
static const char numberdelimiter = '.';


Version::Version(const String& name, u32 major, u32 minor, u32 increment) :
	mName(name), mMajor(major), mMinor(minor), mIncrement(increment)
{
}

Version::Version(String versionStr)
{
	mName = versionStr.substr(0, versionStr.find_first_of(namedelimiter));

	String	versionNumberStr = versionStr.substr(versionStr.find_first_of(namedelimiter) + 1);

	u32 offset0 = versionNumberStr.find_first_of(numberdelimiter);
	mMajor = std::stoi(versionNumberStr.substr(0, offset0) );

	String minor_inc = versionNumberStr.substr(offset0+1);
	u32 offset1 = minor_inc.find_first_of(numberdelimiter);

	mMinor = std::stoi(minor_inc.substr(0, offset1) );
	mIncrement = std::stoi(minor_inc.substr(offset1+1));
}

bool Version::operator > (const Version&lhs)
{
	return	(mMajor > lhs.mMajor) || 
			(mMajor == lhs.mMajor && mMinor > lhs.mMinor) ||
			(mMajor == lhs.mMajor && mMinor == lhs.mMinor && mIncrement > lhs.mIncrement );
}

