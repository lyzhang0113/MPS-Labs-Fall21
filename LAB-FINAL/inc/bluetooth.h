/*
 * bluetooth.h
 *
 *  Created on: Dec 2, 2021
 *      Author: zhangl33
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#define SAMPLING_FREQ 50

#include "uart.h"

void BT_Init(UART_HandleTypeDef* hbt);

void BT_Transmit(UART_HandleTypeDef* hbt, char c);

uint8_t add_parity(uint8_t val);

uint8_t parity_check(uint8_t val);

#endif /* INC_BLUETOOTH_H_ */
