//
//	OutputMem.cpp
//

#include "Core/OutputMem.h"
#include "Core/Debug.h"
#include "Core/StringFormatting.h"

OutputMem::OutputMem(void *memory, int length)
:
	Output(0)
{
	mpStart = (u8 *)memory;
	mpCurrent = mpStart;
	mpEnd = mpStart + length;

	mBytesWritten = 0;
}

OutputMem::~OutputMem()
{
}

void OutputMem::write(const void *src, int length)
{
//	strong_assert(mpCurrent + length <= mpEnd);

	if (!HasFailed())
	{
		if (mpCurrent + length > mpEnd)
		{
			report("OutputMem: Warning: Memory buffer too small!");
			mFailed = true;
			return;
		}

		mBytesWritten += length;
		memcpy(mpCurrent, src, length);
		mpCurrent += length;
	}
}

String OutputMem::GetName() const
{
	return ("OutputMem (" +hex(mpStart) + "-" + hex(mpEnd) + ")");
}

u8 *OutputMem::GetMemoryPtr() const
{
	return mpStart;
}

void OutputMem::Flush()
{
}
