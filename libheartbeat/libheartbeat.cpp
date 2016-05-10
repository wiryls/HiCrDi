/*********************************************************************
 *	@file		libheartbeat.cpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/ 

#include "core/heart.hpp"
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
	return i->view(bgra32, wid, hgt);
}

LIBHEARTBEAT_API void hb::i_know(Heart * i, uint8_t bgra32[], size_t wid, size_t hgt)
{
	return i->know(bgra32, wid, hgt);
}

LIBHEARTBEAT_API int hb::i_decide(Heart * i)
{
	return static_cast<int>(i->plan());
}

LIBHEARTBEAT_API void hb::i_sleep(Heart * i)
{
	delete i;
}

LIBHEARTBEAT_API void hb::i_start(Heart * i)
{
	return i->start();
}

LIBHEARTBEAT_API void hb::i_rest(Heart * i)
{
	return i->pause();
}

LIBHEARTBEAT_API void hb::i_continue(Heart * i)
{
	return i->resume();
}

LIBHEARTBEAT_API void hb::i_stop(Heart * i)
{
	return i->stop();
}

LIBHEARTBEAT_API bool hb::am_i_dead(Heart * i)
{
	return i->dead();
}
