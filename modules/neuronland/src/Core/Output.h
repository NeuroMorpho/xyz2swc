#ifndef __CORE_OUTPUT_H__NL__
#define __CORE_OUTPUT_H__NL__

#include <stddef.h>

#include "Core/String.hpp"
#include "Core/IoBase.h"
#include "Core/FileIO.h"

class OutputSource;

class Output: public IoBase
{
public:

	static Output *open		(const String& url);
	static Output *overwrite(const String& url);
	static Output *append	(const String& url);

	static bool	   save(const String& fname, const u8 *data, int length);

	virtual void	write		(const void *,  int length)/* = 0;*/ {}
	void			writeString	(const String&);
	void			writeStringNoZero( const String& );
	void			writeLine	(const String&, bool crlf=false);
	
	virtual void		Flush(void) /*= 0;*/ {}
	virtual Size		Seek(Offset offset, SeekMode origin) const;
	virtual String		GetName(void) const/* = 0;*/ { return String(); }
	OutputSource*		GetSource(void) const;

	template <class T> inline void write(const T &t)
	{
		write(&t, sizeof(t));
	}
	
	bool	HasFailed() const;
	Size	GetBytesWritten() const;
	void	ClearFailed();

	Output(OutputSource *);
	virtual ~Output();

protected:
	bool			mFailed;
	Size			mBytesWritten;
	OutputSource *	mpSource;
};

inline bool Output::HasFailed(void) const
{
	return mFailed;
}

inline void Output::ClearFailed(void)
{
	mFailed = false;
}

inline Output::Size Output::GetBytesWritten(void) const
{
	return mBytesWritten;
}

#endif // __CORE_OUTPUT_H__NL__
