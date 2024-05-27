/*
 * ad7771.c
 *
 *  Created on: May 4, 2024
 *      Author: rye20
 */


#include "ad7771.h"
#include "main.h"

struct a {
	uint32_t x : 1;
};

uint8_t ad7771_init(SPI_HandleTypeDef *spiHandle, uint8_t* txBufferDMA, uint8_t* rxBufferDMA) {
  //init AD7771 to start transferring over SPI
  uint8_t txBuffer[2];
  uint8_t rxBuffer[8];

  /*spi write to general user config 1 (0x011) to change to high resolution mode
   * change bit 6 to 1, reset = 0x24
   */
  //write is active low
  txBuffer[0] = CONF_REG1 | WRITE_MASK;
  txBuffer[1] = CONF_REG1_DEFAULT | HIGH_RES_MASK;
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(spiHandle, txBuffer, rxBuffer, 2, 1000);
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(10);

  /*spi write to general user config 3 (0x013) to change to spi slave mode
   *change bit 4 to 1, reset = 0x80
   */
  txBuffer[0] = CONF_REG3 | WRITE_MASK;
  txBuffer[1] = CONF_REG3_DEFAULT | START_SPI_TRANSFER_MASK;
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(spiHandle, txBuffer, rxBuffer, 2, 1000);
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(10);

  //after this data starts transmitting, start DMA transmissions
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(spiHandle, txBufferDMA, rxBufferDMA, 4);
  return 0;
}


uint8_t ad7771_callback(SPI_HandleTypeDef *spiHandle, uint8_t* txBufferDMA, uint8_t* rxBufferDMA) {
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

	uint8_t header = rxBufferDMA[0];
	uint8_t data[3];
	uint8_t sendPacket[4];

	data[2] = rxBufferDMA[1];
	data[1] = rxBufferDMA[2];
	data[0] = rxBufferDMA[3];

	//process data, perform conversion...


	//build packet
	sendPacket[3] = header;
	sendPacket[2] = data[2];
	sendPacket[1] = data[1];
	sendPacket[0] = data[0];
	//udp send here ?

	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
	//HAL_SPI_TransmitReceive_DMA(spiHandle, txBufferDMA, rxBufferDMA, 4);
	return 0;
}

