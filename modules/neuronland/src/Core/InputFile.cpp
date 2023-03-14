//	InputFile.cpp

#include "Core/InputFile.h"
#include "Core/Debug.h"


InputFile::Source::Source(const String& name, const String& directory):
	InputSource(name, false),
	m_prefix(directory),
	m_suffix(""),
	m_toUpper(false),
	m_toBackslash(true)
{
}

InputFile::Source::Source(const String& name, const String& prefix, bool toBackslash, bool toUpper, bool front, const String& suffix):
	InputSource(name, front),
	m_prefix(prefix),
	m_suffix(suffix),
	m_toUpper(toUpper),
	m_toBackslash(toBackslash)
{
}
    
InputFile::Source::~Source()
{
}

Input *InputFile::Source::openInput(const String& name)
{
	String fullname = m_prefix+name;

	__handle handle=__open((const char *)fullname.c_str(), _O_RDONLY);

	if (!__valid_handle(handle))
		return 0;
	else
	{
		int length= __seek(handle, 0, SEEK_END);
		
		__seek(handle, 0, SEEK_SET);
		return new InputFile(fullname, handle, length, this);
	}

}

String InputFile::Source::toString() const
{
	String name= "InputFile::Source ("+m_prefix+", "+m_suffix;
	if (m_toUpper)
		name+=", Upper";
	if (m_toBackslash)
		name+=", DOS \\";

	return name+")";
}

InputFile::InputFile(const String& name, __handle handle, Size length, InputSource* pSource)
	: Input(pSource)
	, mName(name)
	, mHandle(handle)
	, mLength(length)
	, mOffset(0)
{
	assert( __valid_handle(handle) );
	mBytesRead = 0;
}

void InputFile::read(void *dest, int length)
{
	int bytesRead = __read(mHandle, dest, length);

	assert( bytesRead == length );

	mOffset	+=bytesRead;
	g_bytesRead += bytesRead;
	mBytesRead += bytesRead;
}

Input::Size InputFile::seek(Offset offset, SeekMode mode)
{
	if (mode == Relative)
		offset += mOffset;
	else if (mode == FromEnd)
		offset = mLength-offset;

	mOffset=__seek(mHandle, offset, SEEK_SET);

	return mOffset;
}

InputFile::~InputFile()
{
	__close(mHandle);
}

