#include "stdafx.h"
#include "LanServer.h"

CLanServer::CLanServer()
{
}

CLanServer::~CLanServer()
{
	Release();
}

unsigned int __stdcall CLanServer::AcceptThread(LPVOID Parameter)
{
	CLanServer* LanServer = static_cast<CLanServer*>(Parameter);

	for (;;)
	{
		if (SERVER_EIXT == LanServer->_ServerState)
		{
			break;
		}

		SOCKADDR_IN	SocketAddr;
		ZeroMemory(&SocketAddr, sizeof(SocketAddr));
		int			SocketSize = sizeof(SocketAddr);

		SOCKET Socket = accept(LanServer->_listen_socket, (SOCKADDR*)&SocketAddr, &SocketSize);

		if (INVALID_SOCKET == Socket)
		{
			continue;
		}

		if (LanServer->_MaxClient < LanServer->_AcceptCount)
		{
			CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"Accept Thread Session Max.txt", L"SessionMax : %d || Current Session Count : %d", LanServer->_MaxClient, LanServer->_AcceptCount);
			closesocket(Socket);
			continue;
		}

		CSession* Session = LanServer->FindSession();

		InetNtop(AF_INET, (IN_ADDR*)&SocketAddr.sin_addr.S_un, Session->_ClientInfo._IP, sizeof(TCHAR) * MAX_IP);

		DWORD Byte;

		WSAIoctl(Socket, SIO_KEEPALIVE_VALS, &LanServer->_Keepalive, sizeof(KEEPALIVE), &LanServer->_Keepalive, sizeof(KEEPALIVE), &Byte, NULL, NULL);

		Session->_ClientInfo._Socket = Socket;
		Session->_ClientInfo._Port = SocketAddr.sin_port;

		InterlockedIncrement(&LanServer->_AcceptFPS);
		++LanServer->_AcceptTotal;
	}

	return 0;
}

unsigned int __stdcall CLanServer::WorkerThread(LPVOID Parameter)
{
	CLanServer*					LanServer = static_cast<CLanServer*>(Parameter);

	DWORD						Transferred = 0;

	CSession*					Session = nullptr;

	CSession::NEWOVERLAPPED*	NewOverlapped = nullptr;

	for (;;)
	{
		if (SERVER_EIXT == LanServer->_ServerState)
		{
			break;
		}

		GetQueuedCompletionStatus(LanServer->_IOCPWorker, &Transferred, (PULONG_PTR)&Session, (LPOVERLAPPED*)&NewOverlapped, INFINITE);

		if (nullptr == NewOverlapped)
		{
			CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"Overlapped Nullpter.txt", L"SessionID : %d", Session->_SessionID);
			CCrashDump::Crash();
			break;
		}

		if (0 == Transferred)
		{
			//CLog::GetInstance()->CreateLogFile(L"Transferred.txt", L"Error", L"Transferred Socket : %ld | SessionIndex : %d | SessionID : %lld", Session->_ClientInfo._Socket, Session->_SessionIndex, Session->_SessionID);
			CCrashDump::Crash();
		}

		if (CSession::OVERLAPPED_RECV == NewOverlapped->_State)
		{
			Session->_RecvRingBuffer.MoveRear(Transferred);

			for (;;)
			{
				HEADER	NetworkHeader;
				memset(&NetworkHeader, 0, sizeof(HEADER));

				if (Session->_RecvRingBuffer.Get_UseSize() < sizeof(HEADER))
				{
					break;
				}

				Session->_RecvRingBuffer.Peek((BYTE*)&NetworkHeader, sizeof(HEADER));

				if (LanServer->_Packet_Code != NetworkHeader._Code)
				{
					//CLog::GetInstance()->CreateLogFile(L"PacketCode.txt", L"Error", L"Code Socket : %ld | SessionIndex : %d | SessionID : %lld", Session->_ClientInfo._Socket, Session->_SessionIndex, Session->_SessionID);
					CCrashDump::Crash();
				}

				Session->_RecvRingBuffer.MoveFront(sizeof(HEADER));

				if (NetworkHeader._Len > Session->_RecvRingBuffer.Get_UseSize())
				{
					Session->_RecvRingBuffer.BackFront(sizeof(HEADER));
					//CLog::GetInstance()->CreateLogFile(L"PayLoad.txt", L"Debug", L"PayLoad Socket : %ld | SessionIndex : %d | SessionID : %lld | PayLoad : %d", Session->_ClientInfo._Socket, Session->_SessionIndex, Session->_SessionID, Session->_RecvRigBuffer.Get_UseSize());
					break;
				}

				CStreamBuffer*	Buffer = SerializationBuffer.Alloc();

				Session->_RecvRingBuffer.Peek(Buffer->Get_WritePointer(), NetworkHeader._Len);

				Buffer->MoveWritePosition(NetworkHeader._Len);

				if (false == Buffer->Decode(LanServer->_PacketKey, NetworkHeader._RandomKey, NetworkHeader._CheckSum))
				{
					//CLog::GetInstance()->CreateLogFile(L"Decode.txt", L"Error", L"Decode Socket : %ld | SessionIndex : %d | SessionID : %lld", Session->_ClientInfo._Socket, Session->_SessionIndex, Session->_SessionID);

					break;
				}

				Session->_CompleteMessage.Enqueue(Buffer);
				Session->_RecvRingBuffer.MoveFront(NetworkHeader._Len);
				InterlockedIncrement(&LanServer->_RecvTPS);
			}

			LanServer->RecvPost(Session);
		}

		else if (CSession::OVERLAPPED_SEND == NewOverlapped->_State)
		{

		}
	}

	return 0;
}

