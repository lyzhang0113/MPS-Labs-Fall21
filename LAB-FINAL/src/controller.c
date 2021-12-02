//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//
#define BUFFER_SIZE 10
#include "init.h"

void Term_Init(void);
void uart_read(UART_HandleTypeDef *huart, uint32_t timeout);
void print_buf(char* buf, uint32_t size);

char uart_rx[BUFFER_SIZE] = {0};
UART_HandleTypeDef uart6;


int main(void) {
	Sys_Init();
	Term_Init();
	initUart(&uart6, 9600, USART6);
	HAL_Delay(1000);

	uart_print(&uart6, "AT+VERSION");
	uart_read(&uart6, 100);
	printf("Bluetooth Module Status: \r\n");
	print_buf(uart_rx, BUFFER_SIZE);
	printf("Complete");

	HAL_Delay(1000);

	uart_print(&uart6, "AT+ROLE=M");
	uart_read(&uart6, 100);
	printf("Bluetooth Module Set To Master: \r\n");
	print_buf(uart_rx, BUFFER_SIZE);

	printf("Complete");
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
