//----------------------------------
// Lab 3 - Part 4: SPI - task04.c
//----------------------------------
//

#include "init.h"
#include "uart.h"
#include "spi.h"
#include <stdio.h>

uint16_t read_temp();
void Terminal_Init();
void Timer_Init();
uint8_t SPI_ReadByteFromReg(SPI_HandleTypeDef* hspi, uint8_t reg);
void SPI_WriteByteToReg(SPI_HandleTypeDef* hspi, uint8_t reg, uint8_t data);


SPI_HandleTypeDef hspi2;
UART_HandleTypeDef huart1;
uint8_t rx_uart, rx_spi, tx;

int main(void) {
	Sys_Init();
	Timer_Init();
	Terminal_Init();

	initSPI(&hspi2, SPI2);
	HAL_Delay(100);

	initUart(&huart1, 115200, USART1);

	while (1) {
		rx_uart = uart_getchar_with_timeout(&huart1, 1, 10);	// Read keyboard
		if (!rx_uart) continue;		// Only alter terminal on input
		printf("\033[4;0HUART: %c\n", rx_uart);	// Print in UART area

		rx_spi 	= SPI_ReadWriteByte(&hspi2, rx_uart);	// R/W to/from SPI
		printf("\033[14;0H SPI: %c\n", rx_spi);	// Print in SPI area
	}
}

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Timer_Init() {
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t) 108; // 108Mhz / 108 = 1MHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 1; // 1MHz / 1 = 1MHz -> every 1us
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



