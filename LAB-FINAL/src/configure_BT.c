//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//
#define BUFFER_SIZE 100
#include "init.h"

void BT_Configure(char* instruction);
void Term_Init(void);

char uart_rx[BUFFER_SIZE] = {0};
UART_HandleTypeDef uart6;


int main(void) {
	Sys_Init();
	Term_Init();
	initUart(&uart6, 9600, USART6);

	BT_Configure("AT");
	BT_Configure("AT+VERSION");
	BT_Configure("AT+ROLE=M");
}


void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void uart_read(UART_HandleTypeDef *huart, uint32_t timeout) {
	HAL_UART_Receive(huart, (uint8_t *)uart_rx, BUFFER_SIZE, timeout);
}

void print_buf(char* buf, uint32_t size) {
	for (uint32_t i = 0; i < size; i++) {
		if (buf[i] == NULL || buf[i] == 0) break;
		printf("%c", buf[i]);
		buf[i] = NULL;
	}
	fflush(stdout);
}

void BT_Configure(char* instruction) {
	HAL_Delay(1000);
	printf("Sending Instruction to HC-06 Module: \r\n\t%s\r\n", instruction);
	uart_print(&uart6, instruction);
	uart_read(&uart6, 100);
	printf("Response: \r\n\t");
	print_buf(uart_rx, BUFFER_SIZE);
	printf("Complete\r\n\n");
	fflush(stdout);
}
