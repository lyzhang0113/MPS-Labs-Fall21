//--------------------------------
// Lab 4 - Analog Conversion and Digital Signal Processing - task05.c
//--------------------------------
//	[Depth] Frequency Mixer: Implement a mixer and filter using two ADC
//	channels as input, and DAC_OUT1 as output.
//

// ADC1_CHANNEL6 ---> PA6 ---> Arduino A0
// ADC3_CHANNEL8 ---> PF10 --> Arduino A3
// DAC_OUT1	     ---> PA4 ---> Arduino A1
// DAC_OUT2 is shared with the USB 2.0 On-the-Go host controller thus not used

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <stdio.h>
#include "init.h"

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void initTIM(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt, uint32_t chn);
void initDAC(DAC_HandleTypeDef* hdac, DAC_TypeDef* Tgt, uint32_t chn);
void initADC(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t chn);
void reset_terminal();

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
TIM_HandleTypeDef htim3;
DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc1, hadc3;

uint32_t adc_x, adc_y;
int32_t dac_res;
uint8_t adc_flags = 0x00; // bit1: adc1; bit3: adc3

uint8_t bias_measured = 0;
uint32_t bias_x = 0, bias_y = 0;

//------------------------------------------------------------------------------------
// GPIO Init (DEBUG)
//------------------------------------------------------------------------------------
//void GPIO_Init( void )
//{
//	// Initialize C6 C7 for debug purposes: toggle when ADC triggers
//	GPIO_InitTypeDef GPIO_C;
//    // enable the GPIO port peripheral clock
//	__HAL_RCC_GPIOC_CLK_ENABLE(); 	// Through HAL
//	/* Initialize Pin Numbers */
//	GPIO_C.Pin = GPIO_PIN_6 | GPIO_PIN_7;
//	/* Initialize Pin Modes */
//	GPIO_C.Mode = GPIO_MODE_OUTPUT_PP;
//	/* Initialize Pull */
//	GPIO_C.Pull = GPIO_NOPULL;
//	/* Initialize Speed */
//	GPIO_C.Speed = GPIO_SPEED_HIGH;
//
//	HAL_GPIO_Init(GPIOC, &GPIO_C);
//
//}


//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main() {
	// Initialize the system
	Sys_Init();
//	GPIO_Init(); // DEBUG
	initTIM(&htim3, TIM3, TIM_CHANNEL_4);
	initDAC(&hdac1, DAC1, DAC_CHANNEL_1);
	initADC(&hadc1, ADC1, ADC_CHANNEL_6);
	initADC(&hadc3, ADC3, ADC_CHANNEL_8);
	reset_terminal();

	// Load Float Constants
	asm("VLDR.F32 s1, =0x3a83126f \r\n VLDR.F32 s2, =0xbb03126f \r\n VLDR.F32 s3, =0x404a9fbe");
	asm("VLDR.F32 s4, =0xc08d6042 \r\n VLDR.F32 s5, =0x4041cac1 \r\n VLDR.F32 s6, =0xbf6a3d71");
	asm("VLDR.F32 s16, =0x44ffe000 \r\n VLDR.F32 s17, =0x44ffe000 \r\n VLDR.F32 s18, =0x44ffe000 \r\n VLDR.F32 s19, =0x44ffe000");

	while (1) {

		if (adc_flags == 0x0A) {
			if (bias_measured == 0) {
				for (uint32_t i = 0; i < 2000000; i++) {
					bias_x += adc_x;
					bias_y += adc_y;
				}
				bias_measured = 1;
				bias_x /= 2000000; bias_y /= 2000000;
				printf("DC Bias X = %ld     DC Bias Y = %ld     \r\n", bias_x, bias_y);
				continue;
			}
			/*
			 * Using MAC Float Assembly
				 * s1: 0.001	-> 0x3a83126f
				 * s2: -0.002	-> 0xbb03126f
				 * s3: 3.166	-> 0x404a9fbe
				 * s4: -4.418	-> 0xc08d6042
				 * s5: 3.028	-> 0x4041cac1
				 * s6: -0.915	-> 0xbf6a3d71
				 * s10: x_k
				 * s11: x_k1
				 * s12: x_k2
				 * s13: x_k3
				 * s14: x_k4
				 * s20: y_k
				 * s16: y_k1
				 * s17: y_k2
				 * s18: y_k3
				 * s19: y_k4
			 * Load s10
			 * s20 := s1 * s10 + s2 * s12 + s1 * s14 + s3 * s16 + s4 * s17 + s5 * s18 + s6 * s19
			 * Store s20
			 * s11 := s10; s12 := s11; s13 := s12; s14 := s13
			 * s16 := s20; s17 := s16; s18 := s17; s19 := s18
			 */
			int32_t adc_x_adj = adc_x - bias_x;
			int32_t adc_y_adj = adc_y - bias_y;
			int32_t mix = adc_x_adj * adc_y_adj / 2047;

			asm volatile("VCVT.F32.S32 s10, %[in]" : :[in] "t" (mix));
			asm volatile("VMUL.F32 s20, s1, s10 \r\n VMLA.F32 s20, s2, s12 \r\n VMLA.F32 s20, s1, s14");
			asm volatile("VMLA.F32 s20, s3, s16 \r\n VMLA.F32 s20, s4, s17 \r\n VMLA.F32 s20, s5, s18");
			asm volatile("VMLA.F32 s20, s6, s19");
			asm volatile("VCVT.S32.F32 %[out], s20" :[out] "=t" (dac_res));
			asm volatile("VMOV s14, s13 \r\n VMOV s13, s12 \r\n VMOV s12, s11 \r\n VMOV s11, s10");
			asm volatile("VMOV s19, s18 \r\n VMOV s18, s17 \r\n VMOV s17, s16 \r\n VMOV s16, s20");
			dac_res += bias_x > bias_y ? bias_x : bias_y;
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t) dac_res);
			adc_flags = 0x00;
		}
	}
}


