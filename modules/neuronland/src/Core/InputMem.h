#ifndef __CORE_INPUTMEM_H__NL__
#define __CORE_INPUTMEM_H__NL__

#include "Core/Input.h"

class InputMem : public Input
{
	u8		*mpStart;
	u8		*mpCurrent;
	u8		*mpEnd;
	bool	mbDelete;

public:
	InputMem(const void *memory, int length, bool delete_when_finished=false);
	InputMem(Input *);
	virtual ~InputMem();

	void	read(void*, int length);
	Size	seek(Offset offset, Input::SeekMode mode);
	Size	remaining() const;

	void	Relocate();
	String	GetName() const;

};

inline Input::Size InputMem::remaining() const
{
	return (Input::Size)( mpEnd - mpCurrent);
}

#include "InputSource.h"

class InputMemSource: private InputSource
{
	const void *m_memory;
	int			mLength;
	String	m_filename;

public:
	InputMemSource(const String& filename, const void *memory, int length, const String& identifier="mem");
	~InputMemSource();

	Input* openInput(const String&);

	String toString() const;
};

#endif // __CORE_INPUTMEM_H__NL__
