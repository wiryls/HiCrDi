/*********************************************************************
 *
 *  @file       timer.cpp
 *  @brief      Implement of class timer
 *
 *
 *  Date        Name        Description
 *  23/11/14    MYLS        Creation.
 *  08/12/14    MYLS        Modify comment.
 *
 *********************************************************************/


#include <time.h>
#include "timer.hpp"


/*==========================================================================*/
/*  public Constructor \ Destructor                                         */
/*==========================================================================*/

cm::timer::
timer()
    : status(TIMER_STOP)
    , time_paused(0)
    , time_initial(0)
{}

cm::timer::
~timer()
{}


/*==========================================================================*/
/*  public Methods, Modify                                                  */
/*==========================================================================*/

void cm::timer::
start(void)
{
    status = TIMER_RUNNINNG;
    time_initial = clock();
    time_paused = 0;
}

void cm::timer::
pause(void)
{
    if (status != TIMER_RUNNINNG)
        return;

    status = TIMER_PAUSED;
    time_paused = clock();
}

void cm::timer::
resume(void)
{
    if (status != TIMER_PAUSED)
        return;

    status = TIMER_RUNNINNG;
    time_initial += clock() - time_paused;
    time_paused = 0;
}

void cm::timer::
stop(void)
{
    status = TIMER_STOP;
    time_initial = 0;
    time_paused = 0;
}


/*==========================================================================*/
/*  public Methods, Query                                                   */
/*==========================================================================*/

unsigned int cm::timer::
get_elapsed_time(void) const
{
    switch (status) {
    case cm::timer::TIMER_STOP:
        return 0;
        break;
    case cm::timer::TIMER_RUNNINNG:
        return clock() - time_initial;
        break;
    case cm::timer::TIMER_PAUSED:
        return time_paused - time_initial;
        break;
    default:
        return 0;
        break;
    }
}


