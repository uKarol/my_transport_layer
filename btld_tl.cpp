#include "btld_tl.h"
#include "cycle_buffer.h"
#include "stdio.h"

btld_tl_data_t btld_tl_data;
cyclic_buffer tl_buffer;

#define TL_MESSAGE_OFFSET 0
#define QUANTUM_MESSAGE_SIZE 8

typedef enum {
    TL_OK = TL_MESSAGE_OFFSET,
    TL_NOT_ACTIVE,
    TL_BUFFER_FULL,
    TL_WAIT,
    TL_BUSY,
    TL_FINISH,
    TL_FAILURE
}tl_status_codes_t;

void send_flow_control(tl_status_codes_t fc_type, uint32_t data)
{
    printf("flow control ");
    switch(fc_type)
    {
        case TL_BUFFER_FULL:
            printf("TL_BUFFER_FULL, %d\n", data);
            break;
        case TL_NOT_ACTIVE:
            printf("TL_BUFFER_NOT_ACTIVE, %d\n", data);
            break;
        case TL_WAIT:
            printf("TL_BUFFER_WAIT, %d\n", data);
            break;
        case TL_OK:
            printf("TL_BUFFER_OK, %d\n", data);
            break;
        case TL_FAILURE:
            printf("TL_FAILURE, %d\n", data);
            break;
        case TL_BUSY:
            printf("TL_BUSY, %d\n", data);
            break;
    }
}

void transmission_stop()
{
    printf("transmission stopped \n");
}

btld_tl_ret_t btld_tl_put(uint8_t* data, uint8_t length) 
{
    if (btld_tl_data.tl_state == BTLD_TL_NOT_ACTIVE) 
    {
        send_flow_control(TL_NOT_ACTIVE, 0);
        return BTLD_TL_NOT_OK;
    }
    else if (btld_tl_data.tl_state == BTLD_TL_WAIT) 
    {
        send_flow_control(TL_WAIT, 0);
        return BTLD_TL_NOT_OK;
    }
    else 
    {
        if (buffer_put(&tl_buffer, data, length ) == BUFFER_OK){
            btld_tl_data.buffer_capacity = btld_tl_data.buffer_capacity - length;
            btld_tl_data.bytes_received = btld_tl_data.bytes_received + length;
            if(btld_tl_data.bytes_received == btld_tl_data.data_size){
                btld_tl_finish_reception();
            }
            else if(btld_tl_data.buffer_capacity < QUANTUM_MESSAGE_SIZE)
            {
                send_flow_control(TL_WAIT, 0);
            }
            else
            {
                send_flow_control(TL_OK, btld_tl_data.buffer_capacity);
            }
        }
        else {
            send_flow_control(TL_BUFFER_FULL,0);
        }
    }
}

uint8_t btld_tl_get_data(uint8_t *data, uint16_t data_size) 
{
    uint8_t stored_data_size;
    stored_data_size = buffer_get_busy(&tl_buffer);
    btld_tl_data.buffer_capacity = stored_data_size;
    return stored_data_size;
}

/*
    Funtion used to start reception 
*/
void btld_tl_start_reception(uint16_t packet_size) 
{
    if(btld_tl_data.tl_state == BTLD_TL_PROCESSING){
        send_flow_control(TL_BUSY, 0);
        return;
    }
    uint16_t free_space;
    tl_buffer.mode = BUFFER_ACTIVE; 
    btld_tl_data.tl_state = BTLD_TL_PROCESSING;
    free_space = buffer_get_free(&tl_buffer);
    btld_tl_data.buffer_capacity = free_space;
    btld_tl_data.bytes_received = 0;
    btld_tl_data.data_size = packet_size;
    if (free_space < packet_size) 
    {
        send_flow_control(TL_WAIT,0);
    }
    else
    {
        send_flow_control(TL_OK, free_space);
    }
}

void btld_tl_finish_reception(void) 
{
    printf("reception finished\n");
}

void btld_tl_init() {
    tl_buffer.head_ptr = 1;
    tl_buffer.tail_ptr = 0;
    tl_buffer.max_length = 10;
    tl_buffer.mode = BUFFER_NOT_ACTIVE;
}