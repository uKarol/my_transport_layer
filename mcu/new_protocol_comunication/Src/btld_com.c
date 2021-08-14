/*
 * btld_com.c
 *
 *  Created on: 7 kwi 2021
 *      Author: Karol
 */

#include <stdint.h>
#include "btld_com.h"
#include "uart_interrupt.h"
#include "btld_timers.h"
#include "crc_32.h"
#include "message_decoder.h"

#define BTLD_MASTER_ID 0

volatile decoder_data_t decoder_data;
volatile message_struct_t processed_message;

void send_msg(uint8_t *data, uint8_t msg_length, uint16_t msg_id )
{
	uint8_t msg[128];
	uint32_t crc = crc_update(0xFFFFFFFF, data, msg_length);
	msg[0] = START_BYTE_CMD;
	msg[1] = (msg_id) & 0xFF;
	msg[2] = (msg_id >> 8) & 0xFF;
	msg[3] = (msg_length + 10) & 0xFF;
	msg[4] = ((msg_length + 10) >> 8) & 0xFF;
	memcpy(&(msg[5]), data, msg_length);
	msg[msg_length+5] = crc & 0xFF;
	msg[msg_length+6] = (crc >> 8)  & 0xFF;
	msg[msg_length+7] = (crc >> 16) & 0xFF;
	msg[msg_length+8] = (crc >> 24) & 0xFF;
	msg[msg_length+9] = STOP_BYTE;
	uart_send(msg, msg_length+10);
}

void retransmission_request()
{
	send_msg("ret_req_", 8, BTLD_MASTER_ID);
}

void send_acknowledge()
{
	uint8_t msg[8] = {0};
	msg[0] = TRANSMISSION_STATUS_FRAME;
	msg[1] = FRAME_ACK;
	send_msg(msg, 8, BTLD_MASTER_ID);
}

void frame_error_signalize(frame_error_t error_type)
{
	uint8_t msg[8] = {0};
	msg[0] = TRANSMISSION_STATUS_FRAME;
	msg[1] = FRAME_ERROR;
	msg[2] = error_type;
	send_msg(msg, 8, BTLD_MASTER_ID);
}

void reception_abort()
{
	decoder_data.current_state = BTLD_US_DATA_ERROR;
	uart_enable_idle_irq();
}

void clear_error()
{
	decoder_data.current_state = BTLD_US_START;
	decoder_data.byte_ctr = 0;
	processed_message.crc = 0;
	processed_message.msg_length = 0;
	processed_message.msg_id = 0;
	uart_disable_idle_irq();
}

void clear_state_variables()
{
	decoder_data.byte_ctr = 0;
	processed_message.crc = 0;
	processed_message.msg_length = 0;
	processed_message.msg_id = 0;
}

void btld_com_callback(uint8_t byte)
{
	reset_timer(&com_timer);
	switch(decoder_data.current_state)
	{
		case BTLD_US_START:
			if( byte == START_BYTE_CMD)
			{
				activate_timer(&com_timer, 10);
				decoder_data.current_state = BTLD_US_ID;
				clear_state_variables();
			}
			else
			{
				decoder_data.current_state = BTLD_US_DATA_ERROR;
				uart_enable_idle_irq();
			}
			break;
		case BTLD_US_ID: // ID is 2 byte (compatible with CAN 2.0)
			decoder_data.byte_ctr++;
			processed_message.msg_id = processed_message.msg_id << 8;
			processed_message.msg_id|= byte;
			if(decoder_data.byte_ctr == 2)
			{
				decoder_data.current_state = BTLD_US_LENGTH;
				decoder_data.byte_ctr = 0;
			}
			break;
		case BTLD_US_LENGTH: // MSG LENGTH IS 2 B
			decoder_data.byte_ctr++;
			processed_message.msg_length = processed_message.msg_length << 8;
			processed_message.msg_length|= byte;
			if(decoder_data.byte_ctr == 2)
			{
				decoder_data.current_state = BTLD_US_DATA;
				decoder_data.byte_ctr = 0;
			}
			break;
		case BTLD_US_DATA:
			processed_message.payload[decoder_data.byte_ctr] = byte;
			decoder_data.byte_ctr++;
			if(decoder_data.byte_ctr == processed_message.msg_length){
				decoder_data.current_state = BTLD_US_DATA_CRC;
				decoder_data.byte_ctr = 0;
			}
			break;
		case BTLD_US_DATA_CRC: // crc is 32 bit
			decoder_data.byte_ctr++;
			processed_message.crc = processed_message.crc << 8;
			processed_message.crc|= byte;
			if(decoder_data.byte_ctr == 4){
				decoder_data.current_state = BTLD_US_FINISH;
				decoder_data.byte_ctr = 0;
			}
			break;
		case BTLD_US_FINISH:
			stop_timer(&com_timer);
			if( byte == STOP_BYTE )
			{
				if( processed_message.crc == crc_update(0xFFFFFFFF, processed_message.payload, processed_message.msg_length) )
				{
					send_acknowledge();
					decode_message(&processed_message);
					decoder_data.current_state = BTLD_US_START;
				}
				else
				{
					frame_error_signalize(BAD_CRC);
				}
			}
			else
			{
				frame_error_signalize(TOO_LONG);
				decoder_data.current_state = BTLD_US_DATA_ERROR;
				uart_enable_idle_irq();
			}
			break;
		case BTLD_US_DATA_ERROR:
			break;
	}
}
