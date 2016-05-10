/**********************************************************************
 *
 *	@file		log.hpp
 *	@brief		Static Class Log, for normal logging.
 *				TODO: add file output support.
 *
 *	Date        Name        Description
 *	02/09/15	MYLS		Creation.
 *	03/17/16	MYLS		Add debug message.
 *	03/22/16	MYLS		Support to output type_info.
 *
 *********************************************************************/

#pragma once

#include <typeinfo>
#include <mutex>
#include <iostream>
#include <fstream>

namespace cm {
	class Log;
	template<class ostream> inline ostream & operator<< (ostream & os, type_info const & info);
}

/**
 *	\brief	Log messages
 */
class cm::Log {

public:

	/**
	 *	\brief	Message type
	 */
	enum struct Type : size_t;

	/**
	 *	\brief	Output target
	 */
	enum struct State : size_t;

public:

	/**
	 *	\brief	Output a message
	 *	\param	any supported type
	 */
	template<typename... Types> static inline void Msg(Types && ... args);
	
	/**
	 *	\brief	Output an error message
	 *	\param	any supported type
	 */
	template<typename... Types> static inline void Err(Types && ... args);

	/**
	 *	\brief	Output a debug message
	 *	\param	any supported type
	 */
	template<typename... Types> static inline void Dbg(Types && ... args);

	/**
	 *	\brief	Enable or disable a certain type of message
	 *	\param	type	type of message {DBG, MSG, ERR}
	 *	\param	state	on or off
	 */
	static void Switch(Type type, State state);

private:

	/**
	 *	\brief	Output a line break
	 */
	static void Output2Console();

	/**
	 *	\brief	Output message to console
	 *	\param	value	first param
	 *	\param	args	rest params
	 */
	template<typename T, typename... Types> static inline void Output2Console(const T & value, Types && ... args);

	/**
	 *	\brief	Output a line break
	 */
	static void Output2File();

	/**
	 *	\brief	Output message to console
	 *	\param	value	first param
	 *	\param	args	rest params
	 */
	template<typename T, typename... Types> static inline void Output2File(const T & value, Types && ... args);

	static State MessageOutput;		/**< Specify normal message output target */
	static State ErrorOutput;		/**< Specify error message output target */
	static State DebugOutput;		/**< Specify debug message output target */

	static std::ofstream File;		/**<  >*/
	static std::mutex Mutex;		/**<  >*/
};


/*********************************************************************
 *	ostream overload
 *********************************************************************/

/**
 *	\brief	override << for output type_info
 *	\param	os		any ostream
 *	\param	info	typeinfo
 *	\return	ref of os
 */
template<class ostream> inline ostream & cm::
operator<<(ostream & os, type_info const & info)
{
	os << info.name();
	return os;

	/* Note:
	 * [Operator overloading outside class]
	 * (http://stackoverflow.com/a/2425965)
	 */
}

/*********************************************************************
 *	enum
 *********************************************************************/

enum struct cm::Log::Type : size_t {
	DBG,
	MSG,
	ERR,
};

enum struct cm::Log::State : size_t {
	CONSOLE,
	FILE,
	OFF,
};

/*********************************************************************
 *	implement
 *********************************************************************/

template<typename T, typename ...Types> inline void cm::Log::
Output2Console(const T & value, Types && ... args)
{
	std::cout << value;
	Output2Console(args...);
}

template<typename T, typename ...Types>
inline void cm::Log::Output2File(const T & value, Types && ...args)
{
	File << value;
	Output2File(args...);
}

template<typename ...Types> inline void cm::Log::
Dbg(Types && ... args)
{
	if (cm::Log::DebugOutput == State::CONSOLE) {
		Mutex.lock();
		std::cout << "[DBG] ";
		Output2Console(args...);
		Mutex.unlock();
	} else if (cm::Log::DebugOutput == State::FILE) {
		Mutex.lock();
		File << "[DBG] ";
		Output2File(args...);
		Mutex.unlock();
	}
}

template<typename ...Types> inline void cm::Log::
Msg(Types && ... args)
{
	if (cm::Log::MessageOutput == State::CONSOLE) {
		Mutex.lock();
		std::cout << "[MSG] ";
		Output2Console(args...);
		Mutex.unlock();
	} else if (cm::Log::DebugOutput == State::FILE) {
		Mutex.lock();
		File << "[MSG] ";
		Output2File(args...);
		Mutex.unlock();
	}
}

template<typename ...Types> inline void cm::Log::
Err(Types && ... args)
{
	if (cm::Log::ErrorOutput == State::CONSOLE) {
		Mutex.lock();
		std::cerr << "[ERR] ";
		Output2Console(args...);
		Mutex.unlock();
	} else if (cm::Log::DebugOutput == State::FILE) {
		Mutex.lock();
		File << "[ERR] ";
		Output2File(args...);
		Mutex.unlock();
	}
}


using cm::Log;

