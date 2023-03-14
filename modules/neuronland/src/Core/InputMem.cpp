//	InputMem.cpp

#include "Core/InputMem.h"
#include "Core/Debug.h"
#include "Core/StringFormatting.h"

InputMem::InputMem(const void *memory, int length, bool delete_when_finished/* =false */)
:
	Input(0),
	mbDelete(delete_when_finished)
{
	mpStart = (u8*) memory;
	mpCurrent = mpStart;
	mpEnd = mpStart + length;
}

InputMem::InputMem(Input *fp)
:
	Input(fp->GetSource())
{
	int length = fp->remaining();
	mpStart = new u8[length];
	fp->read(mpStart, length);
	delete fp;

	mbFailed = Input::didFail();

	mpCurrent = mpStart;
	mpEnd = mpStart + length;
	mbDelete = true;
}

InputMem::~InputMem()
{
	if (mbDelete)
		delete[] mpStart;
}

void InputMem::Relocate()
{
	int length = mpEnd-mpStart;
	int offset = mpCurrent-mpStart;
	u8 *newStart = new u8[length];
	memcpy(newStart, mpStart, length);
	delete[] mpStart;
	mpStart = newStart;
	mpCurrent = mpStart + offset;
	mpEnd = mpStart + length;
}

String InputMem::GetName() const
{
	return "InputMem(" +hex(mpStart) + "-" + hex(mpEnd) + ")";
}

Input::Size InputMem::seek(Offset offset, SeekMode mode)
{
	if (mode == Relative)
		offset += mpCurrent-mpStart;
	else if (mode == FromEnd)
		offset = mpEnd-mpStart-offset;

	mpCurrent = mpStart + offset;

	return offset;
}

void InputMem::read(void *dest, int length)
{
	if (mbFailed)
		return;
	
	assert((Size) length<=remaining());
	memcpy(dest, mpCurrent, length);
	mpCurrent+=length;
}

InputMemSource::InputMemSource(const String& filename, const void *memory, int length, const String& identifier): InputSource(identifier, true)
{
	m_filename = filename;
	m_memory = memory;
	mLength = length;
}

InputMemSource::~InputMemSource()
{
}

Input* InputMemSource::openInput(const String& name)
{
	if (name == m_filename)
	{
		return new InputMem(m_memory, mLength);
	}
	else
	{
		return 0;
	}
}

String InputMemSource::toString() const
{
	return "InputMemSource (@ " +hex(m_memory) + ", length " + dec(mLength) + ")";
}
