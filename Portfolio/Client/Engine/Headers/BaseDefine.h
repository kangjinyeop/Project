#ifndef BASE_DEFINE_H__
#define BASE_DEFINE_H__

typedef unsigned char		_UBYTE;
typedef signed char			_CHAR;
// Windows������ 4����Ʈ
// UNIX, Linux������ 2����Ʈ
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

// Windows������ 8����Ʈ
// UNIX, Linux������ 16����Ʈ
typedef long double			_LGDOUBLE;

typedef bool				_BOOL;

#endif // !BASE_DEFINE_H__