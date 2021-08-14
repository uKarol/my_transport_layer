/*
 * btld_timers.c
 *
 *  Created on: 2 maj 2021
 *      Author: Karol
 */
#include "stdlib.h"
#include "btld_timers.h"
#include "btld_com.h"

volatile btld_timer com_timer 	   = {.ctr = 0, .timeout = 10, .timer_state = TIMER_NOT_ACTIVE, .callback = NULL};  // counts time between consecutive bytes
volatile btld_timer protocol_timer = {.ctr = 0, .timeout = 10, .timer_state = TIMER_NOT_ACTIVE, .callback = NULL};  // counts time between packages
volatile btld_timer wait_timer     = {.ctr = 0, .timeout = 100, .timer_state = TIMER_NOT_ACTIVE, .callback = NULL};

void activate_timer(btld_timer *timer, uint32_t timeout)
{
	timer->timeout = timeout;
	timer->timer_state = TIMER_ACTIVE;
	timer->ctr=0;
}

void assing_callback(btld_timer *timer, void *callback)
{
	timer->callback = callback;
}

void reset_timer(btld_timer *timer)
{
	timer->ctr=0;
}

void stop_timer(btld_timer *timer)
{
	timer->timer_state = TIMER_NOT_ACTIVE;
}
void update_timers()
{
	void (*action)();
	if( com_timer.timer_state == TIMER_ACTIVE )
	{
		com_timer.ctr++;								// increase counter if timer is active
		if( com_timer.ctr == com_timer.timeout )
		{
			com_timer.timer_state = TIMER_NOT_ACTIVE;   // deactivate timer
			reception_abort();							// abort bytes reception and send retransmission request
		}
	}

	if( protocol_timer.timer_state == TIMER_ACTIVE )
	{
		protocol_timer.ctr++;
		if( protocol_timer.ctr == protocol_timer.timeout )
		{
			protocol_timer.timer_state = TIMER_NOT_ACTIVE;
			reception_abort();
		}
	}

	if( wait_timer.timer_state == TIMER_ACTIVE )
	{
		wait_timer.ctr++;
		if( wait_timer.ctr == wait_timer.timeout )
		{
			wait_timer.ctr = 0;
			action = wait_timer.callback;
			action();
		}
	}

}




