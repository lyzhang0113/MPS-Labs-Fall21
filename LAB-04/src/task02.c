//--------------------------------
// Lab 4 - Analog COnv. and Digital Signal Processing - task01.c
//--------------------------------
//
//

// DAC_OUT1	--->	PA4	--->	Arduino A1
// DAC_OUT2 is shared with the USB 2.0 On-the-Go host controller thus not used

#define TERM_WIDTH 80
#define TERM_HEIGHT 24

#define KEY_1 0x31
#define KEY_2 0x32

#include "init.h"
#include <stdio.h>

void initDAC1(DAC_HandleTypeDef* hdac);
void Terminal_Init();
void erase_line(uint8_t line_num);
void print_banner(char* msg, uint8_t line_num, uint8_t beep);


DAC_HandleTypeDef hdac1;
uint8_t dac_out = 0x00;

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	initDAC1(&hdac1);
	Terminal_Init();

	while (1) {
		printf("Please Select Mode:\r\n");
		printf("  1: Sawtooth Wave\r\n");
		printf("  2: Read From ADC\r\n");
		switch (getchar()) {
		case KEY_1:
			Terminal_Init();
			while (1) {
				HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_8B_R, (uint32_t) dac_out++);
			}
			break;
		case KEY_2:
			Terminal_Init();
			break;
		default:
			Terminal_Init();
			print_banner("Invalid Input!", 22, 1);
			break;
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

void initDAC1(DAC_HandleTypeDef* hdac)
{
	// Enable the DAC Clock.
	__DAC_CLK_ENABLE();

	hdac->Instance = DAC1;

	HAL_DAC_Init(&hdac1); // Initialize the DAC

	// Configure the DAC channel
	DAC_ChannelConfTypeDef DAC1Config;
	DAC1Config.DAC_Trigger 		= DAC_TRIGGER_NONE;
	DAC1Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

	HAL_DAC_ConfigChannel(hdac, &DAC1Config, DAC_CHANNEL_1);

	HAL_DAC_Start(hdac, DAC_CHANNEL_1);
}


void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{

	GPIO_InitTypeDef  GPIO_InitStruct;

	if (hdac->Instance == DAC1) {
		__GPIOA_CLK_ENABLE(); // GPIO A4 used for DAC1

		GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;	// Analog Mode
		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;		// No Push-pull
		GPIO_InitStruct.Pin 	= GPIO_PIN_4;		// Pin 4
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}

}
