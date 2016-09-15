/****************************************************************************
 *	@file		ifeel.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	09/05/16	MYLS		Creation.
 *
 ***************************************************************************/
#pragma once

#include "ibeat.hpp"

namespace hb 
{
	struct View;
	struct Sense;

	typedef IBeat<View, Sense> IFeel;
}
