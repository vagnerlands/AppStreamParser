#include "CWinSocket.h"

CWinSocket::CWinSocket()
{
	m_recycle = false;
}

CWinSocket::~CWinSocket()
{

}

bool
CWinSocket::startListenSocket()
{

	// temporary data for connection
	// -----
	int iResult;
	socklen_t addrlen;
	// -----

	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);

	m_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_fd == -1) {
		fprintf(stderr, "socket failed [%s]\n", strerror(errno));
		return -1;
	}
	printf("Created a socket with fd: %d\n", m_fd);
	m_sAddr.sin_family = AF_INET;
	m_sAddr.sin_port = htons(s_GAME_CONNECTION_PORT);
	m_sAddr.sin_addr.s_addr = INADDR_ANY;

	/* Step2: bind the socket */
	iResult = bind(m_fd, (struct sockaddr *)&m_sAddr, sizeof(struct sockaddr_in));
	if (iResult != 0) {
		fprintf(stderr, "bind failed [%s]\n", strerror(errno));
		exit(1);
		return false;
	}

	return true;

}

TInt32 
CWinSocket::write(const TByte* buffer, TInt32 const size)
{
	TInt32 iResult = 0;
	//write(1,"Received a datagram: ", 21);
	m_incmsgAddr.sin_family = AF_INET;
	m_incmsgAddr.sin_port = htons(s_GAME_CONNECTION_PORT);
	m_incmsgAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	TInt32 addrSize = sizeof(sockaddr_in);
	iResult = sendto(m_fd, "Got your message\n", 17, 0, (struct sockaddr *)&m_incmsgAddr, addrSize);
	// return status of the sent message
	return iResult;
}


// incoming buffer
// size in bytes
TInt32
CWinSocket::read(TByte buffer[], TUInt32 size)
{
	TInt32 addrSize = sizeof(sockaddr_in);
	int iResult = recvfrom(m_fd, &*buffer, s_MAX_BUFFER_SIZE, 0, (struct sockaddr *)&m_incmsgAddr, &addrSize);
	if (iResult >= 0)
	{
		//printf("Received a datagram: %s", buffer);
		return iResult;
	}

	return -1;
}


