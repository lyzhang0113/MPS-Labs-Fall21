//----------------------------------
// Lab 3 - Part 4: SPI - task04.c
//----------------------------------
//
#define TERM_WIDTH 80
#define TERM_HEIGHT 24

#define ESC 0x1B
#define MODE2_READ_TERM 0x32
#define MODE3_READ_VERSION 0x33
#define MODE4_READ_TEMP 0x34
#define MODE5_CLR_TERM 0x35
#define MODE6_CHG_DEVID 0x36

#include "init.h"
#include "uart.h"
#include "spi.h"
#include <stdio.h>

void Terminal_Init();
void erase_line(uint8_t line_num);
void print_banner(char* msg, uint8_t line_num, uint8_t beep);
void Timer_Init();


char uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout);
void read_temp();
void print_temp();
void print_version();
void reset_peri_term();
void change_peri_device_id(uint8_t id);

UART_HandleTypeDef huart1;
SPI_HandleTypeDef hspi2;

uint8_t rx_uart, rx_spi, terminate, devid;
volatile uint16_t temp;

int main(void) {
	Sys_Init();
	Timer_Init();
	Terminal_Init();
	initUart(&huart1, 115200, USART1);
	initSPI(&hspi2, SPI2);
	HAL_Delay(100);

	printf("Initializing STaTS Device");
	uint8_t status = 0;
	while (!(status & (uint8_t) 0x01)) {
		status = SPI_ReadByteFromReg(&hspi2, 0x01);
		printf(".");
		fflush(stdout);
		HAL_Delay(10);
	}
	status = SPI_ReadByteFromReg(&hspi2, 0x01);
	printf("\r\nInitialized, Device Status: %d\r\n", status);
	print_banner("<Press Enter to Continue>", 6, 0);
	getchar();

	while (1) {
		rx_uart = uart_getchar_with_timeout(&huart1, 1, 10);	// Read keyboard

		if (rx_uart) {
			Terminal_Init();
			switch (rx_uart) {
			case ESC:
	        	Terminal_Init();
	        	printf("Please Select Mode: \r\n");
	        	printf("    2. Receive char from the peripheral using SPI\r\n");
	        	printf("    3. Print peripheral firmware version\r\n");
	        	printf("    4. Trigger & Print temperature reading\r\n");
	        	printf("    5. Clear peripheral terminal\r\n");
	        	printf("    6. Change device ID\r\n\n");
	        	fflush(stdout);

	        	switch(getchar()) {
	        	case MODE2_READ_TERM:
	        		terminate = 0;
	        		Terminal_Init();
	        		print_banner("You've Entered Mode 2 - Receive From Peripheral Terminal", 2, 0);
	        		print_banner("To EXIT, Enter <ESC> in Terminal", 23, 0);
	        		while (1) {
		        		while (!(SPI_ReadByteFromReg(&hspi2, 0x01) & (uint8_t) 1 << 5)
								&& !(SPI_ReadByteFromReg(&hspi2, 0x01) & (uint8_t) 1 << 6)) {
		        			if (uart_getchar_with_timeout(&huart1, 1, 100) == ESC) {
		        				terminate = 1;
		        				Terminal_Init();
		        				break;
		        			}
		        		}
		        		if (terminate) break;
	        			rx_spi = SPI_ReadByteFromReg(&hspi2, 0x05); // Read from SPI
	        			printf("\033[14;0HCharacters Received From SPI: %c\n", rx_spi);	// Print in SPI area
	        			fflush(stdout);
	        		}
	        		break;
	        	case MODE3_READ_VERSION:
	        		Terminal_Init();
	        		print_banner("You've Entered Mode 3 - Read Peripheral Version", 2, 0);
	        		print_banner("<Press Enter to Continue>", 23, 0);
	        		printf("\033[5;0H");
	        		print_version();
	        		getchar();
	        		break;
	        	case MODE4_READ_TEMP:
	        		// Trigger Temp Reading
	        		SPI_WriteByteToReg(&hspi2, 0x00, 0x02);
	        		Terminal_Init();
	        		print_banner("You've Entered Mode 4 - Trigger & Read Peripheral Temperature", 2, 0);
	        		// Keep Reading until Ready
	        		while (!(SPI_ReadByteFromReg(&hspi2, 0x01) & (uint8_t) 1 << 3)) {
	        			HAL_Delay(50);
	        		}
					// if new temp available, read it
					temp = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x04) << 8;
					temp |= SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x03);
	        		// Print
					float temp_c = 357.6 - 0.187 * temp;
					printf("\033[5;0HTemperature Reading: %d(raw)   --->   %fC\r\n", temp, temp_c);
					print_banner("<Press Enter to Continue>", 23, 0);
					getchar();
					break;
	        	case MODE5_CLR_TERM:
	        		Terminal_Init();
	        		print_banner("You've Entered Mode 5 - Clear Peripheral Terminal", 2, 0);
	        		reset_peri_term();
	        		print_banner("Peripheral's Terminal Has Been CLEARD!", 5, 0);
					print_banner("<Press Enter to Continue>", 23, 0);
					getchar();
	        		break;
	        	case MODE6_CHG_DEVID:
	        		Terminal_Init();
	        		print_banner("You've Entered Mode 6 - Change Peripheral Device ID", 2, 0);
	        		printf("\033[4;0HCurrent device ID: %d", SPI_ReadByteFromReg(&hspi2, 0x09));
	        		printf("\033[5;0HPlease enter the device ID desired (char): ");
	        		fflush(stdout);
	        		scanf("%d", &devid);
	        		change_peri_device_id(devid);
					print_banner("<Press Enter to Continue>", 23, 0);
					getchar();
	        		break;
	        	}
	        	break;
			default:
	            // Send character to Peripheral
				printf("\033[4;0HCharacters Entered To UART: %c\n", rx_uart);	// Print in UART area
				fflush(stdout);
				SPI_WriteByteToReg(&hspi2, 0x05, rx_uart); // W to SPI
				break;
			}
		}

		if (rx_spi) {
            // Send character to Peripheral
			printf("\033[13;0HCharacters Received From SPI: %c\n", rx_spi);	// Print in UART area
			fflush(stdout);
		}

	}

}

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void erase_line(uint8_t line_num) {
	printf("\033[%d;H\033[K", line_num); // removes invalid key warning
	fflush(stdout);
}

