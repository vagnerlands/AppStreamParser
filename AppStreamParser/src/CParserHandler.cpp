#include "CParserHandler.h"
#include <stdlib.h>
#include <windows.h>
#include <sstream>
CParserHandler* CParserHandler::s_pInstance = nullptr;

CParserHandler * CParserHandler::instance()
{
	if (s_pInstance == nullptr)
	{
		s_pInstance = new CParserHandler();
	}
	return s_pInstance;
}

TInt32
CParserHandler::addField(char* structureName, 
	TInt32 opCode,
	char* fieldName, 
	char* fieldType, 
	TInt32 fieldOffset, 
	TInt32 fieldSize, 
	char* arr[s_MAX_ENUMERATORS_IN_GILDA])
{
	// 0 == success
	// any other value means failure
	TInt32 retVal = 0;
	// data to be added to the table
	SField newDBEntry;
	string fType(fieldType);
	string fName(fieldName);
	string fStructureName(structureName);
	// temporary buffer - convert from integer to string operation
	// no need for very long buffer
	char tempBuf[18];
	// converts opCode to char[]
	sprintf_s(tempBuf, "%d", opCode);

	cout << "addField called " << fieldType << " " << fieldType << endl;
	if (fType == "int")
	{
		newDBEntry.m_type = EFIELDTYPES_INT32;
	}
	else if (fType == "uint")
	{
		newDBEntry.m_type = EFIELDTYPES_UINT32;
	}
	else if ((fType == "float") || (fType == "real"))
	{
		newDBEntry.m_type = EFIELDTYPES_FLOAT;
	}
	else if (fType == "double")
	{
		newDBEntry.m_type = EFIELDTYPES_DOUBLE;
	}
	else if (fType == "char")
	{
		newDBEntry.m_type = EFIELDTYPES_CHAR;
	}
	else if (fType == "bool")
	{
		newDBEntry.m_type = EFIELDTYPES_BOOL;
	}
	else if (fType == "enum")
	{
		newDBEntry.m_type = EFIELDTYPES_ENUM;
        for (int i = 0; i < s_MAX_ENUMERATORS_IN_GILDA; i++)
        {
            if (arr[i][0] == 0) break;
            newDBEntry.m_enum_dictionary.insert(make_pair(i, arr[i]));
        }
        
	}
    else
	{
		// type is not valid
		retVal = 1;
	}

	// field offset
	newDBEntry.m_offset = fieldOffset;
	// field size
	newDBEntry.m_size = fieldSize;
	// field name
	newDBEntry.m_name = fName;
	// always refresh the field on first run
	newDBEntry.m_refresh = true;

	if (retVal == 0)
	{
		// try to find given structure name in m_dictionary
		MessageStructureMap::iterator structureIterator = m_dictionaryIndexedByStructName.find(fStructureName);
		// in case the iterator is not valid
		if (structureIterator == m_dictionaryIndexedByStructName.end())
		{
			// creates a new DS for the field messages within this structure
			shared_ptr<MessageFieldMap> newMsgField( new MessageFieldMap() );
			//MessageFieldMap newMsg;
			//newMsg.insert(make_pair(fieldOffset, newDBEntry));
			// adds the new field to this fields DS
			newMsgField->insert(make_pair(fieldOffset, newDBEntry));
			// then adds a new item for the Structure
			m_dictionaryIndexedByStructName.insert(make_pair(fStructureName, newMsgField));
			m_dictionaryIndexedByOpCode.insert(make_pair(tempBuf, newMsgField));
		}
		else
		{
			// in case the structure already exists - just adds it to the DS
			structureIterator->second->insert(make_pair(fieldOffset, newDBEntry));
		}
	}

	return retVal;
}

