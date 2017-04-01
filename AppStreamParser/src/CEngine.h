#ifndef CENGINE_H_
#define CENGINE_H_

#include "CParserHandler.h"
#include "ISocket.h"
#include "IThread.h"
#include "IMutex.h"
#include "CCommonTypes.h"

using namespace Types;
using namespace std;

class CEngine 
{
public:
	static CEngine* instance();
	static void ThreadListener();
	void GetStream();
	void start();
	void run();
	~CEngine();
private:
	CEngine();
	static CEngine* m_pInstance;

	ISocket* m_socket;
	IThread* m_listeningThread;
	IMutex* m_bufferMutex;
	// message queue buffer
	MessageBufferQ m_messageQ;

	TByte m_incomingStream[s_MAX_BUFFER_SIZE];
	TInt32 m_bufLength;

};

#endif