unsigned int __stdcall CLanServer::SendThread(LPVOID Parameter)
{
	CLanServer*		LanServer = static_cast<CLanServer*>(Parameter);
	CSession*		Session = nullptr;
	DWORD			ClinetCount = Session->_ClientInfo;

	for (;;)
	{
		Sleep(SLEEP_TIME);

		if (SERVER_EIXT == MMOServer->_ServerState)
		{
			break;
		}

		for (DWORD ClinetIndex = 0; ClinetIndex < ClinetCount; ++ClinetIndex)
		{
			Session = MMOServer->_Session[ClinetIndex];

			if (AUTH == Session->_State || GAME == Session->_State)
			{
				if (false == InterlockedExchange(&Session->_SendFlag, SEND_TRUE))
				{
					DWORD	WSABufferCount = Session->_SendRigBuffer.Get_Size();

					if (0 >= WSABufferCount)
					{
						InterlockedExchange(&Session->_SendFlag, SEND_FALSE);
						continue;
					}

					if (WSABufferCount > MAX_WSASENDBUFFER)
					{
						WSABufferCount = MAX_WSASENDBUFFER;
					}

					WSABUF	WsaSendBuffer[MAX_WSASENDBUFFER];

					CStreamBuffer*	TempBuffer = nullptr;
					void*			TempPointer = nullptr;

					for (DWORD Index = 0; Index < WSABufferCount; ++Index)
					{
						Session->_SendRigBuffer.Peek(&TempBuffer, TempPointer);

						WsaSendBuffer[Index].buf = (char*)TempBuffer->Get_ReadPointer();
						WsaSendBuffer[Index].len = TempBuffer->Get_UseSize();

						++Session->_SendCount;

						++MMOServer->_SendTPS;
					}

					memset(&Session->_SendOverlapped, 0, sizeof(Session->_SendOverlapped));
					Session->_SendOverlapped._State = OVERLAPPED_SEND;

					DWORD	Byte = 0;

					InterlockedIncrement(&Session->_ReferenceCount);
					int Result = WSASend(Session->_ClientInfo._Socket, WsaSendBuffer, WSABufferCount, &Byte, NULL, &Session->_SendOverlapped._Overlapped, NULL);

					if (SOCKET_ERROR == Result)
					{
						int Error = WSAGetLastError();

						if (WSA_IO_PENDING != Error)
						{
							wprintf(L"Send Not IO_PENDING\n");

							if (0 == InterlockedDecrement(&Session->_ReferenceCount))
							{
								Session->_ReleaseFlag = true;
							}

							else
							{
								//shutdown(Session->_ClientInfo._Socket, SD_BOTH);
							}

							break;
						}

						if (WSAENOBUFS == Error)
						{
							CLog::GetInstance()->CreateLogFile(L"Send Non Paged Memory Full.txt", L"Error", L"Socket : %ld", Session->_ClientInfo._Socket);
							CCrashDump::Crash();
						}
					}
				}
			}
		}

		++MMOServer->_SendFPS;
	}

	*/

	return 0;
}

unsigned int __stdcall CLanServer::MonitorThread(LPVOID Parameter)
{
	CLanServer*		LanServer = static_cast<CLanServer*>(Parameter);

	for (;;)
	{
		Sleep(1000);

		wprintf(L"==============================================================================\n");

		wprintf(L"AcceptTotal : %lld | AcceptCount : %d | AcceptFPS : %d\n", LanServer->_AcceptTotal, LanServer->_AcceptCount, LanServer->_AcceptFPS);

		//wprintf(L"Serialization ChunkCount : %d\n", SerializationBuffer.Get_MemoryPoolChunkCount());

		wprintf(L"WorkerFPS : %d | RecvTPS : %d | SendFPS : %d | SendTPS : %d\n", LanServer->_WorkerFPS, LanServer->_RecvTPS, LanServer->_SendFPS, LanServer->_SendTPS);

		wprintf(L"==============================================================================\n");

		InterlockedExchange(&LanServer->_AcceptFPS, 0);
		InterlockedExchange(&LanServer->_WorkerFPS, 0);
		InterlockedExchange(&LanServer->_RecvTPS, 0);
		InterlockedExchange(&LanServer->_SendFPS, 0);
		InterlockedExchange(&LanServer->_SendTPS, 0);
	}

	return 0;
}

void CLanServer::ServerStart()
{
	DataParser(nullptr);
}

void CLanServer::MakeCheckSum(CStreamBuffer * Buffer)
{
}

