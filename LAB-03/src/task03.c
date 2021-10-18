//----------------------------------
// Lab 3 - Serial Communication - task03.c
//----------------------------------
// Objective: Transmit to and Receive from SPI in loopback configuration.
//

//------------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------------
#define TERM_HEIGHT 24

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
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
		rx_uart = uart_getchar_with_timeout(&huart1, 1, 10);	// Read keyboard
		if (!rx_uart) continue;		// Only alter terminal on input
		printf("\033[4;20H %c\n", rx_uart);	// Print in UART area

		rx_spi 	= SPI_ReadWriteByte(&hspi2, rx_uart);	// R/W to/from SPI
		printf("\033[14;20H%c\n", rx_spi);	// Print in SPI area

		printf("\033[u %c\033[s", rx_uart);	// Print char to history bank
		fflush(stdout);

		HAL_Delay(1);
	}
}

//------------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------------
void TerminalInit( void )
{
	// Clear and set attributes
	printf("\033[0m\033[2J\033[;H");
	fflush(stdout);

	// Set up UART half of terminal
	printf("\033[4;0H\033[2KReceived from UART: ");
	fflush(stdout);

	// Set up SPI half """
	printf("\033[14;0H\033[2KReceived from SPI: ");
	fflush(stdout);

	// Set up scrolling area
	printf("\033[2;3r");
	fflush(stdout);

	// Set up "history" area
	printf("\033[HCharacter History:\r\n\033[s");
	fflush(stdout);
}

uint8_t uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout) {
	char input[1] = { '\0' };
	HAL_UART_Receive(huart, (uint8_t *)input, 1, timeout);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (uint8_t)input[0];
}
