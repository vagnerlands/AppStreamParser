#ifndef _CWINSOCKET_H_
#define _CWINSOCKET_H_

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include "CCommonTypes.h"
#include "ISocket.h"

#include "CAnsiString.h"
#include "CCommonTypes.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

using namespace Types;

class CWinSocket : public ISocket {
public:

	CWinSocket();
	~CWinSocket();
	// Returns the status of a socket connection
	// 0 succeeded;
	// any other value is fail
	bool startListenSocket();
	
	// return value 0 is okay
	// return value isn't 0 - connection problem, delete this socket
	//  to avoid memory leak
	// content of buffer till it's size will be copied
	TInt32 write(const TByte* buffer, TInt32 const size);

	// return value 0 is okay
	// return value isn't 0 - connection problem, delete this socket
	//  to avoid memory leak
	// the buffer will be copied to Byte* pointer
	TInt32 read(TByte buffer[], TUInt32 size);

private:
	SOCKET m_fd;
	sockaddr_in m_sAddr, m_incmsgAddr;
	// input buffer for commands
	char m_readBuffer[Types::s_MAX_BUFFER_SIZE];
	// read buffer last index
	TInt32 m_rbli;
};


#endif //_CWINSOCKET_H_
