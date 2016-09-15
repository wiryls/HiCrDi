/*********************************************************************
 *	@file		RandNum.hpp
 *	@brief		For generating random number.
 *
 *	Date        Name        Description
 *	05/18/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <cstdint>

namespace cm 
{
	/**
	 *	\brief	get a rand number
	 *
	 *	\param	min
	 *	\param	max
	 *	\return	random value of [min, max]
	 */
	extern float    rand(float    min = 0, float    max = 1.f);
	extern double   rand(double   min = 0, double   max = 1.0);
//  extern uint8_t  rand(uint8_t  min = 0, uint8_t  max = std::numeric_limits<uint8_t >::max());
	extern uint16_t rand(uint16_t min = 0, uint16_t max = std::numeric_limits<uint16_t>::max());
	extern uint32_t rand(uint32_t min = 0, uint32_t max = std::numeric_limits<uint32_t>::max());
	extern uint64_t rand(uint64_t min = 0, uint64_t max = std::numeric_limits<uint64_t>::max());
//  extern int8_t   rand(int8_t   min = 0, int8_t   max = std::numeric_limits<int8_t  >::max());
	extern int16_t  rand(int16_t  min = 0, int16_t  max = std::numeric_limits<int16_t >::max());
	extern int32_t  rand(int32_t  min = 0, int32_t  max = std::numeric_limits<int32_t >::max());
	extern int64_t  rand(int64_t  min = 0, int64_t  max = std::numeric_limits<int64_t >::max());
}