void CLanServer::Encoding(BYTE* Buffer, HEADER Header)
{
	BYTE PData = 0;
	BYTE PrePData = 0;
	BYTE PreEData = 0;

	BYTE CheckSum = MakeCheckSum();
	int Size = _WritePosition;

	*(_PreBuffer + HEADER_MAX - 1) = CheckSum;

	/*
	P1 = D1 ^ (RK + 1);
	E1 = P1 ^ (K + 1);
	*/
	PData = *(_PreBuffer + HEADER_MAX - 1) ^ (Header._RandomKey + 1);
	*(_PreBuffer + HEADER_MAX - 1) = PData ^ (Header.FixKey + 1);

	PrePData = PData;
	PreEData = *(_PreBuffer + HEADER_MAX - 1);

	for (int Index = 0; Index < Size; ++Index)
	{
		/*
		Pn = Dn ^ (P(n - 1) + RK + n);
		En = Pn ^ (E(n - 1) + K + n);
		*/
		PData = *(_PreBuffer + Index + HEADER_MAX) ^ (PrePData + RandKey + Index + 2);
		*(_PreBuffer + Index + HEADER_MAX) = PData ^ (PreEData + FixKey + Index + 2);

		PrePData = PData;
		PreEData = *(_PreBuffer + Index + HEADER_MAX);
	}
}

void CLanServer::Decoding()
{
	BYTE PData = 0;	// P
	BYTE PrePData = 0;	// P(n - 1)
	BYTE PreEData = 0;	// E(n - 1)
	int Size = _WritePosition;

	/*
	P1 = E1 ^ (K + n);
	D1 = P1 ^ (RK + n);
	*/
	PData = CheckSum ^ (FixKey + 1);

	PreEData = CheckSum;

	*(_PreBuffer + HEADER_MAX - 1) = PData ^ (RandKey + 1);

	PrePData = PData;

	for (int Index = 0; Index < _WritePosition; ++Index)
	{
		/*
		Pn = En ^ (E(n - 1) + K + n);
		Dn = Pn ^ (P(n - 1) + RK + n);
		*/
		PData = *(_PreBuffer + Index + HEADER_MAX) ^ (PreEData + FixKey + Index + 2);

		PreEData = *(_PreBuffer + Index + HEADER_MAX);

		*(_PreBuffer + Index + HEADER_MAX) = PData ^ (PrePData + RandKey + Index + 2);

		PrePData = PData;
	}
}

void CLanServer::DataParser(const TCHAR * ParserName)
{
	CTextParser TextParser;

	TextParser.LoadFile(L"../Data/NetworkSetup.txt");

	TextParser.ObjectString(L"Network", L"IP", &_IP);

	TextParser.ObjectData(L"Network", L"Port", &_Port, CTextParser::TYPE_WORD);
	TextParser.ObjectData(L"Network", L"_WorkerThread", &_WorkerThreadCount, CTextParser::TYPE_DWORD);
	TextParser.ObjectData(L"Network", L"ActiveWorkerThread", &_ActiveWorkerThreadCount, CTextParser::TYPE_DWORD);	
	TextParser.ObjectData(L"Network", L"KeepAliveOnOff", &_Keepalive._OnOff, CTextParser::TYPE_DWORD);
	TextParser.ObjectData(L"Network", L"KeepAliveTime", &_Keepalive._Time, CTextParser::TYPE_DWORD);
	TextParser.ObjectData(L"Network", L"KeepAliveInterval", &_Keepalive._Interval, CTextParser::TYPE_DWORD);
	TextParser.ObjectData(L"Network", L"KeepAliveInterval", &_Keepalive._Interval, CTextParser::TYPE_DWORD);
	TextParser.ObjectData(L"Network", L"MaxClient", &_MaxClient, CTextParser::TYPE_DWORD);
	TextParser.ObjectData(L"Network", L"HeartBeat", &_HeartBeat, CTextParser::TYPE_DWORD);
	TextParser.ObjectData(L"Network", L"PacketCode", &_Packet_Code, CTextParser::TYPE_CHAR);
	TextParser.ObjectData(L"Network", L"PacketKey", &_PacketKey, CTextParser::TYPE_CHAR);
}

CSession* CLanServer::FindSession()
{
	CSession* SearchSession = nullptr;

	for (int Index = 0; Index < _MaxClient; ++Index)
	{
		if (false == (*_Session)->_UseFlag)
		{
			SearchSession = *_Session;
			break;
		}
	}

	return SearchSession;
}

void CLanServer::Release()
{
	if (nullptr != _IP)
	{
		delete[] _IP;
		_IP = nullptr;
	}

	CloseHandle(_AcceptHandle);

	for (DWORD WorkerIndex = 0; WorkerIndex < _WorkerThreadCount; ++WorkerIndex)
	{
		CloseHandle(_WorkerHandle[WorkerIndex]);
	}

	delete[] _WorkerHandle;
	_WorkerHandle = nullptr;

	CloseHandle(_SendHandle);

	CloseHandle(_MonitorHandle);

	CloseHandle(_IOCPWorker);

	closesocket(_listen_socket);

	for (int Index = 0; Index < _MaxClient; ++Index)
	{
		delete (*_Session);
		(*_Session) = nullptr;
	}
}