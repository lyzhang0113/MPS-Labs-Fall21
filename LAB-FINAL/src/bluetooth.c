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

	uart_getchar_it(hbt, 0);
}

char BT_TransmitReceive(UART_HandleTypeDef* hbt, char c) {
	char rx[1] = {NULL};
	HAL_UART_Transmit(hbt, &c, 1, 100);
	HAL_UART_Receive(hbt, (uint8_t *)rx, 1, 100);
	return (char) rx[0];
}

void BT_Connect(UART_HandleTypeDef* hbt) {
	while (1) {
		HAL_Delay(1000);
		if (BT_TransmitReceive(hbt, 'p') == 'p') break;
	}
	printf("Connected\r\n");
}

