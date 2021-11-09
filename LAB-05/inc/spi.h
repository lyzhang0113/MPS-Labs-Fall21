/*
 * spi.h
 *
 *  Created on: Nov 1, 2021
 *      Author: zhangl33
 */

#ifndef SPI_H_
#define SPI_H_


//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"


//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------

void initSPI(SPI_HandleTypeDef* hspi, SPI_TypeDef* Tgt);
//
//void SPI_ReadWriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData, uint16_t size);
//
//void SPI_ReadByte(SPI_HandleTypeDef* hspi, uint16_t size);
//
//void SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData);

#endif /* SPI_H_ */
