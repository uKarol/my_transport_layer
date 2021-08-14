/*
 * btld_timers.h
 *
 *  Created on: 2 maj 2021
 *      Author: Karol
 */

#ifndef INC_BTLD_TIMERS_H_
#define INC_BTLD_TIMERS_H_

#include "stdint.h"

typedef enum {
	TIMER_ACTIVE,
	TIMER_NOT_ACTIVE
} timer_state_t;

typedef struct {
	uint32_t ctr;
	uint32_t timeout;
	timer_state_t timer_state;
	void *callback;
} btld_timer;

extern volatile btld_timer com_timer;
extern volatile btld_timer protocol_timer;
extern volatile btld_timer wait_timer;

void activate_timer(btld_timer *timer, uint32_t timeout);
void stop_timer(btld_timer *timer);
void update_timers();
void reset_timer(btld_timer *timer);
void assing_callback(btld_timer *timer, void *callback);




#endif /* INC_BTLD_TIMERS_H_ */
