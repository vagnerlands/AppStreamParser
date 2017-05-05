#ifndef _CPARSERHANDLER_H_
#define _CPARSERHANDLER_H_

#include <string>
#include <iostream>
#include <fstream>
#include "CCommonTypes.h"
#include "CAnsiString.h"
#include "ISocket.h"

using namespace Types;
using namespace std;


// Singleton: CParserHandler
// No need to hold multiple instances of this class
// therefore, we may declare this "global"
class CParserHandler 
{
public:
    static const int s_MAX_ENUMERATORS_IN_GILDA = 32;
	static CParserHandler* instance();
	// return
	// 0 success
	// 1 field type isn't valid

	TInt32 addField(char* structureName, 
		TInt32 opCode,
		char* fieldName, 
		char* fieldType, 
		TInt32 fieldOffset, 
		TInt32 fieldSize, 
		char* arr[s_MAX_ENUMERATORS_IN_GILDA]);
	TInt32 parseStream(char* bufStream, int lengthOfStream, bool isBigEndian);
	TInt32 sendStream(ISocket* socketout);
	bool saveToFile(char* fileName);
	void printOut();
private:
	
	CParserHandler();
	~CParserHandler();
	static CParserHandler* s_pInstance;

	// Key is Offset (theoretically, should be only one)
	// content is SField

	//MessageFieldMap m_dictionary;
	MessageStructureMap m_dictionaryIndexedByStructName;
	MessageStructureMap m_dictionaryIndexedByOpCode;
	//MessageStructureOpCodeMap m_dictionaryIndexedByOpCode;
};

#endif

