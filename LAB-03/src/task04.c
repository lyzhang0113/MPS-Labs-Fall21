//----------------------------------
// Lab 3 - Part 4: SPI - task04.c
//----------------------------------
//

#include "init.h"
#include "spi.h"
#include <stdio.h>

void Terminal_Init();

SPI_HandleTypeDef hspi2;
uint8_t rxbuf, txbuf = 0x01;
uint8_t reading = 0;

int main(void) {
	Sys_Init();
	Terminal_Init();

	initSPI(&hspi2, SPI2);

	printf("Initializing STaTS Device\r\n");
	uint8_t reading = SPI_ReadByteFromReg(&hspi2, 0x01);
//	reading = 0;
//
	while (!(reading & (uint8_t) 0x01)) {
		printf("\r%d", reading);
		fflush(stdout);
		HAL_Delay(200);
		reading = SPI_ReadByteFromReg(&hspi2, (uint8_t) 0x01);
	}
	printf("\nInitialized, Device Status: %d\r\n", reading);

//	while (1) {
//		SPI_ReadWriteByte(&hspi2, 0xff);
//		HAL_Delay(10);
//	}


	uint8_t major_version = SPI_ReadWriteByte(&hspi2, 0x07);
	uint8_t minor_version = SPI_ReadWriteByte(&hspi2, 0x08);
	printf("Version: %s, %s", &major_version, &minor_version);
}

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}
