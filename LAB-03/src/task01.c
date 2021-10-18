//----------------------------------
// Lab 3 - Serial Communication - task01.c
//----------------------------------
// Objective:
//	Two-Terminal UART (Polling), connect UART 6 between two DISCO boards.
//  Forward any character received in USB UART (UART1) to UART6,
//  and forward any character received in UART6 to USB UART(UART1).
//

/* UART6 Ports */
// USART6_RX PC7 ARD_D0
// USART6_TX PC6 ARD_D1

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "init.h"

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void Terminal_Init(void);
char uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout);

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
UART_HandleTypeDef uart1, uart6;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void) {
	// Initialize the system
	Sys_Init();
	// Initialize UARTs
	initUart(&uart1, 115200, USART1);
	initUart(&uart6, 38400, USART6);

	uart_print(&uart1, "\033[0m\033[2J\033[;H");

	while (1) {
		// get input from uart1, if there is input, send to uart6
		char in = uart_getchar_with_timeout(&uart1, 1, 10);
		if (in) {
			uart_putchar(&uart6, &in);
			in = NULL;
		}

		in = uart_getchar_with_timeout(&uart6, 0, 10);
		if (in) {
			uart_putchar(&uart1, &in);
		}
	}
}

//------------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------------
// overwrite
char uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout) {
	char input[1] = { NULL };
	HAL_UART_Receive(huart, (uint8_t *)input, 1, timeout);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (char)input[0];
}

