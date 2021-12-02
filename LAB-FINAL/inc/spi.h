//----------------------------------
// Lab 3 - Serial Communication - spi.h
//----------------------------------
//
// Helper functions for SPI

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

void Timer_Init(void);

uint8_t SPI_ReadWriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData);

uint8_t SPI_ReadByte(SPI_HandleTypeDef* hspi, uint32_t timeout);

void SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData, uint32_t timeout);

uint8_t SPI_ReadByteFromReg(SPI_HandleTypeDef* hspi, uint8_t reg);

void SPI_WriteByteToReg(SPI_HandleTypeDef* hspi, uint8_t reg, uint8_t data);

#endif /* SPI_H_ */
