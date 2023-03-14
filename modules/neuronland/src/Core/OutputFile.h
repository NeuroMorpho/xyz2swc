#ifndef __CORE_OUTPUTFILE_H__NL__
#define __CORE_OUTPUTFILE_H__NL__

#include "Core/FileIO.h"
#include "Core/Output.h"
#include "Core/OutputSource.h"

class OutputFile: public Output
{
	String mName;
	__handle mHandle;

public:
	OutputFile(const String &name, __handle handle, OutputSource *pSource);
	virtual ~OutputFile();

	void		write(u8);
	void		write(const void *, int length);

	void		Flush(void);
	Size		Seek(Offset offset, SeekMode origin) const;
	String		GetName(void) const;
	__handle	GetHandle(void) const;

	class Source: public OutputSource
	{
		String	m_prefix;
		String	m_suffix;
		bool	m_toUpper;
		bool	m_toBackslash;

	public:
		Source(const String& name, const String& prefix, bool toBackslash, bool toUpper, const String& suffix="");
        virtual ~Source();

		Output *openOutput(const String& name);
		Output *overwriteOutput(const String& name);
		Output *appendOutput(const String& name);
	};
};


#endif // __CORE_OUTPUTFILE_H__NL__
