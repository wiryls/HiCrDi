/**********************************************************************
 *
 *	@file		log.hpp
 *	@brief		a simple class for logging
 *
 *	Date        Name        Description
 *	02/09/15	MYLS		Creation.
 *
 *********************************************************************/

#include <typeinfo>

#include "log.hpp"

void cm::Log::
Output2Console()
{
	std::cout << std::endl;
}

void cm::Log::Output2File()
{
	File << std::endl;
}

void cm::Log::
Switch(Type type, State state)
{
	if (state == State::FILE && !File.is_open()) {
		File.open("log.txt");
		if (!File.is_open()) {
			Err(typeid(Log), " Cannot open file: \'log.txt\'!");
			return;
		}
	}

	switch (type)
	{
	case Type::MSG:
		MessageOutput = state;
		break;
	case Type::ERR:
		ErrorOutput = state;
		break;
	case Type::DBG:
		DebugOutput = state;
		break;
	default:
		break;
	}
}

cm::Log::State	cm::Log::MessageOutput = State::CONSOLE;
cm::Log::State	cm::Log::ErrorOutput = State::CONSOLE;

#ifdef _DEBUG
cm::Log::State	cm::Log::DebugOutput = State::CONSOLE;
#else
cm::Log::State	cm::Log::DebugOutput = State::OFF;
#endif

std::mutex		cm::Log::Mutex;
std::ofstream	cm::Log::File;
