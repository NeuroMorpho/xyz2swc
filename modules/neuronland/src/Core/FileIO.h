#ifndef __CORE_FILEIO_H__NL__
#define __CORE_FILEIO_H__NL__

#if defined(_WIN32)

#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define __handle int
#define __valid_handle(h)	(h >= 0)

#define __open(a,b) ::_open(a, (b)|_O_BINARY, _S_IREAD | _S_IWRITE)
#define __close ::_close
#define __read ::_read
#define __write ::_write
#define __seek ::_lseek
#define __flush ::_commit

#define	__SEEK_SET	SEEK_SET
#define __SEEK_CUR	SEEK_CUR
#define __SEEK_END	SEEK_END

#define UNIX_STYLE 1

//////////////////////////////////////////////

#elif defined(__linux__) || defined(_OSX)

#include <fcntl.h>
#include <unistd.h>

#define PERMIT_RW_ALL (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

#define __handle int
#define __valid_handle(h)	(h >= 0)

#if defined(_OSX) || defined(__linux__)
#define _O_BINARY 0
#endif

#define __open(a,b) ::open(a,(b)|_O_BINARY,PERMIT_RW_ALL)
#define __close ::close
#define __read ::read
#define __writeX ::write
#define __seek ::lseek
#define __flush(...)	/*::commit*/

#define __SEEK_SET	SEEK_SET
#define __SEEK_CUR	SEEK_CUR
#define __SEEK_END	SEEK_END

#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_RDWR O_RDWR

#define _O_APPEND O_APPEND
#define _O_CREAT O_CREAT
#define _O_TRUNC O_TRUNC

#define UNIX_STYLE 1
//////////////////////////////////////////////

#else

// Just use stdlib stuff
#include <stdio.h>

#define __handle FILE*
#define __valid_handle(h)	(h != NULL)

#define __open(a,b) ::fopen(a,b)
#define __close ::fclose
#define __read(h,p,l) ::fread(p,1,l,h)
#define __write(h,p,l) ::fwrite(p,1,l,h)
#define __seek __seekAndTell
#define	__flush	::fflush

#define _O_RDONLY "r"
#define _O_WRONLY "w"
#define _O_RDWR "rw"

#define	__SEEK_SET	SEEK_SET
#define __SEEK_CUR	SEEK_CUR
#define __SEEK_END	SEEK_END

#define UNIX_STYLE 0

inline size_t   __seekAndTell( __handle handle, int offset, int whence )			{ ::fseek( handle, offset, whence); return ftell(handle); }

#endif

//////////////////////////////////////////////

#if UNIX_STYLE
#define _O_WRONLY_TRUNC			(_O_WRONLY|_O_TRUNC)
#define _O_WRONLY_CREAT_TRUNC	(_O_WRONLY|_O_CREAT|_O_TRUNC)
#define _O_WRONLY_APPEND		(_O_WRONLY|_O_APPEND)
#else
#define _O_WRONLY_TRUNC			("r+")
#define _O_WRONLY_CREAT_TRUNC	("w")
#define _O_WRONLY_APPEND		(_O_WRONLY "a")
#endif

#endif	// __CORE_FILEIO_H__NL__
