/*
 * ad7771.h
 *
 *  Created on: May 4, 2024
 *      Author: Ryan Eng
 *
 * 	SPI Driver header for AD7771 usage in high resolution mode
 */
#include "stm32h7xx_hal.h"

#ifndef INC_AD7771_H_
#define INC_AD7771_H_

//AD7771 definitions
#define CONF_REG1 0x011
#define CONF_REG2 0x012
#define CONF_REG3 0x013
#define CONF_REG1_DEFAULT 0x24
#define CONF_REG2_DEFAULT 0x09
#define CONF_REG3_DEFAULT 0x80
#define WRITE_MASK 0x00
#define READ_MASK 0x80
#define HIGH_RES_MASK 0x40
#define START_SPI_TRANSFER_MASK 0x10

uint8_t ad7771_init(SPI_HandleTypeDef *spiHandle, uint8_t* txBufferDMA, uint8_t* rxBufferDMA);
uint8_t ad7771_callback(SPI_HandleTypeDef *spiHandle, uint8_t* txBufferDMA, uint8_t* rxBufferDMA);

#endif /* INC_AD7771_H_ */
