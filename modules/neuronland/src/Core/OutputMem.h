#ifndef __CORE_OUTPUTMEM_H__NL__
#define __CORE_OUTPUTMEM_H__NL__

#include "Core/Output.h"

class OutputMem: public Output
{
	u8*		mpStart;
	u8*		mpCurrent;
	u8*		mpEnd;
	
public:
				OutputMem(void *memory, int length);
	virtual		~OutputMem();

	void		write(const void *, int length);
	inline void	write(u8 data) { write((const void*)&data, 1); }

	String		GetName() const;
	void		Flush();

	u8*			GetMemoryPtr() const;
};

#endif	// __CORE_OUTPUTMEM_H__NL__
