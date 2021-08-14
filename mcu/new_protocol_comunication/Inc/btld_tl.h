/*
 * btld_tl.h
 *
 *  Created on: 28 lip 2021
 *      Author: Karol
 */

#ifndef INC_BTLD_TL_H_
#define INC_BTLD_TL_H_

#include <stdint.h>
#include "message_decoder.h"
#define TL_MESSAGE_OFFSET 0
#define QUANTUM_MESSAGE_SIZE 64

#define CB_CALL_LIMIT 5

typedef enum {
    BTLD_TL_OK,
    BTLD_TL_NOT_OK
}btld_tl_ret_t;

typedef enum {
    BTLD_TL_NOT_ACTIVE,
    BTLD_TL_PROCESSING,
    BTLD_TL_WAIT
} btld_tl_state_t;

typedef struct {
    uint32_t data_size;
    uint16_t data_id;
    uint16_t command_id;
    uint32_t bytes_received;
    uint16_t buffer_capacity;
    btld_tl_state_t tl_state;
}btld_tl_data_t;

typedef struct {
    uint32_t data_size;
    uint32_t data_sent;
    uint16_t can_id;
    uint16_t target_capacity;
    btld_tl_state_t tl_tx_state;
} btld_tl_tx_data_t;

void btld_tl_init();
uint8_t btld_tl_get_data(uint8_t* data, uint16_t data_size);
btld_tl_ret_t btld_tl_put(uint8_t* data, uint8_t length, uint8_t data_id);
void btld_tl_start_reception(uint32_t packet_size, uint16_t data_id, uint16_t command_id);
void btld_tl_finish_reception(void);
void tl_time_elapsed_callback();
void send_flow_control(flow_control_type fc_type, uint32_t data, uint16_t id);
void btld_tl_can_start_transmission(uint16_t ff_id, uint16_t id, uint32_t data_size);
void btld_tl_transmit_data(uint8_t* data);
void flow_control_processing(flow_control_type fc_type, uint16_t value);

#endif /* INC_BTLD_TL_H_ */
