/*
 * btld_tl.c
 *
 *  Created on: 28 lip 2021
 *      Author: Karol
 */

#include "btld_tl.h"
#include "cyclic_buffer.h"
#include "message_decoder.h"
#include "btld_com.h"
#include "btld_timers.h"

#define BOARD_DATA_ID 0
#define BOARD_COMMAND_ID 1
#define FLOW_CONTROL_PREFIX 0xFC


btld_tl_data_t btld_tl_data;
btld_tl_tx_data_t btld_tl_tx_data;
cyclic_buffer tl_buffer;
uint8_t number_of_cb_call = 0;

void transmission_stop(uint8_t error_code)
{

}

void send_flow_control(flow_control_type fc_type, uint32_t data, uint16_t id)
{
	uint8_t msg[8];
	msg[0] = TRANSPORT_LAYER_FRAME;
	msg[1] = FLOW_CONTROL_FRAME;
	msg[2] = fc_type;
	msg[3] = 'C';
	msg[4] = data & 0xFF;
	msg[5] = (data<< 8) & 0xFF;
	msg[6] = (data<<16) & 0xFF;
	msg[7] = (data<<24) & 0xFF;
	send_msg(msg, 8, id);
}

void tl_timer_activate()
{
	activate_timer(&wait_timer, 100);
}

void tl_timer_deactivate()
{
	stop_timer(&wait_timer);
}

void tl_time_elapsed_callback()
{
    uint16_t free_space;
    number_of_cb_call++;
    if(number_of_cb_call > CB_CALL_LIMIT){
        transmission_stop(0);
        send_flow_control(FC_ERROR, 0, btld_tl_data.data_id);
        tl_timer_deactivate();
    }
    else
    {
        free_space = buffer_get_free(&tl_buffer);
        if(free_space > 128) free_space = 128;
        btld_tl_data.buffer_capacity = free_space;
        if (btld_tl_data.buffer_capacity >= QUANTUM_MESSAGE_SIZE)
        {
            send_flow_control(FC_CTS, free_space, btld_tl_data.data_id);
            btld_tl_data.tl_state = BTLD_TL_PROCESSING;
            number_of_cb_call = 0;
            tl_timer_deactivate();
        }
        else
        {
            send_flow_control(FC_WAIT, 0, btld_tl_data.data_id);
        }
    }
}


btld_tl_ret_t btld_tl_put(uint8_t* data, uint8_t length, uint8_t data_id)
{
    uint16_t free_space;

     if(data_id != btld_tl_data.data_id){
    	return BTLD_TL_NOT_OK;
    }
    else if (btld_tl_data.tl_state == BTLD_TL_NOT_ACTIVE)
    {
        send_flow_control(FC_ERROR, 0, btld_tl_data.data_id);
        return BTLD_TL_NOT_OK;
    }
    else if (btld_tl_data.tl_state == BTLD_TL_WAIT)
    {
        send_flow_control(FC_WAIT, 10, btld_tl_data.data_id);
        return BTLD_TL_NOT_OK;
    }
    else
    {
        if (buffer_put(&tl_buffer, data, length ) == BUFFER_OK){
            btld_tl_data.buffer_capacity = btld_tl_data.buffer_capacity - length;
            btld_tl_data.bytes_received = btld_tl_data.bytes_received + length;
            if(btld_tl_data.bytes_received >= btld_tl_data.data_size){
                btld_tl_finish_reception();
                return BTLD_TL_OK;
            }
            else if(btld_tl_data.buffer_capacity < QUANTUM_MESSAGE_SIZE)
            {
                free_space = buffer_get_free(&tl_buffer);
                if(free_space > 128) free_space = 128;
                btld_tl_data.buffer_capacity = free_space;

                if(btld_tl_data.buffer_capacity >= QUANTUM_MESSAGE_SIZE){
                    send_flow_control(FC_CTS, btld_tl_data.buffer_capacity, btld_tl_data.data_id);
                    return BTLD_TL_OK;
                }
                else {
                    btld_tl_data.tl_state = BTLD_TL_WAIT;
                    tl_timer_activate();
                    send_flow_control(FC_WAIT, 0, btld_tl_data.data_id);
                    return BTLD_TL_OK;
                }
            }
            else
            {
                //everything is ok
                send_flow_control(FC_CTS, btld_tl_data.buffer_capacity, btld_tl_data.data_id);
                return BTLD_TL_OK;
            }
        }
        else {
            send_flow_control(FC_ERROR,0, btld_tl_data.data_id);
            return BTLD_TL_NOT_OK;
        }
    }
}

