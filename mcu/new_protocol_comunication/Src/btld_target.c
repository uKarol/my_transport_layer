/*
 * btld_target.c
 *
 *  Created on: 24 cze 2021
 *      Author: Karol
 */

#include "btld_com.h"
#include "btld_timers.h"
#include "btld_tl.h"
#include "cyclic_buffer.h"
#include "btld_target.h"
#include "message_decoder.h"
#include "btld_memory.h"

#define START_SECTOR 2

typedef enum
{
	BTLD_INITIALIZE,
	BTLD_ERASE_MEMORY,
	BTLD_FLASHING,
	BTLD_FINALIZE
} btld_state_t;

typedef struct
{
	uint32_t program_size;
	uint32_t written_bytes;
	btld_state_t current_state;
	uint8_t current_sector;
	uint32_t bytes_to_sector_end;
	uint32_t current_address;
} btld_target_data_t;

btld_target_data_t btld_data;

void reset_state_variables()
{
	btld_data.current_sector = START_SECTOR;
	btld_data.program_size = 0;
	btld_data.written_bytes = 0;
	btld_data.current_state = BTLD_INITIALIZE;
	btld_data.bytes_to_sector_end = 0;
	btld_data.current_address = flash_get_sector_start_address(START_SECTOR);
}

void btld_target()
{
	uint32_t word;
	uint8_t command[8];
	uint8_t data[8];
	command_status_t ret_val;
	reset_state_variables();
	btld_tl_init();
	flash_unlock();
	while(1)
	{
		switch(btld_data.current_state)
		{
			case BTLD_INITIALIZE:
				send_btld_frame(BTLD_ACK, 1, 0);
				  ret_val = get_command(command, 1000);
				  if((ret_val == COMMAND_OK) && (command[1] == BTLD_PROGRAM_SIZE))
				  {
					  btld_data.program_size = *((uint32_t*)(command+2));
					  send_btld_frame(BTLD_ACK, 1, 0);
					  btld_data.current_state = BTLD_ERASE_MEMORY;
					  btld_tl_start_reception(btld_data.program_size, 1, 0);
				  }
				  else
				  {
					  return;
				  }
				break;
			case BTLD_ERASE_MEMORY:
				flash_erase_sector(btld_data.current_sector);
				btld_data.bytes_to_sector_end = flash_get_sector_size(btld_data.current_sector);
				btld_data.current_sector++;
				btld_data.current_state = BTLD_FLASHING;
				break;

			case BTLD_FLASHING:
				btld_tl_get_data(data, 4);
				word = *(uint32_t*)(data);
				flash_write_word(btld_data.current_address, word);
				btld_data.current_address += 4;
				btld_data.bytes_to_sector_end -= 4;
				btld_data.written_bytes += 4;

				if(btld_data.written_bytes >= btld_data.program_size)
				{
					btld_data.current_state = BTLD_FINALIZE;
				}
				else if(btld_data.bytes_to_sector_end == 0)
				{
					btld_data.current_state = BTLD_ERASE_MEMORY;
				}
				break;

			case BTLD_FINALIZE:
				flash_lock();
				return;
				break;
		}
	}


}
