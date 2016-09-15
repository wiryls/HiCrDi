/**********************************************************************
 *
 *	@file		log.hpp
 *	@brief		Static Class logger, for normal logging.
 *
 *
 *	Date        Name        Description
 *	02/09/15	MYLS		Creation.
 *	03/17/16	MYLS		Add debug message.
 *	03/22/16	MYLS		Support to output type_info.
 *
 *********************************************************************/

#pragma once

#include <iostream>
#include <fstream>

namespace cm {
	class logger;
	template<class _ostream> inline _ostream & operator<< (_ostream & os, typename type_info const & info);
}

/**
 *	\brief	log messages
 */
class cm::logger {

public:

	/**
	 *	\brief	Message type
	 */
	enum struct type_t;

	/**
	 *	\brief	Output target
	 */
	enum struct target_t;

public:

	/**
	 *	\brief	Output a message
	 *	\param	any supported type
	 */
	template<typename... _Types> static inline void msg(_Types && ... args);
	
	/**
	 *	\brief	Output an error message
	 *	\param	any supported type
	 */
	template<typename... _Types> static inline void err(_Types && ... args);

	/**
	 *	\brief	Output a debug message
	 *	\param	any supported type
	 */
	template<typename... _Types> static inline void dbg(_Types && ... args);

	/**
	 *	\brief	Enable or disable a certain type of message
	 *	\param	type	type of message {DBG, MSG, ERR}
	 *	\param	target	output target {}
	 */
	static void set(type_t type, target_t target);

private:

	/**
	 *	\brief	Output a line break
	 */
	static void out_console();

	/**
	 *	\brief	Output message to console
	 *	\param	value	first param
	 *	\param	args	rest params
	 */
	template<typename _T, typename... _Types> static inline void out_console(const _T & value, _Types && ... args);

	/**
	 *	\brief	Output a line break
	 */
	static void out_file();

	/**
	 *	\brief	Output message to console
	 *	\param	value	first param
	 *	\param	args	rest params
	 */
	template<typename _T, typename... _Types> static inline void out_file(const _T & value, _Types && ... args);

	/**
	 *	\brief	lock (thread) or unlock
	 *	\param	if_lock	lock if true, else unlock
	 */
	static void lock(bool if_lock = true);

private:

	static target_t msg_target;		/**< specify normal message output target */
	static target_t err_target;		/**< specify error message output target */
	static target_t dbg_target;		/**< specify debug message output target */

	static std::ofstream file;		/**<  >*/
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
template<class _ostream> inline _ostream & cm::
operator<<(_ostream & os, type_info const & info)
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

enum struct cm::logger::type_t {
	DBG,
	MSG,
	ERR,
};

enum struct cm::logger::target_t {
	CONSOLE,
	FILE,
	OFF,
};

/*********************************************************************
 *	implement
 *********************************************************************/

template<typename _T, typename ... _Types> inline void cm::logger::
out_console(const _T & value, _Types && ... args)
{
	std::cout << value;
	out_console(args...);
}

template<typename _T, typename ... _Types>
inline void cm::logger::out_file(const _T & value, _Types && ...args)
{
	file << value;
	out_file(args...);
}

template<typename ... _Types> inline void cm::logger::
dbg(_Types && ... args)
{
	lock(true);
	switch (logger::dbg_target)
	{
	case target_t::CONSOLE: {
		std::cout << "[DBG] ";
		out_console(args...);
		break;
	}
	case target_t::FILE: {
		if (!file.is_open())
			break;
		file << "[DBG] ";
		out_file(args...);
		break;
	}
	default: break;
	}
	lock(false);
}

template<typename ... _Types> inline void cm::logger::
msg(_Types && ... args)
{
	lock(true);
	switch (logger::dbg_target)
	{
	case target_t::CONSOLE: {
		std::cout << "[MSG] ";
		out_console(args...);
		break;
	}
	case target_t::FILE: {
		if (!file.is_open())
			break;
		file << "[MSG] ";
		out_file(args...);
		break;
	}
	default:
		break;
	}
	lock(false);
}

template<typename ... _Types> inline void cm::logger::
err(_Types && ... args)
{
	lock(true);
	switch (logger::dbg_target)
	{
	case target_t::CONSOLE: {
		std::cerr << "[ERR] ";
		out_console(args...);
		break;
	}
	case target_t::FILE: {
		if (!file.is_open())
			break;
		file << "[ERR] ";
		out_file(args...);
		break;
	}
	default:
		break;
	}
	lock(false);
}

using cm::logger;

