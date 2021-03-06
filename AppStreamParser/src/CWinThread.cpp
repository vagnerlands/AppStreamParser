#include "CWinThread.h"

CWinThread::~CWinThread()
{

}

CWinThread::CWinThread()
{
	// empty implementation
}

void CWinThread::createThread(string thName, void* thEntry)
{
	m_threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thEntry, 0, 0x00000000, &m_threadID);
	if (m_threadHandle == NULL)
	{
		cout << "CreateThread error: " << GetLastError() << " on creating <" << thName << ">" << endl;
	}
	cout << "Created <" << thName << "> thread " << m_threadHandle << endl;
}


void CWinThread::destroy()
{
	CloseHandle(m_threadHandle);
}
