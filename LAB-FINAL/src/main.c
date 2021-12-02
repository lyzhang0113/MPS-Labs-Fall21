/*---------- DEFINES ---------------------------------------------------------------*/
#define BUFFER_SIZE 100

/*---------- INCLUDES --------------------------------------------------------------*/
#include <stdio.h>

#include "init.h"

/*---------- FUNCTION PROTOTYPES ---------------------------------------------------*/
void Term_Init		( void );		// Clear and reset Terminal
void Timer_Init		( void );		// Sets up Timers
void GPIO_Init		( void );		// Configure and enable GPIO
void Interrupt_Init	( void );		// Sets up Interrupts

void BT_Configure	( char* instruction );

void uart_read(UART_HandleTypeDef *huart, uint32_t timeout);

void print_buf	( char *buf, uint32_t size );	// Prints contents of buffer

/*---------- HANDLER TYPEDEFS ------------------------------------------------------*/
UART_HandleTypeDef 	huart6;
SPI_HandleTypeDef 	hspi1;

/*---------- GLOBAL VARIABLE -------------------------------------------------------*/
char uart_rx[BUFFER_SIZE] = {0};

/*---------- MAIN PROGRAM ----------------------------------------------------------*/
int main(void){
	Sys_Init();
	Term_Init();
	Timer_Init();
	GPIO_Init();
	Interrupt_Init();

	initUart(&huart6, 9600, USART6);

	// Read the README in the base directory of this project.
	while (1)
	{
		uart_read(&huart6, 100);
		if (uart_rx[0] == 'p') {
			HAL_UART_Transmit(&huart6, uart_rx, 1, 100);
		}
	}
}

/*--------- INITIALIZATION FUNCTIONS ----------------------------------------------*/
void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Timer_Init( void )
{

}

void GPIO_Init( void )
{

}

void Interrupt_Init( void )
{

}

/*---------- BLUETOOTH FUNCTIONS ---------------------------------------------------*/
/*
void BT_Configure(char* instruction)
{
	HAL_Delay(1000);
	printf("Sending Instruction to HC-06 Module: \r\n\t%s\r\n", instruction);
	uart_print(&huart6, instruction);
	uart_read(&huart6, 100);
	printf("Response: \r\n\t");
	print_buf(uart_rx, BUFFER_SIZE);
	printf("Complete\r\n\n");
	fflush(stdout);
} */

/*---------- UART FUNCTIONS --------------------------------------------------------*/
void uart_read(UART_HandleTypeDef *huart, uint32_t timeout)
{
	HAL_UART_Receive(huart, (uint8_t *)uart_rx, BUFFER_SIZE, timeout);
}

/*---------- HELPER FUNCTIONS ------------------------------------------------------*/
void print_buf(char* buf, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++) {
		if (buf[i] == NULL || buf[i] == 0) break;
		printf("%c", buf[i]);
		buf[i] = NULL;
	}
	fflush(stdout);
}
