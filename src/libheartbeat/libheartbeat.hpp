/*********************************************************************
 *	@file		libheartbeat.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/ 
#pragma once

#include <stdint.h>

#ifdef LIBHEARTBEAT_EXPORTS
#define LIBHEARTBEAT_API __declspec(dllexport) 
#else
#define LIBHEARTBEAT_API __declspec(dllimport) 
#endif

namespace hb {

	/**
	 *	\brief	beating Heart
	 */
	class Heart;
}

namespace hb {

	/**
	 *	\brief	get version of this lib.
	 *
	 *	\return	version in uint.
	 */
	extern "C" LIBHEARTBEAT_API uint32_t version();

	/**
	 *	\brief	if using log.
	 *
	 *	\param	enbale enable if true.
	 */
	extern "C" LIBHEARTBEAT_API void log_switch(bool enbale);
}

namespace hb {

	/**
	 *	\brief	rise of a Heart.
	 *
	 *	\return a pointer to a new beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API Heart * i_come();

	/**
	 *	\brief	am i ready.
	 *
	 *	\return true if i get ready
	 */
	extern "C" LIBHEARTBEAT_API bool am_i_ready(Heart * i);

	/**
	 *	\brief	i see, then i jump...
	 *
	 *	\param	i,				i have a beating Heart.
	 *	\param	bgra32,			what i see, is a bgra32 image.
	 *	\param	wid,			width  of what i see.
	 *	\param	hgt,			height of what i see.
	 *
	 *	\return	does i see?
	 */
	extern "C" LIBHEARTBEAT_API bool i_see(Heart * i, uint8_t const bgra32[], size_t wid, size_t hgt);

	/**
	 *	\brief	i see, then i jump...
	 *
	 *	\param	i,				i have a beating Heart.
	 *	\param	bgra32,			what i see, is a bgra32 image.
	 *	\param	wid,			width  of what i see.
	 *	\param	hgt,			height of what i see.
	 */
	extern "C" LIBHEARTBEAT_API void i_know(Heart * i, uint8_t bgra32[], size_t wid, size_t hgt);

	/**
	 *	\brief	after i see.
	 *
	 *	\return	should i jump?
	 */
	extern "C" LIBHEARTBEAT_API int i_decide(Heart * i);

	/**
	 *	\brief	the Heart, could not beat any nore...
	 *
	 *	\param	i,				i have a beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API void i_sleep(Heart * i);

	/**
	 *	\brief	the Heart, starts jumping...
	 *
	 *	\param	i,				i have a beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API void i_start(Heart * i);

	/**
	 *	\brief	the Heart, takes a rest...
	 *
	 *	\param	i,				i have a beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API void i_rest(Heart * i);

	/**
	 *	\brief	the Heart, carrys on...
	 *
	 *	\param	i,				i have a beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API void i_continue(Heart * i);

	/**
	 *	\brief	the Heart, stops...
	 *
	 *	\param	i,				i have a beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API void i_stop(Heart * i);

	/**
	 *	\brief	is i dead...
	 *
	 *	\param	i,				i have a beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API bool am_i_dead(Heart * i);
}
