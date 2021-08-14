/*
 * crc_32.h
 *
 *  Created on: 3 maj 2021
 *      Author: Karol
 */

#ifndef INC_CRC_32_H_
#define INC_CRC_32_H_

#include "stdint.h"

uint32_t crc_update(uint32_t crc, const void *data, uint32_t data_len);



#endif /* INC_CRC_32_H_ */
