//--------------------------------
// Lab 4 - Analog COnv. and Digital Signal Processing - task04.c
//--------------------------------
//
//


#include <stdio.h>
#include "init.h"

void initDAC1(DAC_HandleTypeDef* hdac);
void initADC1(ADC_HandleTypeDef* hadc);
void reset_terminal();

//uint32_t calc_using_c_float();
uint16_t calc_using_asm(uint16_t new_adc_reading);

DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc1;

//uint16_t x_k = 0, x_k1 = 0, x_k2 = 0, y_k1 = 0, y_k = 0;
uint32_t y_k = 0;
int main() {
	// Initialize the system
	Sys_Init();
	initDAC1(&hdac1);
	initADC1(&hadc1);
	reset_terminal();

	// Load Float Constants: s1=0.312500; s2=0.240385; s3=0.296875
	asm volatile ("VLDR.F32 s1, =0x3EA00000 \r\n VLDR.F32 s2, =0x3E76277C \r\n VLDR.F32 s3, =0x3E980000");

	while (1) {
//		x_k2 = x_k1;
//		x_k1 = x_k;
//		x_k = (uint16_t) HAL_ADC_GetValue(&hadc1);
//		y_k1 = y_k;
		y_k = calc_using_asm(HAL_ADC_GetValue(&hadc1));
//		printf("\rx_k2 = %d, x_k1 = %d, x_k = %d, y_k1 = %d, y_k = %d", x_k2, x_k1, x_k, y_k1, y_k);
//		fflush(stdout);
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, y_k);
	}
}

//uint16_t calc_using_c_float() {
//	// Generate the output value using c style floating point equation
//	return (uint16_t) ((float)0.312500 * x_k + (float)0.240385 * x_k1 + (float)0.312500 * x_k2 + (float)0.296875 * y_k1);
//}

uint32_t calc_using_asm(uint32_t new_adc_reading) {
	/*
	 * s1: 0.312500
	 * s2: 0.240385
	 * s3: 0.296875
	 * s4: x_k
	 * s5: x_k1
	 * s6: x_k2
	 * s7: y_k1
	 * s8: y_k
	 */
	float res = (float) new_adc_reading;
	asm("VLDR.F32 s4, %0" : : "m" (res));
	asm("VMOV s6, s5 \r\n VMOV s5, s4 \r\n VMOV s7, s8"); // store previous x and y values

	// s8 = 0.312500 * s4 + 0.240385 * s5 + 0.312500 * s6 + 0.296875 * s7
	asm("VMUL.F32 s8, s1, s4 \r\n VMLA.F32 s8, s2, s5 \r\n VMLA.F32 s8, s1, s6 \r\n VMLA.F32 s8, s3, s7");
	asm("VSTR s8, %0" : "=m" (res));
//	asm("VCVT"); // round to int
	return (uint32_t) res;

}




void reset_terminal() {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

// -------------- DAC -----------------
void initDAC1(DAC_HandleTypeDef* hdac)
{
	// Enable the DAC Clock.
	__DAC_CLK_ENABLE();

	hdac->Instance = DAC1;

	HAL_DAC_Init(hdac); // Initialize the DAC

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


// --------------- ADC --------------------
void initADC1(ADC_HandleTypeDef* hadc)
{
	__ADC1_CLK_ENABLE();

	hadc->Instance = ADC1;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc->Init.ContinuousConvMode = ENABLE;

	HAL_ADC_Init(hadc);

	ADC_ChannelConfTypeDef ADC1Config;
	ADC1Config.Channel = ADC_CHANNEL_6;
	ADC1Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	ADC1Config.Rank = 1;
	ADC1Config.Offset = 0;

	HAL_ADC_ConfigChannel(&hadc1, &ADC1Config);
	HAL_ADC_Start(&hadc1);

}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if (hadc->Instance == ADC1) {
		__GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;
		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;
		GPIO_InitStruct.Pin 	= GPIO_PIN_6;

		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}
