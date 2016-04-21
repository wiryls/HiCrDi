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

	/**
	 *	\brief	get version of this lib.
	 *
	 *	\return	version in uint.
	 */
	extern "C" LIBHEARTBEAT_API uint32_t version();

	/**
	 *	\brief	rise of a Heart.
	 *
	 *	\return a pointer to a new beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API Heart * i_come();
	
	/**
	 *	\brief	i see, then i beat...
	 *
	 *	\param	i,				i have a beating Heart.
	 *	\param	bgra32,			what i see, is a bgra32 image.
	 *	\param	wid,			width  of what i see.
	 *	\param	hgt,			height of what i see.
	 *
	 *	\return	is Heart beating?
	 */
	extern "C" LIBHEARTBEAT_API bool i_see(Heart * i, uint8_t const bgra32[], size_t wid, size_t hgt);
	
	/**
	 *	\brief	the Heart, could not beat any nore...
	 *
	 *	\param	i,				i have a beating Heart.
	 */
	extern "C" LIBHEARTBEAT_API void i_conquer(Heart * i);
}
