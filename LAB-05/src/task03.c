//--------------------------------
// Lab 5 - Direct Memory Access - task03.c
//--------------------------------
//	IIR Filter Implementation: Use ADC and DAC, make an IIR filter with designated
//	response. Assembly math could be used for faster calculation purposes thus
//	better results.
//	+ Use DMA

// ADC1_CHANNEL6 ---> PA6 ---> Arduino A0
// DAC1_CHANNEL1 ---> PA4 ---> Arduino A1

// ADC1: DMA2 Channel 0 Stream 0 or 4


//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <stdio.h>
#include "init.h"

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void DAC_Init(DAC_HandleTypeDef* hdac, DAC_TypeDef* Tgt, uint32_t Chn);
void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn);
void Term_Init();

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef ADC1Config;
DMA_HandleTypeDef hdmaadc1;

uint32_t adc_reading, dac_response;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main() {
	// Initialize the system
	Sys_Init();
	DAC_Init(&hdac1, DAC1, DAC_CHANNEL_1);
	ADC_Init(&hadc1, ADC1, ADC_CHANNEL_6);
	Term_Init();

	// Load Float Constants: s1=0.312500; s2=0.240385; s3=0.296875
	asm("VLDR.F32 s1, =0x3EA00000 \r\n VLDR.F32 s2, =0x3E76277C \r\n VLDR.F32 s3, =0x3E980000");

	HAL_ADC_Start_DMA(&hadc1, &adc_reading, sizeof(uint32_t));

	while (1) ;
}

// DMA
void DMA2_Stream0_IRQHandler() { // ADC
	HAL_DMA_IRQHandler(&hdmaadc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	asm("VCVT.F32.U32 s4, %[adc]" : :[adc] "t" (adc_reading));
	// s8 = 0.312500 * s4 + 0.240385 * s5 + 0.312500 * s6 + 0.296875 * s7
	asm("VMUL.F32 s8, s1, s4 \r\n VMLA.F32 s8, s2, s5 \r\n VMLA.F32 s8, s1, s6 \r\n VMLA.F32 s8, s3, s7");
	asm("VCVT.U32.F32 %[dac], s8" :[dac] "=t" (dac_response));
	asm("VMOV s6, s5 \r\n VMOV s5, s4 \r\n VMOV s7, s8"); // store previous x and y values
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_response);
}

//------------------------------------------------------------------------------------
// Misc. Helper Functions
//------------------------------------------------------------------------------------
void Term_Init(void) {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}
//------------------------------------------------------------------------------------
// DAC
//------------------------------------------------------------------------------------
void DAC_Init(DAC_HandleTypeDef* hdac, DAC_TypeDef* Tgt, uint32_t Chn)
{
	// Enable the DAC Clock.
	__DAC_CLK_ENABLE();

	hdac->Instance = Tgt;

	HAL_DAC_Init(hdac); // Initialize the DAC

	// Configure the DAC channel
	DAC_ChannelConfTypeDef DAC1Config;
	DAC1Config.DAC_Trigger 		= DAC_TRIGGER_NONE;
//	DAC1Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

	HAL_DAC_ConfigChannel(hdac, &DAC1Config, Chn);

	HAL_DAC_Start(hdac, Chn);
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


//------------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------------
void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn)
{

	__ADC1_CLK_ENABLE();

	hadc->Instance 					= Tgt;
	hadc->Init.ClockPrescaler 		= ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.DataAlign 			= ADC_DATAALIGN_RIGHT;
	hadc->Init.Resolution 			= ADC_RESOLUTION_12B;
	hadc->Init.ExternalTrigConv 	= ADC_SOFTWARE_START;
	hadc->Init.ContinuousConvMode 	= ENABLE; // Continuously Trigger ADC Conversion
	hadc->Init.ScanConvMode 		= DISABLE;
	hadc->Init.NbrOfConversion		= 1;
//	hadc->DMA_Handle 				= &hdmaadc1;
	hadc->Init.DMAContinuousRequests= ENABLE;
	hadc->Init.EOCSelection			= ADC_EOC_SEQ_CONV;

	HAL_ADC_Init(hadc);

	ADC1Config.Channel 		= Chn;
	ADC1Config.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	ADC1Config.Rank 		= ADC_REGULAR_RANK_1;
	HAL_ADC_ConfigChannel(&hadc1, &ADC1Config);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if (hadc->Instance == ADC1) {
		__ADC1_CLK_ENABLE();
		__GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;
		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;
		GPIO_InitStruct.Pin 	= GPIO_PIN_6;

		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// DMA
		__DMA2_CLK_ENABLE();
		hdmaadc1.Instance 				= DMA2_Stream0;
	//		hdmaadc1.Instance 				= DMA2_Stream4;
		hdmaadc1.Init.Channel 			= DMA_CHANNEL_0;
		hdmaadc1.Init.PeriphInc 		= DMA_PINC_ENABLE;
		hdmaadc1.Init.Direction 		= DMA_PERIPH_TO_MEMORY;
		hdmaadc1.Init.Mode 				= DMA_CIRCULAR; // Keep writing new ADC reading to same memloc
		hdmaadc1.Init.MemDataAlignment 	= DMA_MDATAALIGN_WORD; // 32 bit
		hdmaadc1.Init.MemInc			= DMA_MINC_ENABLE;
		hdmaadc1.Init.FIFOMode 			= DMA_FIFOMODE_DISABLE; // Using DIRECT Mode
		HAL_DMA_Init(&hdmaadc1);
		__HAL_LINKDMA(hadc, DMA_Handle, hdmaadc1);

		HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	}
}
