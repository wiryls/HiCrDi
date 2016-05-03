/*********************************************************************
 *	@file		timer.cpp
 *	@brief		Implement of class Timer
 *
 *	Date        Name        Description
 *	23/11/14	MYLS		Creation.
 *	08/12/14	MYLS		Modify comment.
 *
 *********************************************************************/


#include <time.h>
#include "timer.hpp"


/*==========================================================================*/
/*	public Constructor \ Destructor											*/
/*==========================================================================*/

cm::Timer::
Timer()
	: _Status(TIMER_STOP)
	, _TimePaused(0)
	, _TimeInitial(0)
{}

cm::Timer::
~Timer()
{}


/*==========================================================================*/
/*	public Methods, Modify													*/
/*==========================================================================*/

void cm::Timer::
start(void)
{
	_Status = TIMER_RUNNINNG;
	_TimeInitial = clock();
	_TimePaused = 0;
}

void cm::Timer::
pause(void)
{
	if (_Status != TIMER_RUNNINNG)
		return;

	_Status = TIMER_PAUSED;
	_TimePaused = clock();
}

void cm::Timer::
resume(void)
{
	if (_Status != TIMER_PAUSED)
		return;

	_Status = TIMER_RUNNINNG;
	_TimeInitial += clock() - _TimePaused;
	_TimePaused = 0;
}

void cm::Timer::
stop(void)
{
	_Status = TIMER_STOP;
	_TimeInitial = 0;
	_TimePaused = 0;
}


/*==========================================================================*/
/*	public Methods, Query													*/
/*==========================================================================*/

unsigned int cm::Timer::
getElapsedTime(void) const
{
	switch (_Status) {
	case cm::Timer::TIMER_STOP:
		return 0;
		break;
	case cm::Timer::TIMER_RUNNINNG:
		return clock() - _TimeInitial;
		break;
	case cm::Timer::TIMER_PAUSED:
		return _TimePaused - _TimeInitial;
		break;
	default:
		return 0;
		break;
	}
}


