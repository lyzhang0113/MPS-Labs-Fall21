/*
 * bluetooth.h
 *
 *  Created on: Dec 2, 2021
 *      Author: zhangl33
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#include "uart.h"

void BT_Init(UART_HandleTypeDef* hbt);

void BT_Connect(UART_HandleTypeDef* hbt); // Only Master Triggers this

void BT_Transmit(UART_HandleTypeDef* hbt, char c);

#endif /* INC_BLUETOOTH_H_ */
