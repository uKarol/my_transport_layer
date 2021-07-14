#pragma once

#include <stdint.h>

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
    uint16_t data_size;
    uint16_t bytes_sent;
    uint16_t bytes_received;
    uint16_t buffer_capacity;
    btld_tl_state_t tl_state;
}btld_tl_data_t;

void btld_tl_init();
uint8_t btld_tl_get_data(uint16_t data_size);
btld_tl_ret_t btld_tl_put(uint8_t* data, uint8_t length);
void btld_tl_start_reception(uint16_t packet_size);
void btld_tl_finish_reception(void);