void reset_terminal() {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

//------------------------------------------------------------------------------------
// DAC
//------------------------------------------------------------------------------------
void initDAC(DAC_HandleTypeDef* hdac, DAC_TypeDef* Tgt,uint32_t chn)
{
	// Enable the DAC Clock.
	__DAC_CLK_ENABLE();

	hdac->Instance = Tgt;
	HAL_DAC_Init(hdac); // Initialize the DAC

	// Configure the DAC channel
	DAC_ChannelConfTypeDef DACConfig;
	DACConfig.DAC_Trigger 		= DAC_TRIGGER_NONE;
//	DACConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

	HAL_DAC_ConfigChannel(hdac, &DACConfig, chn);
	HAL_DAC_Start(hdac, chn);
}


void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{

	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;	// Analog Mode
	GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;		// No Push-pull

	if (hdac->Instance == DAC1) {
		__GPIOA_CLK_ENABLE(); // GPIO A4 used for DAC1
		GPIO_InitStruct.Pin 	= GPIO_PIN_4;		// Pin 4
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}


//------------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------------
void initADC(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t chn)
{
	if (Tgt == ADC1) {
		__ADC1_CLK_ENABLE();
	}
	else if (Tgt == ADC3) {
		__ADC3_CLK_ENABLE();
	}

	hadc->Instance = Tgt;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_CC4;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc->Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc->Init.ContinuousConvMode = DISABLE;
	hadc->Init.ScanConvMode = DISABLE;
	HAL_ADC_Init(hadc);

	ADC_ChannelConfTypeDef ADCConfig;
	ADCConfig.Channel = chn;
	ADCConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	ADCConfig.Rank = 1;
	HAL_ADC_ConfigChannel(hadc, &ADCConfig);
	HAL_ADC_Start_IT(hadc);
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;
	GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;

	if (hadc->Instance == ADC1) {
		// A6
		__GPIOA_CLK_ENABLE();
		GPIO_InitStruct.Pin 	= GPIO_PIN_6;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	} else if (hadc->Instance == ADC3) {
		// F10
		__GPIOF_CLK_ENABLE();
		GPIO_InitStruct.Pin 	= GPIO_PIN_10;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	}

	// IT
	HAL_NVIC_SetPriority(ADC_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler() {
	HAL_ADC_IRQHandler(&hadc1);
	HAL_ADC_IRQHandler(&hadc3);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == ADC1) {
		adc_x = HAL_ADC_GetValue(&hadc1);
		adc_flags |= 1 << 1;
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // DEBUG
	} else if (hadc->Instance == ADC3) {
		adc_y = HAL_ADC_GetValue(&hadc3);
		adc_flags |= 1 << 3;
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // DEBUG
	}
}

//------------------------------------------------------------------------------------
// Timer 3
//------------------------------------------------------------------------------------

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM3) {
		__HAL_RCC_TIM3_CLK_ENABLE();

		// DEBUG: Output Clock to Arduino Ports (USE TIM3 CHANNEL 3)
//		HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);
//		HAL_NVIC_EnableIRQ(TIM3_IRQn);

//		__GPIOC_CLK_ENABLE();
//		GPIO_InitTypeDef GPIO_InitStruct;
//
//		GPIO_InitStruct.Mode 	= GPIO_MODE_AF_PP;
//		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
//		GPIO_InitStruct.Pull 	= GPIO_NOPULL;
//		GPIO_InitStruct.Pin 	= GPIO_PIN_8;
//		GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
//		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);3
	}
}

void initTIM(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt, uint32_t chn) {
	htim->Instance = Tgt;
	htim->Init.Prescaler = (uint32_t) 108; // 108Mhz / 108 = 1MHz
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = 3;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_OC_Init(htim);
	TIM_OC_InitTypeDef TIMConfig;
	TIMConfig.OCMode = TIM_OCMODE_TOGGLE;

	HAL_TIM_OC_ConfigChannel(htim, &TIMConfig, chn);
	// Rising Edge every 123kHz
	HAL_TIM_OC_Start(htim, chn);
}

