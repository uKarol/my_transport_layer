/*
 * message_decoder.h
 *
 *  Created on: 28 lip 2021
 *      Author: Karol
 */

#ifndef INC_MESSAGE_DECODER_H_
#define INC_MESSAGE_DECODER_H_

#include "cyclic_buffer.h"

typedef enum
{
    TRANSMISSION_STATUS_FRAME = 0x0,
    TRANSPORT_LAYER_FRAME = 0x1,
    BOOTLOADER_FRAME = 0x2,
	UNKNOWN_FRAME = 0x3
} frame_type_t;

typedef enum
{
	BTLD_ACTIVATE = 0x0, 	// + ID
	BTLD_PROGRAM_SIZE, 		// + PROGRAM SIZE
	BTLD_ACK,
	BTLD_ERROR, 			// + ERROR ID
	BTLD_CRC,				// + CRC of memory
	BTLD_FINISH,
	BTLD_ABORT
} btld_commands_t;

typedef enum
{
    FRAME_ACK = 0x0,
    FRAME_ERROR = 0x1
} transmission_frame_t;

typedef enum
{
    FLOW_CONTROL_FRAME = 0x0
} transport_layer_frame_t;

typedef enum
{
    FC_CTS = 0x0,
    FC_WAIT = 0x1,
    FC_ERROR = 0x2,
    FC_FINISH = 0x3
} flow_control_type;

typedef enum
{
    TOO_LONG = 0x0,
    TOO_SHORT = 0x1,
    BAD_CRC = 0x2,
    NO_START_BYTE = 0x3
} frame_error_t;

typedef enum
{
	COMMAND_OK = 0x0,
	COMMAND_TIMEOUT = 0x1
} command_status_t;

extern cyclic_buffer command_buffer;
command_status_t get_command(uint8_t *command, uint32_t timeout);
//void decode_message(message_struct_t *msg);

#endif /* INC_MESSAGE_DECODER_H_ */
