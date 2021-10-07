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
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

		// Initialize SPI2_SCK (PA12)
		GPIO_InitStruct.Pin       = GPIO_PIN_12;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// Initialize SPI2_MISO (PB14) and SPI2_MOSI (PB15)
		GPIO_InitStruct.Pin       = GPIO_PIN_14 | GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

// SPI Initialization
void initSPI(SPI_HandleTypeDef* hspi, SPI_TypeDef* Tgt) {
	hspi->Instance               = Tgt;
	hspi->Init.Mode              = SPI_MODE_MASTER;
	hspi->Init.Direction         = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize          = SPI_DATASIZE_8BIT;
	hspi->Init.CLKPolarity       = SPI_POLARITY_HIGH;
	hspi->Init.CLKPhase          = SPI_PHASE_2EDGE;
	hspi->Init.NSS               = SPI_NSS_HARD_OUTPUT; // Master: Hardware Controller NSS
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; // 108MHz / 128 = 843kHz
	hspi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode            = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	hspi->Init.CRCPolynomial     = 7;
	hspi->Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
	hspi->Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;

	HAL_SPI_Init(hspi);

	__HAL_SPI_ENABLE(hspi);
}

// read and write 1 char at the same time since full-duplex
uint8_t SPI_ReadWriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData) {
	uint8_t RxData;
	HAL_SPI_TransmitReceive(hspi, &TxData, &RxData, 1, 1000);
	return RxData;
}

uint8_t SPI_ReadByte(SPI_HandleTypeDef* hspi, uint32_t timeout) {
	uint8_t RxData;
	HAL_SPI_Receive(hspi, &RxData, 1, timeout);
	return RxData;
}

void SPI_ReadBytes(SPI_HandleTypeDef* hspi, uint8_t* RxDataBuf, uint32_t length) {
	HAL_SPI_Receive(hspi, RxDataBuf, length, 1000);
}

void SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData, uint32_t timeout) {
	HAL_SPI_Transmit(hspi, &TxData, 1, timeout);
}

void SPI_WriteBytes(SPI_HandleTypeDef* hspi, uint8_t* TxDataBuf, uint32_t length) {
	HAL_SPI_Transmit(hspi, TxDataBuf, length, 1000);
}
