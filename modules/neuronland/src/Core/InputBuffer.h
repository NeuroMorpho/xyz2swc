#ifndef __CORE_INPUTBUFFER_H__NL__
#define __CORE_INPUTBUFFER_H__NL__

#include "Core/Input.h"

class InputBuffer: public Input
{
	u8*				mpBuffer;
	int				mBufferPos;
	int				mBufferLen;
	Size 			mPosition;

	Input*			mpInput;
	Size			mLength;
	Size			mSize;

	void			fillBuffer();
	
public:
					InputBuffer(int length, Input *src);
					InputBuffer(void *memory, int length, Input *src);
	virtual			~InputBuffer();

	void			read(void *, int length);
	IoBase::Size	seek(IoBase::Offset offset, IoBase::SeekMode mode);
	String			readLine();
	void			rewindLine(const String& str);
	Size			remaining() const;

	String			GetName() const;
};

#endif // __CORE_INPUTBUFFER_H__NL__
