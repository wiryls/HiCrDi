/****************************************************************************
 *	@file		Heart.cpp
 *	@brief		Implement of Heart.hpp
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 ***************************************************************************/

#include "Heart.hpp"

/****************************************************************************
 *	private Impl
 ***************************************************************************/

struct hb::Heart::Impl
{
	;
};


/****************************************************************************
 *	public Constructor \ Destructor
 ***************************************************************************/

hb::Heart::Heart() : impl(std::make_unique<Impl>()) {}

hb::Heart::~Heart() {}