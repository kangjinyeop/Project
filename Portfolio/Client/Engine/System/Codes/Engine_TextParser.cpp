#include "stdafx.h"
#include "Engine_TextParser.h"

USING(Engine)

CTextParser::CTextParser()
	:	_Error(PARSER_END), _Buffer(nullptr), _ObjectBuffer(nullptr),
		_MaxSize(0)
{
}

CTextParser::~CTextParser()
{
	Release();
}

CTextParser::PARSER_CODE CTextParser::GetError()
{
	return _Error;
}

bool CTextParser::LoadFile(const TCHAR * PathFile)
{
	if (nullptr != _Buffer)
	{
		delete[] _Buffer;
		_Buffer = nullptr;
	}

	FILE* FilePointer = nullptr;

	_wfopen_s(&FilePointer, PathFile, L"r");

	if (nullptr == FilePointer)
	{
		_Error = PARSER_FILE_ERROR;

		return false;
	}

	fseek(FilePointer, 0, SEEK_END);
	_MaxSize = ftell(FilePointer);

	fseek(FilePointer, 0, SEEK_SET);

	_Buffer = new char[_MaxSize];

	fread(_Buffer, sizeof(char) * _MaxSize, 1, FilePointer);

	fclose(FilePointer);

	// 주석 제거
	RemoveCommond();

	// 공백 제거
	RemoveSpace();

	return true;
}

void CTextParser::ObjectData(const TCHAR* Object, const TCHAR * SelectName, void * OutData, DATA_TYPE Type)
{
	if (nullptr == _ObjectBuffer)
	{
		SelectObject(Object);
	}

	int NameLength = static_cast<int>(wcslen(SelectName));

	char* MutiByteConvertName = UnicodeToMultiByte(SelectName, NameLength);

	char* DataStartPointer = nullptr;

	int StartIndex = 0;
	
	bool Result = false;

	for (; StartIndex < _MaxSize; ++StartIndex)
	{
		DataStartPointer = strstr(&_ObjectBuffer[StartIndex], MutiByteConvertName);

		if (0x3A == DataStartPointer[NameLength])
		{
			Result = true;
			break;
		}
	}

	if (false == Result)
	{
		_Error = PARSER_NOT_FIND_DATA;
		CCrashDump::Crash();
	}

	int EndIndex = 0;

	for (EndIndex = StartIndex; EndIndex < _MaxSize; ++EndIndex)
	{
		if (0x2C == DataStartPointer[EndIndex])
		{
			break;
		}
	}

	int DataLength = EndIndex - (NameLength + StartIndex + 1);

	char* Data = new char[DataLength + NULL_TEXT];
	Data[DataLength] = '\0';

	memcpy(Data, &DataStartPointer[NameLength + 1], sizeof(char) * DataLength);

	TextToRealNumber(Data, OutData, Type);

	delete[] MutiByteConvertName;
	MutiByteConvertName = nullptr;

	delete[] Data;
	Data = nullptr;
}

void CTextParser::ObjectString(const TCHAR* Object, const TCHAR * SelectName, TCHAR** OutString)
{
	if (nullptr == _ObjectBuffer)
	{
		SelectObject(Object);
	}

	int NameLength = static_cast<int>(wcslen(SelectName));

	char* MutiByteConvertName = UnicodeToMultiByte(Object, NameLength);

	char* DataStartPointer = nullptr;

	int StartIndex = 0;
	int EndIndex = 0;
	bool Result = false;

	for (; StartIndex < _MaxSize; ++StartIndex)
	{
		DataStartPointer = strstr(&_ObjectBuffer[StartIndex], MutiByteConvertName);

		if (0x3A == DataStartPointer[NameLength])
		{
			Result = true;
			break;
		}
	}

	if (false == Result)
	{
		_Error = PARSER_NOT_FIND_STRING;
		CCrashDump::Crash();
	}

	for (EndIndex = StartIndex; EndIndex < _MaxSize; ++EndIndex)
	{
		if (0x2C == DataStartPointer[EndIndex])
		{
			break;
		}
	}

	int DataLength = EndIndex - (NameLength + StartIndex + 1);

	char* Data = new char[DataLength + NULL_TEXT];
	Data[DataLength] = '\0';

	memcpy(Data, &DataStartPointer[NameLength + 1], sizeof(char) * DataLength);

	TCHAR* TempOutString = new TCHAR[DataLength + NULL_TEXT];

	MultiByteToWideChar(CP_ACP, 0, Data, DataLength + NULL_TEXT, TempOutString, DataLength + NULL_TEXT);

	*OutString = TempOutString;
}

void CTextParser::SelectObject(const TCHAR * Object)
{
	/*
	1. Object 길이 얻기
	2. Object 길이 + 1(\0) 만큼 메모리 확보
	3. \0 추가
	4. UniCode에서 MutiByte로 변환
	*/
	int ObjectLength = static_cast<int>(wcslen(Object));

	char* MutiByteConvertObject = UnicodeToMultiByte(Object, ObjectLength);

	char* ObjectPointer = nullptr;

	int StartIndex = 0;
	int EndIndex = 0;
	bool Result = false;

	for (; StartIndex < _MaxSize; ++StartIndex)
	{
		/*
		1. Object가 시작되는 위치로 이동
		2. Object 이름과 데이터 이름이 중복될 경우를 대비해서
		   Object 위치 다음에 { 순서일 경우에만 종료한다.
		*/
		ObjectPointer = strstr(&_Buffer[StartIndex], MutiByteConvertObject);

		if (0x7B == ObjectPointer[ObjectLength])
		{
			Result = true;
			break;
		}
	}

	if (false == Result)
	{
		_Error = PARSER_NOT_FIND_OBJECT;
		CCrashDump::Crash();
	}

	for (EndIndex = StartIndex; EndIndex < _MaxSize; ++EndIndex)
	{
		if (0x7D == ObjectPointer[EndIndex])
		{
			break;
		}
	}

	int ObjectSize = EndIndex - (ObjectLength + StartIndex + 1);

	_ObjectBuffer = new char[ObjectSize + NULL_TEXT];
	_ObjectBuffer[ObjectSize] = '\0';

	memcpy(_ObjectBuffer, &ObjectPointer[ObjectLength + 1], sizeof(char) * ObjectSize);

	delete[] MutiByteConvertObject;
	MutiByteConvertObject = nullptr;
}

