#ifndef _CANSISTRING_H_
#define _CANSISTRING_H_

#include <string>
#include "CCommonTypes.h"

using namespace std;
using namespace Types;

class CAnsiString {
public:
	// regular ctor - just initialize m_data
	CAnsiString();
	// regular ctor - just initialize m_data with input
	CAnsiString(string input);

	string getData();

	TInt32 size();

	// removes all format definitions
	void resetFormat();
	// set text foreground color 
	void setForegroundColor(EAnsiColor ansColor);
	// set text background color 
	void setBackgroundColor(EAnsiColor ansColor);
	// set bold style
	void setBold();

	void operator=(string param);
	void operator+=(string param);

	//void setColor()

private:
	string m_data;
};

inline void
CAnsiString::resetFormat()
{
	m_data += "\x1B[0m";
}

inline void
CAnsiString::setBold()
{
	m_data += "\x1B[1m";
}

inline string 
CAnsiString::getData()
{
	return m_data;
}

inline TInt32 
CAnsiString::size()
{
	return m_data.size();
}

#endif //_CANSISTRING_H_
