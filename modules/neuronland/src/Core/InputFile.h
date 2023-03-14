#ifndef __CORE_INPUTFILE_H__NL__
#define __CORE_INPUTFILE_H__NL__

#include "Core/Input.h"
#include "Core/InputSource.h"
#include "Core/FileIO.h"

class InputFile: public Input
{
	String		mName;
	__handle	mHandle;
	Size		mLength;
	Size		mOffset;

public:
	InputFile(const String& name, __handle handle, Size length, InputSource* pSource);
	virtual ~InputFile();

	void	read(void*, int length);
	Size	seek(Offset offset, Input::SeekMode mode);
	Size	remaining() const;
	int		totalBytesRead() const;

	String			GetName() const;
	InputSource*	GetSource() const;

	class Source: public InputSource
	{
		String	m_prefix;
		String	m_suffix;
		bool	m_toUpper;
		bool	m_toBackslash;

	public:
		Source(const String& name, const String& prefix, bool toBackslash, bool toUpper=false, bool front=false, const String& suffix="");
		Source(const String& name, const String& directory);

        virtual ~Source();

		Input *openInput(const String& name);
		
		virtual String toString() const;
	};
};

///////////////////////////////////////////////////////////////
////
////	Inlines

inline int InputFile::totalBytesRead() const
{
	return mBytesRead;
}

inline Input::Size InputFile::remaining() const
{
	return mLength - mOffset;
}

inline String InputFile::GetName() const
{
	return mName;
}

#endif // __CORE_INPUTFILE_H__NL__
