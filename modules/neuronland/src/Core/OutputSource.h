#ifndef __CORE_OUTPUTSOURCE_H__NL__
#define __CORE_OUTPUTSOURCE_H__NL__

#include "Core/String.hpp"

class Output;

class OutputSource
{
	String mIdentifier;

public:
	OutputSource(const String& identifier);
	virtual ~OutputSource();

	const String& getIdentifier() const;

	virtual Output *openOutput(const String&)=0;
	virtual Output *overwriteOutput(const String&)=0;
	virtual Output *appendOutput(const String&);
};

inline const String& OutputSource::getIdentifier() const
{
	return mIdentifier;
}

#endif // __CORE_OUTPUTSOURCE_H__NL__