void CTextParser::RemoveCommond()
{
	for (int Index = 0; Index < _MaxSize; ++Index)
	{
		/*
		C 주석 검사
		*/ 
		// //
		if (0x2F == _Buffer[Index] && 0x2F == _Buffer[Index + 1])
		{
			for (int LocalIndex = 0; LocalIndex < _MaxSize; ++LocalIndex)
			{
				// \n 검사
				if (0x0A == _Buffer[LocalIndex])
				{
					memcpy(&_Buffer[Index], &_Buffer[LocalIndex + NEW_NINE], _MaxSize - LocalIndex - NEW_NINE);
					break;
				}
			}
		}

		/*
		C++ 주석 검사
		*/
		// /*
		if (0x2F == _Buffer[Index] && 0x2A == _Buffer[Index + 1])
		{
			for (int LocalIndex = 0; LocalIndex < _MaxSize; ++LocalIndex)
			{
				/*
				C++ 주석 끝 검사
				*/
				// */
				if (0x2A == _Buffer[LocalIndex] && 0x2F == _Buffer[LocalIndex + 1])
				{
					memcpy(&_Buffer[Index], &_Buffer[LocalIndex + COMMAND + NEW_NINE], _MaxSize - LocalIndex - COMMAND - NEW_NINE);
					break;
				}
			}
		}
	}
}

void CTextParser::RemoveSpace()
{
	int LocalSize = 0;

	for (int Index = 0; Index < _MaxSize; ++Index)
	{
		// 공백 " HT(\t) LF(\n)
		if (0x20 == _Buffer[Index] || 0x22 == _Buffer[Index] || 0x09 == _Buffer[Index] || 0x0A == _Buffer[Index])
		{
			continue;
		}

		_Buffer[LocalSize] = _Buffer[Index];

		++LocalSize;
	}
}

char * CTextParser::UnicodeToMultiByte(const TCHAR* Data, int TextLength)
{
	char* MutiByteConvertObject = new char[TextLength + NULL_TEXT];
	MutiByteConvertObject[TextLength] = '\0';
	WideCharToMultiByte(CP_ACP, 0, Data, -1, MutiByteConvertObject, TextLength, NULL, NULL);

	return MutiByteConvertObject;
}

void CTextParser::TextToRealNumber(const char* Text, void* OutData, DATA_TYPE Type)
{
	bool Flag = true;
	
	switch (Type)
	{
	case TYPE_int:		*static_cast<int*>(OutData) = atoi(Text); break;
	case TYPE_long:		*static_cast<long*>(OutData) = atol(Text); break;
	case TYPE_longlong: *static_cast<long long*>(OutData) = atoll(Text); break;
	case TYPE_float:	*static_cast<float*>(OutData) = (float)atof(Text); break;
	case TYPE_double:	*static_cast<double*>(OutData) = atof(Text); break;
	default: Flag = false; break;
	}
	 
	if (false == Flag)
	{
		int Size = 0;

		for (;;)
		{
			if ('\0' == Text[Size])
			{
				break;
			}

			++Size;
		}

		bool MinusFlag = false;

		if ('-' == Text[0])
		{
			MinusFlag = true;
		}

		unsigned long long PowerOfNumber = 1;

		for (int Index = 0; Index < Size; ++Index)
		{
			PowerOfNumber *= 10;
		}

		unsigned long long Data = 0;

		for (int Index = 0; Index < Size; ++Index)
		{
			Data += PowerOfNumber * (Text[Index] - '0');
			PowerOfNumber /= 10;
		}

		short LocalNumber = (short)Data;

		if (true == MinusFlag)
		{
			LocalNumber *= -1;
		}

		switch (Type)
		{
		case TYPE_char:			*static_cast<char*>(OutData) = (char)LocalNumber; break;
		case TYPE_BYTE:			*static_cast<BYTE*>(OutData) = (BYTE)Data; break;
		case TYPE_short:		*static_cast<short*>(OutData) = LocalNumber; break;
		case TYPE_WORD:			*static_cast<WORD*>(OutData) = (WORD)Data; break;
		case TYPE_UINT:			*static_cast<UINT*>(OutData) = (UINT)Data; break;
		case TYPE_DWORD:		*static_cast<DWORD*>(OutData) = (DWORD)Data; break;
		case TYPE_Ulonglong:	*static_cast<unsigned long long*>(OutData) = Data; break;
		}
	}
}

void CTextParser::Release()
{
	if (nullptr != _Buffer)
	{
		delete[] _Buffer;
		_Buffer = nullptr;
	}

	if (nullptr != _ObjectBuffer)
	{
		delete[] _ObjectBuffer;
		_ObjectBuffer = nullptr;
	}
}