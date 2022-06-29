#ifndef TextParser_H__
#define TextParser_H__

#include <stdio.h>
#include <Windows.h>

class CTextParser
{
private:
#define NULL_TEXT 1
#define NEW_NINE 1
#define COMMAND 2
public:
	typedef enum PARSER_CODE
	{
		PARSER_FILE_ERROR		= 0,
		PARSER_NOT_FIND_OBJECT	= 1,
		PARSER_NOT_FIND_DATA	= 2,
		PARSER_NOT_FIND_STRING	= 3,
		PARSER_END,
	}PARSER_CODE;

	typedef enum DATA_TYPE
	{
		TYPE_BASE		= 0,
		TYPE_int		= TYPE_BASE,
		TYPE_long		= TYPE_BASE + 1,
		TYPE_longlong	= TYPE_BASE + 2,
		TYPE_float		= TYPE_BASE + 3,
		TYPE_double		= TYPE_BASE + 4,
		TYPE_char		= TYPE_BASE + 5,
		TYPE_BYTE		= TYPE_BASE + 6,
		TYPE_short		= TYPE_BASE + 7,		
		TYPE_WORD		= TYPE_BASE + 8,
		TYPE_UINT		= TYPE_BASE + 9,
		TYPE_DWORD		= TYPE_BASE + 10,
		TYPE_Ulonglong	= TYPE_BASE + 11,
		TYPE_END		= 99,
	}DATA_TYPE;
public:
	CTextParser();
	~CTextParser();
public:
	PARSER_CODE	GetError();
public:
	bool		LoadFile(const TCHAR* PathFile);
public:	
	void		ObjectData(const TCHAR* Object, const TCHAR* SelectName, void* OutData, DATA_TYPE Type);
	void		ObjectString(const TCHAR* Object, const TCHAR* SelectName, TCHAR** OutString);
private:
	void		SelectObject(const TCHAR* Object);
	void		RemoveCommond();
	void		RemoveSpace();
private:
	void		TextToRealNumber(const char* Text, void* OutData, DATA_TYPE Type);
private:
	void		Release();
private:
	PARSER_CODE	_Error;
	char*		_Buffer;
	char*		_ObjectBuffer;
	int			_MaxSize;
};

#endif // !TextParser_H__