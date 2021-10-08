//----------------------------------
// Lab 3 - Part 4: SPI - task04.c
//----------------------------------
//

#define ESC 0x1B

#include "init.h"
#include "uart.h"
#include "spi.h"
#include <stdio.h>

void Terminal_Init();
void Timer_Init();


char uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout);
void read_temp();
void print_temp();
void print_version();
void reset_peri_term();
void change_peri_device_id(uint8_t id);

UART_HandleTypeDef huart1;
SPI_HandleTypeDef hspi2;

uint8_t rx_uart, rx_spi, tx;
volatile uint16_t temp;

int main(void) {
	Sys_Init();
	Timer_Init();
	Terminal_Init();
	initUart(&huart1, 115200, USART1);
	initSPI(&hspi2, SPI2);
	HAL_Delay(100);

	printf("Initializing STaTS Device");
	uint8_t status = 0;
	while (!(status & (uint8_t) 0x01)) {
		status = SPI_ReadByteFromReg(&hspi2, 0x01);
		printf(".");
		fflush(stdout);
		HAL_Delay(10);
	}
	status = SPI_ReadByteFromReg(&hspi2, 0x01);
	printf("\r\nInitialized, Device Status: %d\r\n", status);

	print_version();
	SPI_WriteByteToReg(&hspi2, 0x00, 0x02);

	while (1) {
		// Read Device Status
		uint8_t status = SPI_ReadByteFromReg(&hspi2, 0x01);
		// if new temp available, read it
		if (status & (uint8_t) 1 << 3) {
			read_temp();
		}

		rx_uart = uart_getchar_with_timeout(&huart1, 1, 10);	// Read keyboard
		if (rx_uart) {
			printf("\033[4;0HUART: %c\n", rx_uart);	// Print in UART area
			SPI_WriteByte(&hspi2, rx_uart, 100);	// W to SPI
			fflush(stdout);
		}

        if (rx_uart == ESC) { // halt on ESC or CTL+[
        	Terminal_Init();
        	printf("Please Select Mode: \r\n");
        	printf("    2. Receive char from the peripheral using SPI\r\n");
        	printf("    3. Print peripheral firmware version\r\n");
        	printf("    4. Print temperature reading\r\n");
        	printf("    5. Clear peripheral terminal\r\n");
        	printf("    6. Change device ID\r\n\n");
        	fflush(stdout);

        	switch(getchar()) {
        	case 0x32:
        		while (1) {
        			rx_spi 	= SPI_ReadByte(&hspi2, 1000);	// R/W to/from SPI
        			printf("\033[14;0H SPI: %c\n", rx_spi);	// Print in SPI area
        			if (rx_spi == ESC || uart_getchar_with_timeout(&huart1, 1, 10) == ESC) break;
        		}
        		break;
        	case 0x33:
        		print_version();
        		break;
        	case 0x34:
        		print_temp();
        		break;
        	case 0x35:
        		reset_peri_term();
        		break;
        	case 0x36:
        		printf("Please enter the device ID desired (char): ");
        		fflush(stdout);
        		change_peri_device_id(getchar());
        		break;
        	}
        }

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


void read_temp() {
	temp = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x04) << 8;
	temp |= SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x03);
	SPI_WriteByteToReg(&hspi2, 0x00, 0x02);
}

void print_temp() {
	float temp_c = 357.6 - 0.187 * temp;
	printf("Temperature Reading: %d(raw)   --->   %fC\r\n", temp, temp_c);
}

void print_version() {
	uint8_t major_version = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x07);
	uint8_t minor_version = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x08);
	printf("Firmware Version: %d.%d\r\n", major_version, minor_version);
}

void reset_peri_term() {
	SPI_WriteByteToReg(&hspi2, 0x00, (uint8_t) 1 << 3);
}

void change_peri_device_id(uint8_t id) {
	SPI_WriteByteToReg(&hspi2, 0x00, (uint8_t) 1 << 7);
	SPI_WriteByteToReg(&hspi2, 0x09, id);
	id = SPI_ReadByteFromReg(&hspi2, 0x09);
	printf("The Device ID has been set to %c\r\n", id);
}

char uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout) {
	char input[1] = { NULL };
	HAL_UART_Receive(huart, (uint8_t *)input, 1, timeout);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (char)input[0];
}

