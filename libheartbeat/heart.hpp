/*********************************************************************
 *	@file		Heart.hpp
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
	class Heart;
}

/**
 *	\brief	A beating Heart
 */
class hb::Heart
{

public:

	// TODO:

public:

	/**
	 *	\brief constructor
	 */
	Heart();

	/**
	 *	\brief destructor
	 */
	~Heart();

private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};
