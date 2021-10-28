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

DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef ADC1Config;

//uint16_t x_k = 0, x_k1 = 0, x_k2 = 0, y_k1 = 0, y_k = 0;
int main() {
	// Initialize the system
	Sys_Init();
	initDAC1(&hdac1);
	initADC1(&hadc1);
	reset_terminal();

	// Load Float Constants: s1=0.312500; s2=0.240385; s3=0.296875
	asm("VLDR.F32 s1, =0x3EA00000 \r\n VLDR.F32 s2, =0x3E76277C \r\n VLDR.F32 s3, =0x3E980000");

	uint32_t adc_reading = 0, dac_response = 0;
	uint32_t x_k1 = 0, x_k2 = 0, y_k1 = 0;

	while (1) {

		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		adc_reading = HAL_ADC_GetValue(&hadc1);

		/*
		 * Using MAC Float Assembly
			 * s1: 0.312500
			 * s2: 0.240385
			 * s3: 0.296875
			 * s4: x_k
			 * s5: x_k1
			 * s6: x_k2
			 * s7: y_k1
			 * s8: y_k
		 */
		asm("VCVT.F32.U32 s4, %[adc]" : :[adc] "t" (adc_reading));
		// s8 = 0.312500 * s4 + 0.240385 * s5 + 0.312500 * s6 + 0.296875 * s7
		asm("VMUL.F32 s8, s1, s4 \r\n VMLA.F32 s8, s2, s5 \r\n VMLA.F32 s8, s1, s6 \r\n VMLA.F32 s8, s3, s7");
		asm("VCVT.U32.F32 %[dac], s8" :[dac] "=t" (dac_response));
		asm("VMOV s6, s5 \r\n VMOV s5, s4 \r\n VMOV s7, s8"); // store previous x and y values

		/*
		 * Using c Float Math
		 */
//		dac_response = (uint32_t) (0.312500f * adc_reading + 0.240385f * x_k1 + 0.312500f * x_k2 + 0.296875f * y_k1);
//		x_k2 = x_k1; x_k1 = adc_reading; y_k1 = dac_response;


		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_response);
	}
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
//	DAC1Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

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
	hadc->Init.ContinuousConvMode = DISABLE;
	hadc->Init.ScanConvMode = DISABLE;

	HAL_ADC_Init(hadc);

	ADC1Config.Channel = ADC_CHANNEL_6;
	ADC1Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	ADC1Config.Rank = 1;
	HAL_ADC_ConfigChannel(&hadc1, &ADC1Config);
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
