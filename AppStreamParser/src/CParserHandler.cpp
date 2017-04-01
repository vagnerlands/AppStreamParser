#include "CParserHandler.h"
#include <stdlib.h>
#include <windows.h>
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
CParserHandler::addField(char* fieldName, char* fieldType, int fieldOffset, int fieldSize)
{
	// 0 == success
	// any other value means failure
	int retVal = 0;
	// data to be added to the table
	SField newDBEntry;
	string fType(fieldType);
	string fName(fieldName);
	cout << "addField called " << fieldType << " " << fieldType << endl;
	if (fType == "int")
	{
		newDBEntry.m_type = EFIELDTYPES_INT32;
	}
	else if (fType == "uint")
	{
		newDBEntry.m_type = EFIELDTYPES_UINT32;
	}
	else if (fType == "float")
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
		// adds a new item to the data structure
		m_dictionary.insert({ fieldOffset , newDBEntry });
	}

	return retVal;
}

TInt32
CParserHandler::parseStream(char * bufStream, int lengthOfStream, bool isBigEndian)
{
	// current buffer position/pointer
	int curBuffPos = 0;
	// go over till the pointer has reached the end of the stream buffer
	while (curBuffPos < lengthOfStream)
	{
		if (m_dictionary.find(curBuffPos) != m_dictionary.end())
		{
			// input node
			SField* node = &m_dictionary[curBuffPos];
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
			if ((isBigEndian) && (node->m_type != EFIELDTYPES_CHAR))
			{
				// byte iterator 
				int bi = 0;
				int fieldOffset = node->m_size - 1;
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
					int boolVal;
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
				case EFIELDTYPES_UINT32:
					// temporary buffer
					unsigned int uintVal;
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
	// iterates in the map (according to the offset order)
	for (MessageFieldMap::iterator iter = m_dictionary.begin(); iter != m_dictionary.end(); iter++)
	{
		// formats the string as 
		// Field Name = Value \r\n
		ofs << iter->second.m_name << " = " << iter->second.m_value << endl;
	}

	// closes the fd handler
	ofs.close();
	return false;
}

void CParserHandler::printOut()
{
	// first line in console to print the report
	TInt32 lineNum = 3;
	// iterates in the map (according to the offset order)
	for (MessageFieldMap::iterator iter = m_dictionary.begin(); iter != m_dictionary.end(); iter++)
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

CParserHandler::CParserHandler()
{
	// empty implementation
}

CParserHandler::~CParserHandler()
{
	// avoid memory leak
	delete s_pInstance;
}