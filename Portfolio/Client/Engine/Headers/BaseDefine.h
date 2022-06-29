#ifndef BASE_DEFINE_H__
#define BASE_DEFINE_H__

typedef unsigned char		_UBYTE;
typedef signed char			_CHAR;
// Windows에서는 4바이트
// UNIX, Linux에서는 2바이트
typedef wchar_t				_TCHAR;
typedef unsigned short		_UWORD;
typedef signed short		_SHORT;
typedef unsigned int		_UINT;
typedef signed int			_INT;
typedef unsigned long		_ULONG;
typedef signed long			_LONG;
typedef unsigned long long	_ULONGLONG;
typedef signed long long	_LONGLONG;

typedef float				_FLOAT;
typedef double				_DOUBLE;

// Windows에서는 8바이트
// UNIX, Linux에서는 16바이트
typedef long double			_LGDOUBLE;

typedef bool				_BOOL;

#endif // !BASE_DEFINE_H__