#ifndef __CORE_TYPES_H__NL__
#define __CORE_TYPES_H__NL__

#include <xmmintrin.h>

typedef __m128				u128;

#if defined (__linux__)
#include <inttypes.h>
typedef uint64_t	u64;
typedef int64_t	s64;
#else
typedef unsigned __int64	u64;
typedef signed __int64		s64;
#endif
typedef unsigned int		u32;
typedef signed int			s32;
typedef unsigned short		u16;

typedef signed short		s16;
typedef unsigned char		u8;
typedef signed char			s8;

/// Macro to delete a null out a pointer
 // template <class T> inline void erase(T *&a) { delete a; a=0; }

#endif // __CORE_TYPES_H__NL__
