/*
 * btld_memory.h
 *
 *  Created on: 8 sie 2021
 *      Author: Karol
 */

#ifndef INC_BTLD_MEMORY_H_
#define INC_BTLD_MEMORY_H_

#include "stdint.h"

typedef enum
{
	BTLD_FLASH_OK,
	BTLD_FLASH_ERROR
} btld_flash_status_t;

btld_flash_status_t flash_erase_sector( uint8_t sector );
btld_flash_status_t flash_write_word(uint32_t address, uint32_t word);
uint32_t flash_get_sector_size(uint8_t sector);
uint32_t flash_get_sector_start_address(uint8_t sector);
void flash_lock();
void flash_unlock ();

#endif /* INC_BTLD_MEMORY_H_ */
