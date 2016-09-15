/*********************************************************************
 *	@file		timer.hpp
 *	@brief		A simple timer, for timing and so on.
 *
 *	Date        Name        Description
 *	23/11/14	MYLS		Creation.
 *
 *********************************************************************/

#pragma once

namespace cm {
	class timer;
}

/**
 *	\brief	A timer, just for timing;
 */
class cm::timer {

public:

	/**
	 *	\brief Constructor
	 */
	timer();

	/**
	 *	\brief	Destructor
	 */
	~timer();

public:

	/**
	 *	\brief	Clear and start timer
	 */
	void start(void);

	/**
	 *	\brief	pause timer
	 *			While running
	 */
	void pause(void);

	/**
	 *	\brief	resume timer
	 *			while paused
	 */
	void resume(void);

	/**
	 *	\brief	Reset the timer
	 */
	void stop(void);

	/**
	 *	\brief	Get Elapsed Time...
	 *
	 *	\return	time in millisecond.
	 */
	unsigned int get_elapsed_time(void) const;

private:

	/**
	 *	\brief	a enum representative timer status
	 */
	enum timer_status : unsigned char {
		TIMER_STOP,											/**< status stop */
		TIMER_RUNNINNG,										/**< status running */
		TIMER_PAUSED										/**< status paused */
	};

	timer_status status;									/**< timer's Status */
	unsigned int time_paused;								/**< time when pause timer */
	unsigned int time_initial;								/**< time when start timer */
};
