#ifndef _COMMONTYPES_H_
#define _COMMONTYPES_H_

#include <memory>
#include <list>
#include <map>
#include <unordered_map>

using namespace std;

namespace Types {
	typedef unsigned int TUInt32;
	typedef int TInt32;
	typedef short TInt16;
	typedef unsigned short TUInt16;
	typedef char TByte;
	typedef unsigned char TUByte;
	typedef double TDouble;
	typedef float TFloat;

	enum EFieldTypes
	{
		EFIELDTYPES_FIRST = 0,
		EFIELDTYPES_INT32 = EFIELDTYPES_FIRST,
		EFIELDTYPES_UINT32,
		EFIELDTYPES_FLOAT,
		EFIELDTYPES_DOUBLE,
		EFIELDTYPES_CHAR, // also byte type
		EFIELDTYPES_BOOL,
        EFIELDTYPES_ENUM,
		EFIELDTYPES_LAST
	};

	static const TInt32 s_GAME_CONNECTION_PORT = 1234;
	static const TInt32 s_MAX_BUFFER_SIZE = 4096;

	enum EAnsiColor
	{
		EANSICOLOR_BLACK = 0,
		EANSICOLOR_RED,
		EANSICOLOR_GREEN,
		EANSICOLOR_YELLOW,
		EANSICOLOR_BLUE,
		EANSICOLOR_MAGENTA,
		EANSICOLOR_CYAN,
		EANSICOLOR_WHITE
	};

	enum EMessageDirection
	{
		EMESSAGEDIRECTION_Incoming = 0,
		EMESSAGEDIRECTION_Outgoing
	};

    typedef map<int, string> GildaEnumMap;
	// internal structure
	struct SField
	{
		// field name
		string m_name;
		// type to perform cast
		EFieldTypes m_type;
		// size in bytes
		int m_size;
		// offset of this field in the structure
		int m_offset;
		// value (after parsing)
		// decided to parse values to string
		string m_value;
		// gilda enumerator
        GildaEnumMap m_enum_dictionary;
		// flag to know if this field should have the content refreshed
		bool m_refresh;
	};

	struct SIncomingMsg
	{
		TUInt32 m_length;
		TByte m_data[s_MAX_BUFFER_SIZE];
	};

	typedef map<string, SField> MessageFieldMap;

	struct SMessageStruct
	{
		string m_name;
		EMessageDirection m_direction;
		MessageFieldMap m_fields;
	};

	typedef unordered_map<string, shared_ptr<SMessageStruct>> MessageStructureMap;
	//typedef unordered_map<TInt32, shared_ptr<MessageFieldMap>> MessageStructureOpCodeMap;
	typedef list<SIncomingMsg> MessageBufferQ;

}

#endif //_COMMONTYPES_H_
