#ifndef __CORE_OUTPUTBUFFER_H__NL__
#define __CORE_OUTPUTBUFFER_H__NL__

#include "Core/Output.h"

class OutputBuffer: public Output
{
	u8 *	mpBuffer;
	int		mPosition;
	int		mLength;
	Output *m_pOutput;
	bool	m_fallthrough;
	
public:
	OutputBuffer(int length, Output *destination, bool allow_fallthrough=true);
	virtual ~OutputBuffer();

	void		write(const void *, int length);

	void			Flush();
	//int				Seek(int offset, int origin);	///< Seek within the file
	String			GetName() const;
	OutputSource*	GetSource() const;
};

#endif // __CORE_OUTPUTBUFFER_H__NL__