uint8_t btld_tl_get_data(uint8_t *data, uint16_t data_size)
{
	uint32_t prev_tick = HAL_GetTick();
    while( buffer_get_busy(&tl_buffer) < data_size)
    {

    }
    buffer_get(&tl_buffer, data, data_size);
    return data_size;

}

/*
    Funtion used to start reception
*/
void btld_tl_start_reception(uint32_t packet_size, uint16_t data_id, uint16_t command_id)
{
	btld_tl_data.data_id = data_id;
	btld_tl_data.command_id = command_id;
    if(btld_tl_data.tl_state == BTLD_TL_PROCESSING){
        send_flow_control(FC_ERROR, 0, btld_tl_data.data_id);
        return;
    }
    uint16_t free_space;
    tl_buffer.mode = BUFFER_ACTIVE;
    btld_tl_data.tl_state = BTLD_TL_PROCESSING;
    free_space = buffer_get_free(&tl_buffer);
    if(free_space > 128) free_space = 128;
    btld_tl_data.buffer_capacity = free_space;
    btld_tl_data.bytes_received = 0;
    btld_tl_data.data_size = packet_size;
    if (free_space < QUANTUM_MESSAGE_SIZE)
    {
        send_flow_control(FC_WAIT,0, btld_tl_data.data_id);
    }
    else
    {
        send_flow_control(FC_CTS, free_space, btld_tl_data.data_id);
    }
}

void btld_tl_finish_reception(void)
{
	send_flow_control(FC_FINISH, 0, btld_tl_data.data_id);
}

void btld_tl_init()
{
    tl_buffer.head_ptr = 1;
    tl_buffer.tail_ptr = 0;
    tl_buffer.max_length = 512;
    tl_buffer.mode = BUFFER_NOT_ACTIVE;
    assing_callback(&wait_timer, tl_time_elapsed_callback);
}


void can_send(uint16_t ID, uint8_t* data) {

}

void activate_wait_timer()
{

}
void deactivate_wait_timer()
{

}
void reset_wait_timer()
{

}

void abort_transmission()
{

}

void btld_tl_enter_wait()
{
    if (btld_tl_tx_data.tl_tx_state == BTLD_TL_PROCESSING)
    {
        btld_tl_tx_data.tl_tx_state = BTLD_TL_WAIT;
        activate_wait_timer();
    }
    else if (btld_tl_tx_data.tl_tx_state == BTLD_TL_WAIT )
    {
        reset_wait_timer();
    }
    else
    {
        /*do nothing*/
    }
}

void btld_ack_from_slave(uint16_t size){

    if (btld_tl_tx_data.tl_tx_state == BTLD_TL_WAIT)
    {
        deactivate_wait_timer();
        btld_tl_tx_data.target_capacity = size;
        btld_tl_tx_data.tl_tx_state = BTLD_TL_PROCESSING;
    }
    else
    {
        /*do nothing*/
    }
}

void flow_control_processing(flow_control_type fc_type, uint16_t value)
{
    switch (fc_type) {
    case FC_CTS:
        btld_ack_from_slave(value);
        break;
    case FC_WAIT:
        btld_tl_enter_wait();
        break;
    case FC_FINISH:
        break;
    case FC_ERROR:
        break;
    }
}

void btld_tl_transmission_finished() {
    btld_tl_tx_data.tl_tx_state = BTLD_TL_NOT_ACTIVE;
}

void btld_tl_transmit_data(uint8_t *data) {
    if(btld_tl_tx_data.tl_tx_state == BTLD_TL_PROCESSING){
        can_send(btld_tl_tx_data.can_id, data);
        btld_tl_tx_data.data_sent = btld_tl_tx_data.data_sent + 8;
        btld_tl_tx_data.target_capacity = btld_tl_tx_data.target_capacity - 8;
        if(btld_tl_tx_data.data_sent == btld_tl_tx_data.data_size){
            btld_tl_transmission_finished();
        }
        else if(btld_tl_tx_data.target_capacity < 8){
            btld_tl_enter_wait();
        }
    }
    else{

    }
}

void btld_tl_can_start_transmission(uint16_t ff_id, uint16_t id, uint32_t data_size){

    uint8_t temp_data[8] = {0,0,0,0,0,0,0,0};
    if( btld_tl_tx_data.tl_tx_state == BTLD_TL_PROCESSING ){
        return;
    }
    else {
        btld_tl_tx_data.can_id = id;
        btld_tl_tx_data.data_sent = 0;
        btld_tl_tx_data.data_size = data_size;
        btld_tl_tx_data.tl_tx_state = BTLD_TL_WAIT;
    }
    temp_data[1] = data_size >> 8;
    temp_data[0] = data_size & 0xFF;
    can_send(ff_id, temp_data);
}
