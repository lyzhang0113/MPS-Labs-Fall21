//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//
#define BUFFER_SIZE 100
#include "init.h"

void BT_Connect();
char BT_TransmitReceive(char c);
void Term_Init(void);

char uart_rx[BUFFER_SIZE] = {0};
UART_HandleTypeDef uart6;


int main(void) {
	Sys_Init();
	Term_Init();
	initUart(&uart6, 9600, USART6);
	BT_Connect();
}


void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

char BT_TransmitReceive(char c) {
	char rx[1] = {NULL};
	HAL_UART_Transmit(&uart6, &c, 1, 100);
	HAL_UART_Receive(&uart6, (uint8_t *)rx, 1, 100);
	return (char) rx[0];
}

void BT_Connect() {
	while (1) {
		HAL_Delay(1000);
		if (BT_TransmitReceive('p') == 'p') break;
	}
	printf("Connected\r\n");
}
