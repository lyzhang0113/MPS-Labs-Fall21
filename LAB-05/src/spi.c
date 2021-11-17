//----------------------------------
// Lab 5 -  - spi.c
//----------------------------------
//
// Helper functions for SPI

// SPI2_MISO: 	GPIOB_14 (Arduino D12)
// SPI2_MOSI: 	GPIOB_15 (Arduino D11)
// SPI2_SCK:	GPIOA_11 (Arduino D13)

#include "spi.h"
#include <stdio.h>

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	// SPI GPIO initialization structure here
	GPIO_InitTypeDef  GPIO_InitStruct;

	if (hspi->Instance == SPI2)
	{
		// Enable clocks for related GPIO ports and SPI2
		__GPIOA_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
		__SPI2_CLK_ENABLE();

		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

		// Initialize SPI2_SCK (PA12)
		GPIO_InitStruct.Pin       = GPIO_PIN_12;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// Initialize SPI2_MISO (PB14) and SPI2_MOSI (PB15)
		GPIO_InitStruct.Pin       = GPIO_PIN_14 | GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		// Software NSS (PA11)
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
		GPIO_InitStruct.Pin       = GPIO_PIN_11;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	}
}


void SPI_Init(SPI_HandleTypeDef* hspi, SPI_TypeDef* Tgt) {
	hspi->Instance               = Tgt;
	hspi->Init.Mode              = SPI_MODE_MASTER;
	hspi->Init.Direction         = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize          = SPI_DATASIZE_8BIT;
	hspi->Init.CLKPolarity       = SPI_POLARITY_LOW;
	hspi->Init.CLKPhase          = SPI_PHASE_2EDGE;
	hspi->Init.NSS               = SPI_NSS_SOFT; // Master: Software Controller NSS (GPIO A11)
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; // 108MHz / 128 = 843kHz
	hspi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode            = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	hspi->Init.CRCPolynomial     = 7;
	hspi->Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
	hspi->Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;

	HAL_SPI_Init(hspi);

}