TInt32
CParserHandler::parseStream(char * bufStream, int lengthOfStream, bool isBigEndian)
{
	// current buffer position/pointer
	int curBuffPos = 0;
    // string stream to convert float to string
    stringstream ss(stringstream::in | stringstream::out);
    // temporary buffer (for enumeration swapping
    char buffer[33];
    // search iterator
	GildaEnumMap::iterator locIt;
	// TODO: must add some pre-parser of "bufStream" to find the opcode and use it to search
	// which structure iterator to be used in the parsing
	MessageStructureMap::iterator structureIterator = m_dictionaryIndexedByOpCode.begin();
	// go over till the pointer has reached the end of the stream buffer
	while (curBuffPos < lengthOfStream)
	{
		MessageFieldMap::iterator fieldIterator =  structureIterator->second->find(curBuffPos);
		if (fieldIterator != structureIterator->second->end())
		{
			// input node
			SField* node = &fieldIterator->second;
			// integer/float/double convertion buffer
			char numericBuf[20];
			string t;
			// temporary buffer
			char maxBuf[s_MAX_BUFFER_SIZE];
			// data buffer
			char* allocBuf = new char[node->m_size];
			// copies content of bufStream to this new temporary buffer
			memcpy(allocBuf, bufStream + curBuffPos, node->m_size);
			// if this is big endian, perform byte swapping accordingly
			// not sure if char type should be parsed, though
			if (isBigEndian)
			{
				// byte iterator 
				TInt32 bi = 0;
				TInt32 fieldOffset = node->m_size - 1;
				// iterates till the current byte iterator cursor reaches the middle point
				while (bi < (fieldOffset - bi))
				{
					char swapByte = allocBuf[fieldOffset - bi];
					allocBuf[fieldOffset - bi] = allocBuf[bi];
					allocBuf[bi] = swapByte;
					bi++;
				}
			}

			switch (node->m_type)
			{
				case EFIELDTYPES_BOOL:
					// temporary buffer
					TInt32 boolVal;
					// high risk of mem corruption!!!
					memcpy(&boolVal, allocBuf, node->m_size);
					if (boolVal > 0)
					{
						if (node->m_value != "TRUE") node->m_refresh = true;
						node->m_value = "TRUE";
					}
					else
					{
						if (node->m_value != "FALSE") node->m_refresh = true;
						node->m_value = "FALSE";
					}
					break;
				case EFIELDTYPES_ENUM:
					// temporary buffer
					TUInt32 enumUIntVal;
					// high risk of mem corruption!!!
					memcpy(&enumUIntVal, allocBuf, node->m_size);
                    //search for the value in the enum_dictionary
                    locIt = node->m_enum_dictionary.find(enumUIntVal);
                    if (locIt != node->m_enum_dictionary.end())
                    {
                        if (node->m_value != locIt->second)
                        {
                            node->m_refresh = true;
                        }
                        node->m_value = locIt->second;
                    }
                    else
                    {
                        node->m_value = "<!> PARS_FAIL[";
                        _itoa(enumUIntVal, buffer, 10);/*to_string(enumUIntVal)*/;
                        node->m_value += buffer;
                        node->m_value += "]";
                    }
                    
					break;

				case EFIELDTYPES_UINT32:
					// temporary buffer
					TUInt32 uintVal;
					// high risk of mem corruption!!!
					memcpy(&uintVal, allocBuf, node->m_size);
					sprintf_s(numericBuf, "%d", uintVal);
					t = numericBuf;
					if (node->m_value != t) node->m_refresh = true;
					node->m_value = t;
					break;
				case EFIELDTYPES_INT32:
					// temporary buffer
					int intVal;
					// high risk of mem corruption!!!
					memcpy(&intVal, allocBuf, node->m_size);
					sprintf_s(numericBuf, "%d", intVal);
					t = numericBuf;
					if (node->m_value != t) node->m_refresh = true;
					node->m_value = t;
					break;
				case EFIELDTYPES_FLOAT:
					// temporary buffer
					float floatVal;
					// high risk of mem corruption!!!
					memcpy(&floatVal, allocBuf, node->m_size);
					sprintf_s(numericBuf, "%f", floatVal);
					t = numericBuf;
					if (node->m_value != t) node->m_refresh = true;
					node->m_value = t;
					break;
				case EFIELDTYPES_DOUBLE:
					// temporary buffer
					double doubleVal;
					// high risk of mem corruption!!!
					memcpy(&doubleVal, allocBuf, node->m_size);
					sprintf_s(numericBuf, "%f", doubleVal);
					t = numericBuf;
					if (node->m_value != t) node->m_refresh = true;
					node->m_value = t;
					break;
				case EFIELDTYPES_CHAR:
					// high risk of mem corruption!!!
					memcpy(&maxBuf, allocBuf, node->m_size);
					// null terminator
					maxBuf[node->m_size] = 0;
					node->m_value = maxBuf;
					node->m_refresh = true;
					break;
				default:
					// no parsing
					break;
			}
			// no one wants memory leaks!
			delete[] allocBuf;
			allocBuf = nullptr;

			// add the same number of bytes to the current buffer since the current was parsed
			curBuffPos += node->m_size;
		}
		else
		{
			// no item with this offset in the list
			// move one byte forward
			curBuffPos++;
		}

	}
	return 0;
}

