/*
 * bluetooth.c
 *
 *  Created on: Dec 2, 2021
 *      Author: zhangl33
 */
#include "bluetooth.h"

void BT_Init(UART_HandleTypeDef* hbt) {
	initUart(hbt, 9600, USART6);

	HAL_NVIC_SetPriority(USART6_IRQn, 15, 1);
	HAL_NVIC_EnableIRQ(USART6_IRQn);
	__HAL_UART_ENABLE_IT(hbt, UART_IT_RXNE);

}

char BT_TransmitReceive(UART_HandleTypeDef* hbt, char c) {
	char rx[1] = {NULL};
	HAL_UART_Transmit(hbt, &c, 1, 10);
	HAL_UART_Receive(hbt, (uint8_t *)rx, 1, 10);
	return (char) rx[0];
}

void BT_Connect(UART_HandleTypeDef* hbt) {
	printf("Initializing Connection to Car ");
	while (1) {
		HAL_Delay(500);
		printf(".");
		fflush(stdout);
		if (BT_TransmitReceive(hbt, 'p') == 'p') break;
	}
	printf("  Connected\r\n");
	fflush(stdout);
}

void BT_Transmit(UART_HandleTypeDef* hbt, char c) {
	HAL_UART_Transmit(hbt, &c, 1, 100);
}
