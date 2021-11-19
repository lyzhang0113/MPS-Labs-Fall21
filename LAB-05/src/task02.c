//--------------------------------
// Lab 5 - Direct Memory Access - task02.c
//--------------------------------
//	SPI DMA: Modify Lab 3 Task 3's code to have DMA manage the transmission and
//		reception of the loopback data. The program should read a full line of
//		characters from the terminal into a buffer, stopping when the user presses
//		enter (\n), instead of just one character.
//

//------------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------------
#define TERM_HEIGHT 24
#define NEW_LINE 0x0D
#define BUFFER_SIZE 100
//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "init.h"
#include "spi.h"
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void Term_Init( void );
void DMA_Init(void);
uint8_t uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout);

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdmatx2, hdmarx2;

uint8_t RxBuf[BUFFER_SIZE] = {0};
uint8_t TxBuf[BUFFER_SIZE] = {0};
uint16_t num_char = 0;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void) {
	// Initialize the system
	Sys_Init();
	Term_Init();
	SPI_Init(&hspi2, SPI2);
	DMA_Init();

	while (1)
	{
		for (uint8_t i = 0; i < BUFFER_SIZE;) {
			TxBuf[i] = uart_getchar_with_timeout(&USB_UART, 1, 10);	// Read keyboard
			if (!TxBuf[i]) continue;		// Do nothing if no input
			printf("\033[u %c\033[s", TxBuf[i]);	// Print char to history bank
			fflush(stdout);

			if (TxBuf[i] == NEW_LINE) {
				// newline \n Received, Start Transmission
				num_char = i;
				HAL_SPI_TransmitReceive_DMA(&hspi2, TxBuf, RxBuf, num_char);
				break;
			}
			i++;
		}
	}
}

//------------------------------------------------------------------------------------
// IRQHandler & Callbacks
//------------------------------------------------------------------------------------
// DMA
void DMA1_Stream1_IRQHandler() { // RX
	HAL_DMA_IRQHandler(&hdmarx2);
}

void DMA1_Stream6_IRQHandler() { // TX
	HAL_DMA_IRQHandler(&hdmatx2);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	printf("\033[14;20H (%d)     \r\n\033[2K", num_char);	// Print in SPI area
	for (uint8_t i = 0; i < num_char; i++) {
		printf("%c", RxBuf[i]);
	}
//	fflush(stdout);
	printf("\033[4;21H (%d)     \r\n\033[2K", num_char);	// Print in UART area
	for (uint8_t i = 0; i < num_char; i++) {
		printf("%c", TxBuf[i]);
	}
	fflush(stdout);
}

//------------------------------------------------------------------------------------
// Initializations
//------------------------------------------------------------------------------------
void Term_Init( void )
{
	// Clear and set attributes
	printf("\033[0m\033[2J\033[;H");
	fflush(stdout);

	// Set up UART half of terminal
	printf("\033[4;0H\033[2KTransmitted to COPI: ");
	fflush(stdout);

	// Set up SPI half """
	printf("\033[14;0H\033[2KReceived from CIPO: ");
	fflush(stdout);

	// Set up scrolling area
	printf("\033[2;3r");
	fflush(stdout);

	// Set up "history" area
	printf("\033[HCharacter History:\r\n\033[s");
	fflush(stdout);
}

void DMA_Init() {

	// DMA
	// DMA1 Channel 9:
	// 		Stream 1: SPI2_RX
	// 		Stream 6: SPI2_TX
	__DMA1_CLK_ENABLE();

	hdmatx2.Instance = DMA1_Stream6;
	hdmatx2.Init.Channel = DMA_CHANNEL_9;
	hdmatx2.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdmatx2.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // Using DIRECT Mode
	hdmatx2.Init.Mode = DMA_NORMAL;
	hdmatx2.Init.MemInc = DMA_MINC_ENABLE;
	HAL_DMA_Init(&hdmatx2);

	hdmarx2.Instance = DMA1_Stream1;
	hdmarx2.Init.Channel = DMA_CHANNEL_9;
	hdmarx2.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdmarx2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdmarx2.Init.Mode = DMA_NORMAL;
	hdmarx2.Init.MemInc = DMA_MINC_ENABLE;
	HAL_DMA_Init(&hdmarx2);

	__HAL_LINKDMA(&hspi2, hdmatx, hdmatx2); // Link DMA to SPI TX
	__HAL_LINKDMA(&hspi2, hdmarx, hdmarx2); // Link DMA to SPI RX
	HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
	HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn); // Enable TX IT
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn); // Enable RX IT
}

//------------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------------

uint8_t uart_getchar_with_timeout(UART_HandleTypeDef *huart, uint8_t echo, uint32_t timeout) {
	char input[1] = { '\0' };
	HAL_UART_Receive(huart, (uint8_t *)input, 1, timeout);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (uint8_t)input[0];
}
