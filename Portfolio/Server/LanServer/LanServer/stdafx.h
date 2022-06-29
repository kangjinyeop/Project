#ifndef PCH_H
#define PCH_H

// Winsock2.h 와 Windows.h의 순서 변경시 컴파일 오류
#include <WinSock2.h> 
#include <Windows.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <strsafe.h> // Stringxx 관련 함수

#endif //PCH_H