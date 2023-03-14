#ifndef __CORE_INPUTSOURCE_H__NL__
#define __CORE_INPUTSOURCE_H__NL__

#include "Core/String.hpp"

class Input;

class InputSource
{
	String mIdentifier;

public:
	InputSource(const String& identifier, bool bFront=false);
	virtual ~InputSource();

	const String& getIdentifier() const;

	virtual Input *openInput(const String&)=0;
	virtual String toString() const=0;
};

inline const String& InputSource::getIdentifier() const
{
	return mIdentifier;
}


#endif // __CORE_INPUTSOURCE_H__NL__
