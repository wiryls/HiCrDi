/****************************************************************************
 *	@file		ibeat.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	09/05/16	MYLS		Creation.
 *
 ***************************************************************************/
#pragma once

namespace hb 
{
	template<class Input, class Output> class IBeat;
}

/****************************************************************************
 *	declaration
 ***************************************************************************/

/**
 *	\brief	TODO:
 */
template<class input_t, class output_t> class hb::IBeat
{

public:

	typedef  input_t  Input;

	typedef output_t Output;

public:

	virtual IBeat &	operator <<   (Input  const &) = 0;

	virtual IBeat & operator >>   (Output &)       = 0;

	virtual         operator bool () const         = 0;

	virtual void    clear()						   = 0;

public:

	/**
	 *	\brief virtual destructor
	 */
	virtual ~IBeat() = 0;
};

/****************************************************************************
 *	implementation
 ***************************************************************************/

template<class T, class U> inline hb::IBeat<T, U>::~IBeat() {}