TInt32
CParserHandler::sendStream(ISocket * socketout)
{
	socketout->write("a", 1);
	return 0;
}

bool CParserHandler::saveToFile(char * fileName)
{
	// assuming that given filename is okay
	// opens a fd handler
	std::ofstream ofs(fileName, ofstream::out);

	// iterates through opcode - but it should have identical fields / content as indexed by structure name
	for (MessageStructureMap::iterator structIter = m_dictionaryIndexedByOpCode.begin();
		structIter != m_dictionaryIndexedByOpCode.end(); structIter++)
	{
		// iterates in the map (according to the offset order)
		for (MessageFieldMap::iterator iter = structIter->second->begin(); 
			iter != structIter->second->end(); iter++)
		{
			// formats the string as 
			// Field Name = Value \r\n
			ofs << iter->second.m_name << " = " << iter->second.m_value << endl;
		}
	}

	// closes the fd handler
	ofs.close();
	return false;
}

void CParserHandler::printOut()
{
	// first line in console to print the report
	TInt32 lineNum = 3;
	// iterates through opcode - but it should have identical fields / content as indexed by structure name
	for (MessageStructureMap::iterator structIter = m_dictionaryIndexedByOpCode.begin();
		structIter != m_dictionaryIndexedByOpCode.end(); structIter++)
	{
		// iterates in the map (according to the offset order)
		for (MessageFieldMap::iterator iter = structIter->second->begin(); 
			iter != structIter->second->end(); iter++)
		{
			if (iter->second.m_refresh)
			{
				// no need to refresh it till the content has changed
				iter->second.m_refresh = false;
				// formats the string as 
				// Field Name = Value \r\n
				CAnsiString fieldStr;
				CAnsiString valueStr;
				COORD coord;
				fieldStr.resetFormat();
				fieldStr.setForegroundColor(Types::EANSICOLOR_CYAN);
				fieldStr += iter->second.m_name;

				valueStr.resetFormat();
				valueStr.setBold();
				valueStr.setForegroundColor(Types::EANSICOLOR_MAGENTA);
				valueStr += iter->second.m_value;

				coord.Y = lineNum;
				coord.X = 0;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
				cout << fieldStr.getData();
				// connects the field name with the value with .........
				for (TInt32 x = 0; x < (35 - iter->second.m_name.size()); x++) cout << ".";

				coord.Y = lineNum;
				coord.X = 35;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
				// clears the value field
				for (TInt32 x = 0; x < 40; x++) cout << " ";
				// prints it out
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
				cout << valueStr.getData();
			}
			++lineNum;
		}
	}
}

CParserHandler::CParserHandler()
{
	// empty implementation
}

CParserHandler::~CParserHandler()
{
	// avoid memory leak
	delete s_pInstance;
}