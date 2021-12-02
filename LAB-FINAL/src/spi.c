//----------------------------------
// Lab 3 - Serial Communication - spi.c
//----------------------------------
//
// Helper functions for SPI


#include "spi.h"

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
TIM_HandleTypeDef htim7;
volatile uint32_t curr_time_in_micro = 0;

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


void initSPI(SPI_HandleTypeDef* hspi, SPI_TypeDef* Tgt) {
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

//------------------------------------------------------------------------------------
// Timer 7 Functions (for the 10us delay between transmitting/receiving)
//------------------------------------------------------------------------------------
/*

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	__HAL_RCC_TIM7_CLK_ENABLE();
//	HAL_NVIC_SetPriority(TIM7_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void Timer_Init( void ) {
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t) 108; // 108Mhz / 108 = 1MHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 1; // 1MHz / 1 = 1MHz -> every 1us
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void TIM7_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM7) {
		curr_time_in_micro++;
	}
}
*/

void wait_us(uint32_t us) {
	curr_time_in_micro = 0;
	while (curr_time_in_micro < us) ;
}
//------------------------------------------------------------------------------------
// SPI Read/Write
//------------------------------------------------------------------------------------

// read and write 1 char at the same time since full-duplex
uint8_t SPI_ReadWriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	uint8_t RxData;
	HAL_SPI_TransmitReceive(hspi, &TxData, &RxData, 1, 1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	return RxData;
}

uint8_t SPI_ReadByte(SPI_HandleTypeDef* hspi, uint32_t timeout) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	uint8_t RxData;
	HAL_SPI_Receive(hspi, &RxData, 1, timeout);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	return RxData;
}

void SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t TxData, uint32_t timeout) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, &TxData, 1, timeout);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
}

uint8_t SPI_ReadByteFromReg(SPI_HandleTypeDef* hspi, uint8_t reg) {
	uint8_t RxData;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	wait_us(5);
	HAL_SPI_Transmit(hspi, &reg, 1, 1000);
	wait_us(10);
	HAL_SPI_Receive(hspi, &RxData, 1, 1000);
	wait_us(5);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	wait_us(5);
	return RxData;
}

void SPI_WriteByteToReg(SPI_HandleTypeDef* hspi, uint8_t reg, uint8_t data) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	wait_us(5);
	HAL_SPI_Transmit(hspi, &reg, 1, 1000);
	wait_us(10);
	HAL_SPI_Transmit(hspi, &data, 1, 1000);
	wait_us(5);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	wait_us(5);
}
