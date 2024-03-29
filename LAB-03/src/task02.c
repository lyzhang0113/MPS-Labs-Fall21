//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//

/*
 * - The board requires the grounds to be connected
 *
 */

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "init.h"

//------------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------------
void Terminal_Init( void );
void Interrupt_Init( void );
void UART_Init( void );
char uart_getchar_it(UART_HandleTypeDef *huart, uint8_t echo);

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
UART_HandleTypeDef huart1, huart6;
char in, input[1];


//------------------------------------------------------------------------------------
// MAIN FUNCTION
//------------------------------------------------------------------------------------
int main(void)
{
	//Initialize the system
	Sys_Init();
	UART_Init();
	Interrupt_Init();

	uart_getchar_it(&huart1, 0);
	uart_getchar_it(&huart6, 0);

	Terminal_Init();

	while (1);
}

//------------------------------------------------------------------------------------
// Initializations
//------------------------------------------------------------------------------------
void Terminal_Init( void )
{
    uart_print(&huart1, "\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    uart_print(&huart6, "\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
}

void Interrupt_Init( void )
{
	HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
	HAL_NVIC_SetPriority(USART6_IRQn, 15, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_EnableIRQ(USART6_IRQn);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
}

void UART_Init( void )
{
	initUart(&huart1, (uint32_t) 115200, USART1);
	initUart(&huart6, (uint32_t) 38400, USART6);
}

//------------------------------------------------------------------------------------
// Interrupt Handlers
//------------------------------------------------------------------------------------
void USART1_IRQHandler( void ) { HAL_UART_IRQHandler(&huart1); }
void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&huart6); }
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	/* Receive in UART1, Transmit to UART6 */
	if (huart->Instance == USART1)
	{
		in = uart_getchar_it(&huart1, 1);
		uart_putchar(&huart6, &in);
	}
	/* Receive in UART6, Transmit to UART1 */
	else if (huart->Instance == USART6)
	{
		in = uart_getchar_it(&huart6, 0);
		uart_putchar(&huart1, &in);
	}
}

//------------------------------------------------------------------------------------
// Additional functions
//------------------------------------------------------------------------------------
// Get one character
// 'echo' means enable (1) or disable (0) echoing of characters
char uart_getchar_it(UART_HandleTypeDef *huart, uint8_t echo) {
	HAL_UART_Receive_IT(huart, (uint8_t *)input, 1);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (char)input[0];
}
