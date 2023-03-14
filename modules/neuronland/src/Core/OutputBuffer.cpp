//	OutputBuffer.cpp

#include <algorithm>

#include "Core/OutputBuffer.h"
#include "Core/String.hpp"

OutputBuffer::OutputBuffer(int length, Output *destination, bool allow_fallthrough)
:
	Output(destination->GetSource())
{
	mpBuffer = new u8[length];
	mLength=length;
	mPosition=0;
	m_pOutput=destination;
	m_fallthrough=allow_fallthrough;
}

void OutputBuffer::write(const void *data, int length)
{
	if (!m_pOutput)
		return;

	const u8 *from=(const u8 *) data;
	while (!mFailed && length > 0)
	{
		if (mPosition == 0 && length>mLength && m_fallthrough)
		{
			m_pOutput->write(from, length);
			mFailed=m_pOutput->HasFailed();
			length=0;
		}
		else
		{
			int room=mLength-mPosition;
			int tocopy=std::min(length, room);

			memcpy(mpBuffer+mPosition,from,tocopy);
			mPosition += tocopy;
			length -= tocopy;
			from += tocopy;

			if (mPosition==mLength)
			{
				Flush();
			}
		}
	}
}

OutputBuffer::~OutputBuffer()
{
	Flush();
	delete [] mpBuffer;
	delete m_pOutput;
}

void OutputBuffer::Flush()
{
	if (m_pOutput)
	{
		m_pOutput->write(mpBuffer, mPosition);
		m_pOutput->Flush();
		mFailed=m_pOutput->HasFailed();
		mPosition=0;
	}
}

String OutputBuffer::GetName() const
{
	return m_pOutput->GetName();
}
