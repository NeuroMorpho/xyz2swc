#ifndef __CORE_INPUT_H__NL__
#define __CORE_INPUT_H__NL__

#include <stddef.h>

#include "Core/Types.h"
#include "Core/String.hpp"
#include "Core/IoBase.h"


class InputSource;

class Input : public IoBase
{
public:
	static bool		exists(const String& url);
	static Input *	open(const String& url);
	static void *	load(const String& url);
	static void *	load(const String& url, int &length);

	u8				read();
	virtual void 	read(void*,  int length) /*=0;*/{}
	String			readLine();
	String			readString();
	int				readInt();
	float			readFloat();
	void			rollback();

	virtual Size	seek(Offset offset, SeekMode mode = Relative) /*=0;*/ { return 0; }
	virtual Size	remaining() const { return 0; }

	virtual String	GetName() const { return String(); }
	InputSource*	GetSource() const;

	void			ToStart();

					Input(InputSource*);
	virtual			~Input();

	template <class T> void read(T &t)
	{
		read(&t, sizeof(t));
	}

	bool	didFail();

protected:
	int		mBytesRead;
	bool	mbFailed;

	void	clearFailFlag();

	static void Callback(const class InputSource *, const class Input *, const String&); /*__attribute__((weak));*/

protected:
	InputSource*	 mpSource;
};

inline int Input::readInt()
{
	int i; Input::read(i); return i;
}

inline float Input::readFloat()
{
	float f; read(f); return f;
}

inline u8 Input::read()
{
	u8 ch=0;
	read(&ch, 1);
	return ch;
}

inline InputSource *Input::GetSource() const
{
	return mpSource;
}

inline bool Input::didFail()
{
	return mbFailed;
}

inline void Input::clearFailFlag()
{
	mbFailed = false;
}

inline void Input::ToStart()
{
	seek(0, IoBase::Absolute);
}

extern unsigned g_bytesRead;

#endif	// __CORE_INPUT_H__NL__
