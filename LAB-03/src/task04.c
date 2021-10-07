//----------------------------------
// Lab 3 - Part 4: SPI - task04.c
//----------------------------------
//

#include "init.h"
#include "spi.h"
#include <stdio.h>

uint16_t read_temp();
void Terminal_Init();
void Timer_Init();
uint8_t SPI_ReadByteFromReg(SPI_HandleTypeDef* hspi, uint8_t reg);
void SPI_WriteByteToReg(SPI_HandleTypeDef* hspi, uint8_t reg, uint8_t data);

volatile uint32_t curr_time_in_micro = 0;
TIM_HandleTypeDef htim7;
SPI_HandleTypeDef hspi2;

int main(void) {
	Sys_Init();
	Timer_Init();
	Terminal_Init();

	initSPI(&hspi2, SPI2);
	HAL_Delay(100);

//	printf("Initializing STaTS Device");
	uint8_t status = 0;
	while (!(status & (uint8_t) 0x01)) {
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
//		HAL_SPI_Receive(&hspi2, &status, 1, 50);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		status = SPI_ReadByteFromReg(&hspi2, 0x01);
		printf(".");
		fflush(stdout);
		HAL_Delay(100);
	}
	status = SPI_ReadByteFromReg(&hspi2, 0x01);
	printf("\r\nInitialized, Device Status: %d\r\n", status);
//	printf("Resetting Nucleo Board...\r\n");
//	SPI_WriteByteToReg(&hspi2, 0x00, 0x1F);
//	printf("Done\r\n");

//	while (1) {
//		uint8_t v_maj = SPI_ReadByteFromReg(&hspi2, 0x07);
//		uint8_t v_min = SPI_ReadByteFromReg(&hspi2, 0x08);
//
//		printf("version = %d.%d\r\n", v_maj, v_min);
//		HAL_Delay(200);
//
//	}

	while (1) {
		uint16_t temp = read_temp();
		printf("Temp: %d\r\n", temp);
		HAL_Delay(1000);
	}


	uint8_t major_version = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x07);
	uint8_t minor_version = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x08);
	printf("Version: %d.%d\r\n", major_version, minor_version);


	status = SPI_ReadByteFromReg(&hspi2, 0x01);
	printf("\r\nDevice Status: %d\r\n", status);

	uint16_t temp = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x04) << 8;
	temp |= SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x03);
	double temp_c = 357.6 - 0.187 * temp;
	printf("Temp: %dC\r\n", temp);
}

void TIM7_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM7) {
		curr_time_in_micro++;
	}
}

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Timer_Init() {
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t) 108; // 108Mhz / 1080 = 100kHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 1; // 100kHz / 100 = 1kHz -> every 1ms
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	__HAL_RCC_TIM7_CLK_ENABLE();
//	HAL_NVIC_SetPriority(TIM7_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
}


uint16_t read_temp() {
	SPI_WriteByteToReg(&hspi2, 0x00, 0x02);
	uint16_t temp_raw = 0;
	uint8_t status = SPI_ReadByteFromReg(&hspi2, 0x01);
	if (status & ((uint8_t) 0x02)) {
		printf("A Fault has occurred during temperature measurement!\r\n");
	} else if (status & ((uint8_t) 0x08)) {
		temp_raw = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x04) << 8;
		temp_raw |= SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x03);
	} else if (status & ((uint8_t) 0x04)) {
		printf("The device is busy.\r\n");
		HAL_Delay(10);
		return read_temp();
	}
	printf("status = %d\r\n", status);
	return temp_raw;
}

void wait_us(uint32_t us) {
	curr_time_in_micro = 0;
	while (curr_time_in_micro < us) ;
}

uint8_t SPI_ReadByteFromReg(SPI_HandleTypeDef* hspi, uint8_t reg) {
	uint8_t RxData;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	wait_us(10);
	HAL_SPI_Transmit(hspi, &reg, 1, 1000);
	wait_us(20);
	HAL_SPI_Receive(hspi, &RxData, 1, 1000);
	wait_us(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	wait_us(10);
	return RxData;
}

void SPI_WriteByteToReg(SPI_HandleTypeDef* hspi, uint8_t reg, uint8_t data) {
	uint8_t RxData;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	wait_us(10);
	HAL_SPI_Transmit(hspi, &reg, 1, 1000);
	wait_us(20);
	HAL_SPI_Transmit(hspi, &data, 1, 1000);
	wait_us(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	wait_us(10);
}
