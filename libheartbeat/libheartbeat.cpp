/*********************************************************************
 *	@file		libheartbeat.cpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/ 

#include "heart.hpp"
#include "libheartbeat.hpp"

LIBHEARTBEAT_API uint32_t hb::version()
{
	return 0x01000000U;
}

LIBHEARTBEAT_API hb::Heart * hb::i_come()
{
	return new Heart();
}

LIBHEARTBEAT_API bool hb::i_see(Heart * i, uint8_t const bgra32[], size_t wid, size_t hgt)
{
	return false;
}

LIBHEARTBEAT_API bool hb::i_jump()
{
	return false;
}

LIBHEARTBEAT_API void hb::i_sleep(Heart * i)
{
	delete i;
}

