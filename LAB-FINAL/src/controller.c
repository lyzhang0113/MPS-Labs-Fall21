//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//
#define BUFFER_SIZE 100
#include "init.h"
#include "bluetooth.h"

char uart_getchar_it(UART_HandleTypeDef *huart, uint8_t echo);
void BT_Connect();
char BT_TransmitReceive(char c);
void Term_Init(void);

char uart_rx[BUFFER_SIZE] = {0};
UART_HandleTypeDef bt;


int main(void) {
	Sys_Init();
	Term_Init();
	BT_Init(&bt);

	BT_Connect(&bt);


}

void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&bt); }

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	if (huart->Instance == USART6)
	{
		char in = uart_getchar_it(&bt, 0);
	}
}

void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}


