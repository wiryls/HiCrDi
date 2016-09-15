/*********************************************************************
 *	@file		RandNum.cpp
 *	@brief		For generating random number.
 *
 *	Date        Name        Description
 *	05/18/16	MYLS		Creation.
 *
 *********************************************************************/

#include <algorithm>
#include <unordered_map>
#include <random>

#include "RandNum.hpp"

namespace cm
{
	template <typename _T> inline _T rand_real(_T min, _T max);
	template <typename _T> inline _T rand_integer(_T min, _T max);
}

namespace cm 
{
	static std::mt19937 engine = std::mt19937(std::random_device()());
}

template<typename _T> _T cm::rand_real(_T min, _T max)
{
	std::uniform_real_distribution<_T> distribution(min, max);
	return distribution(engine);
}

template<typename _T> _T cm::rand_integer(_T min, _T max)
{
	std::uniform_int_distribution<_T> distribution(min, max);
	return distribution(engine);
}

/* for real */

#if defined(CM_DECL_RAND)
#error "definitions has alreay existed!"
#else

#define CM_DECL_RAND(type) \
type cm::rand(type min, type max) { return rand_real(min, max); }

CM_DECL_RAND(float)
CM_DECL_RAND(double)

#undef CM_DECL_RAND

/* for integer */

#define CM_DECL_RAND(type) \
type cm::rand(type min, type max) { return rand_integer(min, max); }

//CM_DECL_RAND(uint8_t)
CM_DECL_RAND(uint16_t)
CM_DECL_RAND(uint32_t)
CM_DECL_RAND(uint64_t)
//CM_DECL_RAND(int8_t)
CM_DECL_RAND(int16_t)
CM_DECL_RAND(int32_t)
CM_DECL_RAND(int64_t)

#endif


