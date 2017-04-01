#include "CAnsiString.h"

CAnsiString::CAnsiString() : m_data("") 
{
	resetFormat();
	setForegroundColor(Types::EANSICOLOR_CYAN);
}

CAnsiString::CAnsiString(string inputstr) : m_data(inputstr)
{
	// empty implementation
}

void CAnsiString::operator=(string param)
{
	m_data = param;
}

void
CAnsiString::operator+=(string param)
{
	m_data += param;
}

void 
CAnsiString::setForegroundColor(EAnsiColor ansColor)
{
	switch (ansColor)
	{
		case EANSICOLOR_BLACK:
			m_data += "\x1B[30m";
			break;
		case EANSICOLOR_RED:
			m_data += "\x1B[31m";
			break;
		case EANSICOLOR_BLUE:
			m_data += "\x1B[34m";
			break;
		case EANSICOLOR_CYAN:
			m_data += "\x1B[36m";
			break;
		case EANSICOLOR_GREEN:
			m_data += "\x1B[32m";
			break;
		case EANSICOLOR_MAGENTA:
			m_data += "\x1B[35m";
			break;
		case EANSICOLOR_WHITE:
			m_data += "\x1B[37m";
			break;
		case EANSICOLOR_YELLOW:
			m_data += "\x1B[33m";
			break;
		default:
			// do nothing
			break;
	}
}

void
CAnsiString::setBackgroundColor(EAnsiColor ansColor)
{
	switch (ansColor)
	{
	case EANSICOLOR_BLACK:
		m_data += "\x1B[40m";
		break;
	case EANSICOLOR_RED:
		m_data += "\x1B[41m";
		break;
	case EANSICOLOR_BLUE:
		m_data += "\x1B[44m";
		break;
	case EANSICOLOR_CYAN:
		m_data += "\x1B[46m";
		break;
	case EANSICOLOR_GREEN:
		m_data += "\x1B[42m";
		break;
	case EANSICOLOR_MAGENTA:
		m_data += "\x1B[45m";
		break;
	case EANSICOLOR_WHITE:
		m_data += "\x1B[47m";
		break;
	case EANSICOLOR_YELLOW:
		m_data += "\x1B[43m";
		break;
	default:
		// do nothing
		break;
	}
}
