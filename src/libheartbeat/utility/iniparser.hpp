/*********************************************************************
 *	@file		iniparser.hpp
 *	@brief		Class iniparser, for load ini file
 *
 *
 *	Date        Name        Description
 *	04/03/15	MYLS		Creation.
 *
 *********************************************************************/

#pragma once

#include <memory>
#include <string>

namespace cm {
	class iniparser;
}

/**
 *	\brief	Class iniparser, for load ini file
 */
class cm::iniparser
{

public:

	/**
	 *	\brief Constructor
	 */
	iniparser();

	/**
	 *	\brief	Destructor
	 */
	~iniparser();

public:

	/**
	 *	\brief	load data from a file
	 *
	 *	\param	filePath	file path
	 *
	 *	\return	if loaded successfully
	 */
	bool load(const std::string & filePath);

	/**
	 *	\brief	get valueini of [section.key]
	 *
	 *	\param	key		"section.key"
	 *
	 *	\return			target value, if not exits, return "".
	 */
	const std::string & get_string(const std::string & key) const;

	/**
	 *	\brief	clear all data
	 */
	void clear();

private:

	/**
	 *	\brief	remove trailing spaces
	 *
	 *	\param	str		string to be trim
	 *
	 *	\return			result of trimming
	 */
	static std::string& trim(std::string & str);

	/**
	 *	\brief	remove trailing spaces[overload for GCC]
	 *
	 *	\param	str		string to be trim
	 *
	 *	\return			result of trimming
	 */
	static std::string trim(std::string && str);

private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};


