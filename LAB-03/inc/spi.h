#ifndef SPI_H_
#define SPI_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void initSPI(SPI_HandleTypeDef* hspi, SPI_TypeDef* Tgt);

uint8_t SPI_ReadWriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData);

uint8_t SPI_ReadByte(SPI_HandleTypeDef* hspi, uint32_t timeout);

void SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData, uint32_t timeout);



#endif /* SPI_H_ */
