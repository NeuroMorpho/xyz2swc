#ifndef __CORE_IOBASE_H__NL__
#define __CORE_IOBASE_H__NL__

#include "Core/Types.h"

struct IoBase
{
	enum SeekMode
	{
		Absolute,
		Relative,
		FromEnd
	};

	typedef s64 Offset;
	typedef u64 Size;
};

#endif // __CORE_IOBASE_H__NL__
