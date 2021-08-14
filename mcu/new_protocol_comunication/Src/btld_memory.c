/*
 * btld_memory.c
 *
 *  Created on: 8 sie 2021
 *      Author: Karol
 */

#include "btld_memory.h"
#include "main.h"

#define FLASH_FKEY1 0x45670123
#define FLASH_FKEY2 0xCDEF89AB

uint32_t sectors_size[8] = {16384, 16384, 16384, 16384, 65636, 131072, 131072, 131072};
uint32_t sectors_start_address[8] = {0x08000000, 0x08004000, 0x08008000,  0x0800C000, 0x08010000, 0x08020000, 0x08040000, 0x08060000};

void flash_unlock (){

	// Wait for the flash memory not to be busy
	while ((FLASH->SR & FLASH_SR_BSY) != 0 );
	// Check if the controller is unlocked already
	if ((FLASH->CR & FLASH_CR_LOCK) != 0 ){
		// Write the first key
		FLASH->KEYR = FLASH_FKEY1;
		// Write the second key
		FLASH->KEYR = FLASH_FKEY2;
		FLASH->CR |= FLASH_CR_EOPIE;
	}

}

btld_flash_status_t flash_erase_sector( uint8_t sector )
{
	FLASH->CR |= FLASH_CR_SER; // Page erase operation
	FLASH->CR &= ~((uint32_t)(FLASH_CR_SNB_0|FLASH_CR_SNB_1|FLASH_CR_SNB_2|FLASH_CR_SNB_3|FLASH_CR_SNB_4));     // Set the address to the page to be written
	FLASH->CR |= sector << FLASH_CR_SNB_Pos;
	FLASH->CR |= FLASH_CR_STRT;// Start the page erase


	// Wait until page erase is done
	while ((FLASH->SR & FLASH_SR_BSY) != 0);
	// If the end of operation bit is set...
	if ((FLASH->SR & FLASH_SR_EOP) != 0){
	    // Clear it, the operation was successful
	    FLASH->SR |= FLASH_SR_EOP;
	    FLASH->CR &= ~FLASH_CR_SER;
	    return BTLD_FLASH_OK;
	}
	//Otherwise there was an error
	else{
		FLASH->CR &= ~FLASH_CR_SER;
	    // Manage the error cases
		return BTLD_FLASH_ERROR;
	}
	// Get out of page erase mode
}
btld_flash_status_t flash_write_word(uint32_t address, uint32_t word)
{
	FLASH->CR |= (FLASH_CR_PG | FLASH_PSIZE_WORD);     // Programing mode 32 bits

	*(__IO uint32_t*)(address) = word;       // Write data

	// Wait until the end of the operation
	while ((FLASH->SR & FLASH_SR_BSY) != 0);
	// If the end of operation bit is set...
	if ((FLASH->SR & FLASH_SR_EOP) != 0){
	    // Clear it, the operation was successful
	     FLASH->SR |= FLASH_SR_EOP;
	     FLASH->CR &= ~FLASH_CR_PG;
	     return BTLD_FLASH_OK;
	}
	//Otherwise there was an error
	else{
		FLASH->CR &= ~FLASH_CR_PG;
		return BTLD_FLASH_ERROR;
	}
}

void flash_lock()
{
	FLASH->CR |= FLASH_CR_LOCK;
	FLASH->CR &= ~FLASH_CR_EOPIE;
}

uint32_t flash_get_sector_size(uint8_t sector)
{
	return sectors_size[sector];
}

uint32_t flash_get_sector_start_address(uint8_t sector)
{
	return sectors_start_address[sector];
}
