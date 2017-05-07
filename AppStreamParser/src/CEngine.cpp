#include "CEngine.h"
#include "CWinSocket.h"
#include "CWinThread.h"
#include "CWinMutex.h"

#define MAX_ENUMERATOR_SIZE 32
#define ENUMERATOR_STR_SIZE 50

CEngine* CEngine::m_pInstance = NULL;

CEngine::~CEngine()
{

}

CEngine::CEngine()
{
	memset(m_incomingStream, 0, s_MAX_BUFFER_SIZE);
	m_bufLength = -1;
}

CEngine* 
CEngine::instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CEngine();
	}
	return m_pInstance;
}

// static function - point access to the listenner thread
void 
CEngine::ThreadListener()
{
	while (true)
	{
		// listener is constantly calling GetStream()
		instance()->GetStream();
	}
}

void CEngine::GetStream()
{
	unsigned int size = 0;
	// gets a stream buffer- attention - blocking operation
	m_bufLength = m_socket->read(&m_incomingStream[0], size);
	// if a message was retrieved, this value shall be positive
	if (m_bufLength > 0)
	{
		//CParserHandler::instance()->sendStream(m_socket);
		// new item to be pushed into the queue
		SIncomingMsg newMsg;
		newMsg.m_length = m_bufLength;
		// creates a copy of the buffer stream
		memcpy(&newMsg.m_data, &m_incomingStream, s_MAX_BUFFER_SIZE);
		// critical area - pushing new item to the queue
		m_bufferMutex->mutexLock();
		// pushes to the front of the queue
		m_messageQ.push_front(newMsg);
		// end of critical area - pushing new item to the queue
		m_bufferMutex->mutexUnlock();

		memset(m_incomingStream, 0, s_MAX_BUFFER_SIZE);
		m_bufLength = -1;
	}
	
}

void CEngine::start()
{
	m_listeningThread = new CWinThread();
	m_socket = new CWinSocket();
	m_bufferMutex = new CWinMutex();
	// starts singletones - good practice
	CParserHandler::instance();

    // allocates bidimensional array of chars
    char** arr = new char *[MAX_ENUMERATOR_SIZE];
    // 30 is the longest list of enumerators found - AIS
    for (int iCnt = 0; iCnt < MAX_ENUMERATOR_SIZE; iCnt++)
    {
        arr[iCnt] = new char[ENUMERATOR_STR_SIZE];
        memset(arr[iCnt], 0, MAX_ENUMERATOR_SIZE);
    }

	CParserHandler::instance()->addStruct("structVagner", 123, EMESSAGEDIRECTION_Outgoing);
	// prepare the parser database
	CParserHandler::instance()->addField("structVagner", 123, "OpCode", "int", 0, 4, arr);
	CParserHandler::instance()->addField("structVagner", 123, "Length", "uint", 4, 4, arr);
	CParserHandler::instance()->addField("structVagner", 123, "Latitude", "float", 8, 4, arr);
	CParserHandler::instance()->addField("structVagner", 123, "Longitude", "float", 12, 4, arr);
	CParserHandler::instance()->addField("structVagner", 123, "Name1", "char", 16, 1, arr);
	CParserHandler::instance()->addField("structVagner", 123, "Name2", "char", 17, 1, arr);
	CParserHandler::instance()->addField("structVagner", 123, "Valid", "bool", 18, 4, arr);
    delete[] arr;
	// uses clear screen command - to clean whatever was printed so far...
	system("cls");
	// creates a buffer mutex to avoid concurrency on m_messageQ
	m_bufferMutex->createMutex("BufferMutex");
	// creates a thread for the listener - necessary since this is a blocking operation
	m_listeningThread->createThread("ThListenerUDP", instance()->ThreadListener);
	// runs the thread (listener)
	m_socket->startListenSocket();

	//CParserHandler::instance()->SetFieldValue("structVagner", "Longitude", "55.1234");
	
	// finally - call run
	run();
}

void CEngine::run()
{
	// runs until the application is halted
	while (true)
	{
		// critical area - operating with the m_messageQ
		m_bufferMutex->mutexLock();
		if (!m_messageQ.empty())
		{
			SIncomingMsg nextMsgToProcess;
			// processes the oldest item in the queue 
			nextMsgToProcess = m_messageQ.back();
			// and removes the oldest item in the queue
			m_messageQ.pop_back();
			// must parse this stream buffer
			CParserHandler::instance()->parseStream(&nextMsgToProcess.m_data[0], nextMsgToProcess.m_length, true);

			string retVal = CParserHandler::instance()->GetFieldValue("structVagner", "Longitude");
		}
		// end of critical area - operating with the m_messageQ
		m_bufferMutex->mutexUnlock();
		// prints in console the information output
		CParserHandler::instance()->printOut();
	}
}
