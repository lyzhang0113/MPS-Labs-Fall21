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


void BT_Transmit(UART_HandleTypeDef* hbt, char c) {
	HAL_UART_Transmit(hbt, &c, 1, 100);
}

uint8_t get_parity(uint8_t val) {
	uint8_t parity = 0x00;
	while (val) {
		parity = !parity;
		val = val & (val - 1);
	}
	return parity;
}

uint8_t add_parity(uint8_t val) {
	val = (val << 1) >> 1;
	return val | get_parity(val) << 7;
}

uint8_t parity_check(uint8_t val) {
	return val >> 7 == get_parity(val);
}
