#ifndef SPI_H_
#define SPI_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void initSPI(SPI_HandleTypeDef* hspi);

uint8_t SPI_ReadWriteByte(uint8_t TxData);

uint8_t SPI_ReadByte(SPI_HandleTypeDef* hspi, uint32_t timeout);

void SPI_ReadBytes(SPI_HandleTypeDef* hspi, uint8_t* RxDataBuf, uint32_t length);

void SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData, uint32_t timeout);

void SPI_WriteBytes(SPI_HandlTypeDef* hspi, uint8_t* TxDataBuf, uint32_t length);

#endif /* SPI_H_ */
