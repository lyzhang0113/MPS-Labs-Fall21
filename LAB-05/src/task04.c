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

#define BUFFER_SIZE 1000
//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void TIM2_Init(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt);
void DAC_Init(DAC_HandleTypeDef* hdac, DAC_TypeDef* Tgt, uint32_t Chn);
void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn);
void Term_Init();
void filter();
//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
TIM_HandleTypeDef htim2;
DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef ADC1Config;
DMA_HandleTypeDef hdmaadc1, hdmadac1;

uint32_t adc_reading[BUFFER_SIZE] = {0};
uint32_t dac_response[BUFFER_SIZE] = {0};

static volatile uint32_t *inBufPtr;
static volatile uint32_t *outBufPtr;

//void GPIO_Init( void )
//{
//	// Initialize C7 for debug purposes: toggle when ADC triggers
//	GPIO_InitTypeDef GPIO_C;
//    // enable the GPIO port peripheral clock
//	__GPIOC_CLK_ENABLE(); 	// Through HAL
//	/* Initialize Pin Numbers */
//	GPIO_C.Pin = GPIO_PIN_7;
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
//
//void print_buffer(uint32_t* buf) {
//	for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
//		if (i % 15 == 0) printf("\r\n");
//		printf("%6ld", buf[i]);
//	}
//	printf("\r\n");
//}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main() {
	// Initialize the system
	Sys_Init();
	ADC_Init(&hadc1, ADC1, ADC_CHANNEL_6);
	DAC_Init(&hdac1, DAC1, DAC_CHANNEL_1);
	TIM2_Init(&htim2, TIM2);
	Term_Init();

	// Load Float Constants: s1=0.312500; s2=0.240385; s3=0.296875
	asm("VLDR.F32 s1, =0x3EA00000 \r\n VLDR.F32 s2, =0x3E76277C \r\n VLDR.F32 s3, =0x3E980000");

	HAL_TIM_Base_Start(&htim2);
	HAL_ADC_Start_DMA(&hadc1, adc_reading, BUFFER_SIZE);
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, dac_response, BUFFER_SIZE, DAC_ALIGN_12B_R);

	while (1) {
		filter();
	}
}

void filter() {
	for (uint32_t i = 0; i < BUFFER_SIZE / 2; i++) {
//		outBufPtr[i] = inBufPtr[i];
		asm("VCVT.F32.U32 s4, %[adc]" : :[adc] "t" (inBufPtr[i]));
		// s8 = 0.312500 * s4 + 0.240385 * s5 + 0.312500 * s6 + 0.296875 * s7
		asm("VMUL.F32 s8, s1, s4 \r\n VMLA.F32 s8, s2, s5 \r\n VMLA.F32 s8, s1, s6 \r\n VMLA.F32 s8, s3, s7");
		asm("VCVT.U32.F32 %[dac], s8" :[dac] "=t" (outBufPtr[i]));
		asm("VMOV s6, s5 \r\n VMOV s5, s4 \r\n VMOV s7, s8"); // store previous x and y values
	}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	inBufPtr = &adc_reading[0];
	outBufPtr = &dac_response[BUFFER_SIZE/2];

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	inBufPtr = &adc_reading[BUFFER_SIZE/2];
	outBufPtr = &dac_response[0];

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

	// DMA
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	__DMA1_CLK_ENABLE();

	hdmadac1.Instance 				= DMA1_Stream5;
	hdmadac1.Init.Channel 			= DMA_CHANNEL_7;
	hdmadac1.Init.PeriphInc 		= DMA_PINC_DISABLE;
	hdmadac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdmadac1.Init.Direction 		= DMA_MEMORY_TO_PERIPH;
	hdmadac1.Init.Mode 				= DMA_CIRCULAR; // Keep writing new ADC reading to same memloc
	hdmadac1.Init.MemDataAlignment 	= DMA_MDATAALIGN_WORD; // 32 bit
	hdmadac1.Init.MemInc			= DMA_MINC_ENABLE;
	hdmadac1.Init.FIFOMode 			= DMA_FIFOMODE_DISABLE; // Using DIRECT Mode
	HAL_DMA_Init(&hdmadac1);
	__HAL_LINKDMA(hdac, DMA_Handle1, hdmadac1);

	hdac->Instance = Tgt;
	HAL_DAC_Init(hdac); // Initialize the DAC

	// Configure the DAC channel
	DAC_ChannelConfTypeDef sConfig;
	sConfig.DAC_Trigger 		= DAC_TRIGGER_T2_TRGO;
	sConfig.DAC_OutputBuffer 	= DAC_OUTPUTBUFFER_DISABLE;

	HAL_DAC_ConfigChannel(hdac, &sConfig, Chn);

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
	// DMA
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	__DMA2_CLK_ENABLE();


	hdmaadc1.Instance 				= DMA2_Stream0;
//		hdmaadc1.Instance 				= DMA2_Stream4;
	hdmaadc1.Init.Channel 			= DMA_CHANNEL_0;
	hdmaadc1.Init.Priority			= DMA_PRIORITY_HIGH;
	hdmaadc1.Init.PeriphInc 		= DMA_PINC_DISABLE;
	hdmaadc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdmaadc1.Init.Direction 		= DMA_PERIPH_TO_MEMORY;
	hdmaadc1.Init.Mode 				= DMA_CIRCULAR; // Keep writing new ADC reading to same memloc
	hdmaadc1.Init.MemDataAlignment 	= DMA_MDATAALIGN_WORD; // 32 bit
	hdmaadc1.Init.MemInc			= DMA_MINC_ENABLE;
	hdmaadc1.Init.FIFOMode 			= DMA_FIFOMODE_DISABLE; // Using DIRECT Mode
	HAL_DMA_Init(&hdmaadc1);
	__HAL_LINKDMA(hadc, DMA_Handle, hdmaadc1);

	hadc->Instance 					= Tgt;
	hadc->Init.ClockPrescaler 		= ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.DataAlign 			= ADC_DATAALIGN_RIGHT;
	hadc->Init.Resolution 			= ADC_RESOLUTION_12B;
	hadc->Init.ExternalTrigConv 	= ADC_EXTERNALTRIGCONV_T2_TRGO;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc->Init.ContinuousConvMode 	= DISABLE; // Trigger By Timer
	hadc->Init.DiscontinuousConvMode= DISABLE;
	hadc->Init.ScanConvMode 		= DISABLE;
	hadc->Init.NbrOfConversion		= 1;
//	hadc->DMA_Handle 				= &hdmaadc1;
	hadc->Init.DMAContinuousRequests= ENABLE;
	hadc->Init.EOCSelection			= ADC_EOC_SINGLE_CONV;

	HAL_ADC_Init(hadc);

	ADC1Config.Channel 		= Chn;
	ADC1Config.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	ADC1Config.Rank 		= ADC_REGULAR_RANK_1;
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

// DMA
void DMA1_Stream5_IRQHandler() { // DAC
	HAL_DMA_IRQHandler(&hdmadac1);
}
void DMA2_Stream0_IRQHandler() { // ADC
	HAL_DMA_IRQHandler(&hdmaadc1);
}

// TIM 2
void TIM2_Init(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt) {
	__TIM2_CLK_ENABLE();
	htim->Instance = Tgt;
	htim->Init.Prescaler = (uint32_t) 54; // 108MHz
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = (uint32_t) 2;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(htim);

	TIM_ClockConfigTypeDef sClockSourceConfig;
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig);
	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
	HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig);

}
