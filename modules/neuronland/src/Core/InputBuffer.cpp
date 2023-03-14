//	InputBuffer.cpp

#include <algorithm>

#include "Core/Debug.h"
#include "Core/InputBuffer.h"

InputBuffer::InputBuffer(int length, Input *src): Input(src->GetSource())
{
	mpInput=src;
	mpBuffer=new u8[length];
	mLength=length;
	mSize=mpInput->remaining();
	mPosition=0;
	fillBuffer();
}

InputBuffer::InputBuffer(void *memory, int length, Input *src): Input(src->GetSource())
{
	mpInput=src;
	mpBuffer=(u8*) memory;
	mLength=length;
	mSize=mpInput->remaining();
	mPosition=0;
	fillBuffer();
}

void InputBuffer::fillBuffer()
{
	if (mbFailed)
		return;
	
	mBufferLen = std::min(mpInput->remaining(),  mLength);
	mBufferPos = 0;
	mpInput->read(mpBuffer, mBufferLen);

	mbFailed = mpInput->didFail();
}

Input::Size InputBuffer::remaining() const
{
	assert(mpInput);
	return mBufferLen - mBufferPos + mpInput->remaining();
}

String InputBuffer::readLine()
{
	String line;

	char c;
	read(&c, 1);
	while(c != 0x0A)
	{
		line += c;
		read(&c, 1);
	}

	return line;
}

void InputBuffer::rewindLine(const String& str)
{
	seek(-(str.length() + 1), Relative);
}

void  InputBuffer::read(void *_dest, int length)
{
	if (mbFailed)
		return;

	assert((Size) length<=remaining());

	u8 *dest=(u8 *) _dest;
	while (length)
	{
		int chunk = std::min(mBufferLen-mBufferPos, length);

		memcpy(dest, mpBuffer+mBufferPos, chunk);
		
		length-=chunk;
		dest+=chunk;
		mBufferPos+=chunk;
		mPosition+=chunk;
		if (mBufferPos==mBufferLen)
		{
			fillBuffer();
			if (mbFailed)
				return;
		}
	}
}

IoBase::Size InputBuffer::seek(IoBase::Offset amount, IoBase::SeekMode mode)
{
	if (mbFailed)
	{
		return -1;
	}

	int offset;
	if (mode==Absolute)
	{
		offset = amount-mPosition;
	}
	else if (mode==FromEnd)
	{
		offset = (mSize-amount)-mPosition;
	}
	else
	{
		assert(mode==Relative);
		offset = amount;
	}

	assert(mPosition+offset>=0);
	assert(mPosition+offset<=mSize);

	if (offset<0)
	{
		if (mBufferPos >= -offset)
		{
			mBufferPos+=offset;
		}
		else
		{
			mpInput->seek(offset-(mBufferLen-mBufferPos), Relative);
			fillBuffer();
		}
	}
	else
	{
		int after_buf = offset-(mBufferLen-mBufferPos);
		if (after_buf < 0)
		{
			mBufferPos+=offset;
		}
		else
		{
			mpInput->seek(after_buf, Relative);
			fillBuffer();
		}
	}
	mPosition+=offset;

	return mPosition;
}

InputBuffer::~InputBuffer()
{
	delete mpInput;
	delete mpBuffer;
}

String InputBuffer::GetName() const
{
	return mpInput->GetName();
}
