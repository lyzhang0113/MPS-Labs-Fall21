//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

#include <stdio.h>
#include "init.h"
#include "bluetooth.h"

char uart_getchar_it(UART_HandleTypeDef *huart, uint8_t echo);
void BT_Connect();
char BT_TransmitReceive(char c);
void Term_Init(void);

UART_HandleTypeDef bt;


int main(void) {
	Sys_Init();
	Term_Init();
	BT_Init(&bt);

	BT_Connect(&bt);

	uart_getchar_it(&bt, 0);

	while (1) {
		char input = uart_getchar(&USB_UART, 1);
		BT_Transmit(&bt, input);
	}


}

void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&bt); }

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	if (huart->Instance == USART6)
	{
		char in = uart_getchar_it(&bt, 0);
		switch (in) {
		default:
			break;
		}
	}
}

void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}


