//----------------------------------
// Lab 3 - task03.c
//----------------------------------
// Objective:
//
//


#include "init.h"
#include "spi.h"
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void TerminalInit( void );
uint8_t uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout);

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
SPI_HandleTypeDef hspi2;
UART_HandleTypeDef huart1;
uint8_t rx_uart = 0, rx_spi, tx;

//------------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------------
void TerminalInit( void )
{
	printf("\033[0m\033[2J\033[;H");
	fflush(stdout);
}

uint8_t uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout) {
	char input[1] = { '\0' };
	HAL_UART_Receive(huart, (uint8_t *)input, 1, timeout);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (uint8_t)input[0];
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void) {
	// Initialize the system
	Sys_Init();
	TerminalInit();
	initSPI(&hspi2, SPI2);
	initUart(&huart1, 115200, USART1);

	while (1)
	{
		printf("\033[H\033[2K");
		fflush(stdout);

		rx_uart = uart_getchar_with_timeout(&huart1, 1, 10);
		if (!rx_uart) continue;
		printf("\033[2;0H\033[2KReceived from UART: %c", rx_uart);
		fflush(stdout);

		rx_spi 	= SPI_ReadWriteByte(&hspi2, rx_uart);
		printf("\033[3;0H\033[2KReceived from SPI: %c", rx_spi);
		fflush(stdout);

		HAL_Delay(10);
	}
}


