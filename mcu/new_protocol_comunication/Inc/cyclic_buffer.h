/*
 * cyclic_buffer.h
 *
 *  Created on: 23 cze 2021
 *      Author: Karol
 */

#ifndef INC_CYCLIC_BUFFER_H_
#define INC_CYCLIC_BUFFER_H_

#define MAX_SIZE 520
#include "stdint.h"
typedef enum {
    BUFFER_OK,
    BUFFER_EMPTY,
    BUFFER_FULL,
    BUFFER_ERROR
} buffer_state;

typedef enum {
    BUFFER_ACTIVE,
    BUFFER_NOT_ACTIVE,
} buffer_mode;

typedef struct{
    uint8_t buffer[MAX_SIZE];
    uint16_t tail_ptr;
    uint16_t head_ptr;
    buffer_mode mode;
    uint16_t max_length;
} cyclic_buffer;

uint16_t buffer_get_free(cyclic_buffer * buffer);
uint16_t buffer_get_busy(cyclic_buffer * buffer);
buffer_state buffer_put(cyclic_buffer * buffer, uint8_t *data_in, uint16_t data_length );
buffer_state buffer_get(cyclic_buffer *buffer, uint8_t *data_out, uint16_t data_length );


#endif /* INC_CYCLIC_BUFFER_H_ */
