/*********************************************************************
 *	@file		Mind.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <memory>

namespace hb {
	class Mind;
}

/**
 *	\brief	TODO:
 */
class hb::Mind {

public:


public:


public:

	/**
	 *	\brief constructor
	 */
	Mind();

	/**
	 *	\brief destructor
	 */
	~Mind();

public:


private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};
