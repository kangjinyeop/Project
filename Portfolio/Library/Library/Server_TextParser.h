#ifndef SERVER_TEXTPARSER_H__
#define SERVER_TEXTPARSER_H__

#include <stdio.h>
#include <ctime>

#include "Server_CrashDump.h"

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
		PARSER_NOT_FIND_OBJECT	= PARSER_FILE_ERROR + 1,
		PARSER_NOT_FORM			= PARSER_FILE_ERROR + 2,
		PARSER_NOT_FIND_DATA	= PARSER_FILE_ERROR + 3,
		PARSER_NOT_FIND_STRING	= PARSER_FILE_ERROR + 4,
		PARSER_END				= PARSER_FILE_ERROR + 5,
	}PARSER_CODE;

	typedef enum DATA_TYPE
	{
		TYPE_BASE		= 0,
		TYPE_INT		= TYPE_BASE,
		TYPE_LONG		= TYPE_BASE + 1,
		TYPE_LONGLONG	= TYPE_BASE + 2,
		TYPE_FLOAT		= TYPE_BASE + 3,
		TYPE_DOUBLE		= TYPE_BASE + 4,
		TYPE_CHAR		= TYPE_BASE + 5,
		TYPE_BYTE		= TYPE_BASE + 6,
		TYPE_SHORT		= TYPE_BASE + 7,
		TYPE_WORD		= TYPE_BASE + 8,
		TYPE_UINT		= TYPE_BASE + 9,
		TYPE_DWORD		= TYPE_BASE + 10,
		TYPE_ULONGLONG	= TYPE_BASE + 11,
		TYPE_END		= TYPE_BASE + 12,
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
	char*		UnicodeToMultiByte(const TCHAR* Data, int TextLength);
	void		TextToRealNumber(const char* Text, void* OutData, DATA_TYPE Type);
private:
	void		Release();
private:
	PARSER_CODE	_Error;
	char*		_Buffer;
	char*		_ObjectBuffer;
	int			_MaxSize;
};

#endif // !SERVER_TEXTPARSER_H__