/*
 * message_decoder.c
 *
 *  Created on: 28 lip 2021
 *      Author: Karol
 */
#include "message_decoder.h"
#include "cyclic_buffer.h"
#include "btld_com.h"
#include "btld_tl.h"

#define COMMAND_ID 0
#define DATA_ID 1

cyclic_buffer command_buffer;

void send_btld_frame(btld_commands_t command, uint16_t id, uint32_t data)
{
	uint8_t msg[8];
	msg[0] = BOOTLOADER_FRAME;
	msg[1] = command;
	msg[2] = data & 0xFF;
	msg[3] = (data >>  8) & 0xFF;
	msg[4] = (data >> 16) & 0xFF;
	msg[5] = (data >> 24) & 0xFF;
	send_msg(msg, 8, COMMAND_ID);
}

command_status_t get_command(uint8_t *command, uint32_t timeout){
	uint32_t prev_tick = HAL_GetTick();
	while(buffer_get_busy(&command_buffer) < 8)
	{
		if(HAL_GetTick() - prev_tick > timeout) return COMMAND_TIMEOUT;
	}
	buffer_get(&command_buffer, command, 8);
	return COMMAND_OK;
}

void command_buffer_init()
{
	command_buffer.head_ptr = 1;
	command_buffer.tail_ptr = 0;
	command_buffer.max_length = 255;
	command_buffer.mode = BUFFER_ACTIVE;
}

void decode_message(message_struct_t *msg)
{
	if(msg->msg_id == COMMAND_ID)
	{
		if(msg->payload[0] == BOOTLOADER_FRAME)
		{
			buffer_put(&command_buffer, msg->payload, msg->msg_length);
		}
		else
		{
			frame_error_signalize(UNKNOWN_FRAME);
		}
	}
	else if(msg->msg_id == DATA_ID)
	{
		btld_tl_put(msg->payload, msg->msg_length, msg->msg_id);
	}
	else
	{
		frame_error_signalize(UNKNOWN_FRAME);
	}
}
