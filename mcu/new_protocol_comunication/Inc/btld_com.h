/*
 * btld_com.h
 *
 *  Created on: 9 kwi 2021
 *      Author: Karol
 */

#ifndef INC_BTLD_COM_H_
#define INC_BTLD_COM_H_

#include "stdint.h"
#include "message_decoder.h"

#define SOFT_TIMEOUT 10
#define START_BYTE_CMD 0xAB
#define START_BYTE_DATA 0xA5
#define STOP_BYTE 0xCD
#define MAX_PAYLOAD_LENGTH 255

typedef enum {
	BTLD_US_START,
	BTLD_US_ID,
	BTLD_US_LENGTH,
	BTLD_US_DATA_CRC,
	BTLD_US_DATA,
	BTLD_US_FINISH,
	BTLD_US_DATA_ERROR
} uart_serializer_state_t;

typedef struct{
	uint8_t payload[MAX_PAYLOAD_LENGTH];
	uint8_t msg_length;
	uint8_t msg_id;
	uint32_t crc;
} message_struct_t;

typedef struct{
	uart_serializer_state_t current_state;
	uint16_t byte_ctr;
}decoder_data_t;

void retransmission_request(void);
void send_acknowledge(void);
void clear_error(void);
void btld_com_callback( uint8_t byte);
void reception_abort();
void send_msg(uint8_t *data, uint8_t msg_length, uint16_t msg_id );
void frame_error_signalize(frame_error_t error_type);

#endif /* INC_BTLD_COM_H_ */
