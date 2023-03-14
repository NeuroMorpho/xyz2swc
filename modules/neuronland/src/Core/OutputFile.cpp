//	OutputFile.cpp

#include "Core/OutputFile.h"
#include "Core/Debug.h"

OutputFile::Source::Source(const String& name, const String& prefix, bool toBackslash, bool toUpper, const String& suffix):
	OutputSource(name),
	m_prefix(prefix),
	m_suffix(suffix),
	m_toUpper(toUpper),
	m_toBackslash(toBackslash)
{
}
    
OutputFile::Source::~Source()
{
}

Output *OutputFile::Source::openOutput(const String& name)
{
	String fullname(m_prefix+name);
	__handle handle=__open(fullname.c_str(), _O_WRONLY_CREAT_TRUNC);

	if (!__valid_handle(handle))
		return 0;
	else
		return new OutputFile(name, handle, this);
}

Output *OutputFile::Source::overwriteOutput(const String& name)
{
	__handle handle=__open( (m_prefix+name).c_str(), _O_WRONLY_TRUNC);

	if (!__valid_handle(handle))
		return 0;
	else
		return new OutputFile(name, handle, this);
}

Output *OutputFile::Source::appendOutput(const String& name)
{
	__handle handle=__open( (m_prefix+name).c_str(), _O_WRONLY_APPEND);

	if (!__valid_handle(handle))
		return 0;
	else
		return new OutputFile(name, handle, this);
}

OutputFile::OutputFile(const String &name, __handle handle, OutputSource *pSource)
:
	Output(pSource)
{
	assert(__valid_handle(handle));

	mName = name;
	mHandle = handle;
	mBytesWritten = 0;
}

void OutputFile::write(const void *data, int length)
{
#if defined(__linux__)
	__writeX(mHandle, data, length);
#else
	__write(mHandle, data, length);
#endif
	mBytesWritten += length;
}

void OutputFile::write(u8 ch)
{
#if defined(__linux__)
	__writeX(mHandle, &ch, 1);
#else
	__write(mHandle, &ch, 1);
#endif
	++mBytesWritten;
}

Output::Size OutputFile::Seek(Offset offset, SeekMode origin) const
{
	return __seek(mHandle, offset, origin);
}

void OutputFile::Flush(void)
{
#if !defined(_OSX) && !defined(_LINUX)
	__flush(mHandle);
#endif
}

OutputFile::~OutputFile()
{
	__close(mHandle);
}

String OutputFile::GetName(void) const
{
	return mName;
}

__handle OutputFile::GetHandle(void) const
{
	return mHandle;
}
