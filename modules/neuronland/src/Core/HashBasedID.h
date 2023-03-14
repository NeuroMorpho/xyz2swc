#ifndef __CORE_HASHBASEDID_H__NL__
#define __CORE_HASHBASEDID_H__NL__

#include "Core/HString.h"

static const u32 kDefaultHashBasedID = HString("UndefinedHash");

/**

	Identifier class, to avoid passing HStrings directly, so it makes sure we are passing a HashBasedID

	Full of flaws...will do for now...

**/

class HashBasedID
{
public:	
					HashBasedID();
					HashBasedID(HString name);
					~HashBasedID() {}

	HString			GetHStringID () const;

	HashBasedID&	operator=(const HashBasedID& name);
	bool			operator== (HashBasedID rhs) const;

	bool			operator!=	(HashBasedID rhs) const;
	bool			operator!=	(HString rhs) const;

	bool			operator<	(HashBasedID rhs) const;
	bool			operator<	(HString rhs) const;

	bool			operator>	(HashBasedID rhs) const;
	bool			operator>	(HString rhs) const;

private:

	HString mId;
	HString GetDefaultId () const;
};

inline HashBasedID::HashBasedID()
: mId(kDefaultHashBasedID)
{
}

inline HashBasedID::HashBasedID(HString name) 
: mId(name)
{
}

inline	HString HashBasedID::GetHStringID() const 
{
	return mId;
}

inline HashBasedID& HashBasedID::operator=(const HashBasedID& name)
{
	mId = name.mId;
	return *this;
}

inline 	bool HashBasedID::operator==(HashBasedID rhs) const
{
	return mId == rhs.mId;
}

inline 	bool HashBasedID::operator!=(HashBasedID rhs) const
{
	return mId != rhs.mId;
}

inline 	bool HashBasedID::operator!=(HString rhs) const
{
	return mId != rhs;
}

inline 	bool HashBasedID::operator<(HashBasedID rhs) const
{
	return mId < rhs.mId;
}

inline 	bool HashBasedID::operator<(HString rhs) const
{
	return mId < rhs;
}

inline 	bool HashBasedID::operator>(HashBasedID rhs) const
{
	return mId > rhs.mId;
}

inline 	bool HashBasedID::operator>(HString rhs) const
{
	return mId > rhs;
}

inline HString HashBasedID::GetDefaultId() const
{
	return kDefaultHashBasedID;
}

#endif // __CORE_HASHBASEDID_H__NL__