void print_banner(char* msg, uint8_t line_num, uint8_t beep) {
	erase_line(line_num);
	int padlen = (TERM_WIDTH - strlen(msg)) / 2;
	if (beep) printf("\a");
	printf("\033[%d;H%*s%s%*s", line_num, padlen, "", msg, padlen, "");
	fflush(stdout);
}

void Timer_Init() {
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t) 108; // 108Mhz / 108 = 1MHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 1; // 1MHz / 1 = 1MHz -> every 1us
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	__HAL_RCC_TIM7_CLK_ENABLE();
//	HAL_NVIC_SetPriority(TIM7_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
}


void read_temp() {
	temp = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x04) << 8;
	temp |= SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x03);
	SPI_WriteByteToReg(&hspi2, 0x00, 0x02);
}

void print_temp() {
	float temp_c = 357.6 - 0.187 * temp;
	printf("Temperature Reading: %d(raw)   --->   %fC\r\n", temp, temp_c);
}

void print_version() {
	uint8_t major_version = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x07);
	uint8_t minor_version = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x08);
	printf("Firmware Version: %d.%d\r\n", major_version, minor_version);
}

void reset_peri_term() {
	SPI_WriteByteToReg(&hspi2, 0x00, (uint8_t) 1 << 3);
}

void change_peri_device_id(uint8_t id) {
	SPI_WriteByteToReg(&hspi2, 0x00, (uint8_t) 1 << 7);
	SPI_WriteByteToReg(&hspi2, 0x09, id);
	id = SPI_ReadByteFromReg(&hspi2, 0x09);
	printf("\r\nThe Device ID has been set to %d\r\n", id);
}

char uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout) {
	char input[1] = { NULL };
	HAL_UART_Receive(huart, (uint8_t *)input, 1, timeout);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (char)input[0];
}

