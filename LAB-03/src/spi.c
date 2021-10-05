#include "spi.h"
/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
 // Do NOT change the name of an MspInit function; it needs to override a
 // __weak function of the same name. It does not need a prototype in the header.
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
		GPIO_InitStruct.Alternate = GPIO_AF7_SPI2;

		// Initialize SPI2_SCK (PA12)
		GPIO_InitStruct.Pin       = GPIO_PIN_12;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// Initialize SPI2_MISO (PB14) and SPI2_MOSI (PB15)
		GPIO_InitStruct.Pin       = GPIO_PIN_14 | GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void initSPI(SPI_HandleTypeDef* hspi, SPI_TypeDef* Tgt) {
	hspi->Instance    = Tgt;
	hspi->Init.Mode   = SPI_MODE_MASTER;
	hspi->Init.Direction = SPI_DIRECTION_2LINES;
	hspi->Init.TIMode = SPI_TIMODE_DISABLE;

	HAL_SPI_Init(&hspi);
}
