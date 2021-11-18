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
void TIM_Init(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt, uint32_t chn);
void DAC_Init(DAC_HandleTypeDef* hdac, DAC_TypeDef* Tgt, uint32_t Chn);
void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn);
void Term_Init();

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
TIM_HandleTypeDef htim3;
DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef ADC1Config;
DMA_HandleTypeDef hdmaadc1, hdmadac1;

uint32_t adc_reading[BUFFER_SIZE] = {0};
uint32_t calculating_buf[BUFFER_SIZE] = {0};
uint32_t dac_response[BUFFER_SIZE] = {0};

uint16_t count = 0;

void GPIO_Init( void )
{
	// Initialize C7 for debug purposes: toggle when ADC triggers
	GPIO_InitTypeDef GPIO_C;
    // enable the GPIO port peripheral clock
	__GPIOC_CLK_ENABLE(); 	// Through HAL
	/* Initialize Pin Numbers */
	GPIO_C.Pin = GPIO_PIN_7;
	/* Initialize Pin Modes */
	GPIO_C.Mode = GPIO_MODE_OUTPUT_PP;
	/* Initialize Pull */
	GPIO_C.Pull = GPIO_NOPULL;
	/* Initialize Speed */
	GPIO_C.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOC, &GPIO_C);

}

void print_buffer(uint32_t* buf) {
	for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
		if (i % 15 == 0) printf("\r\n");
		printf("%6ld", buf[i]);
	}
	printf("\r\n");
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main() {
	// Initialize the system
	Sys_Init();
//	GPIO_Init();
	DAC_Init(&hdac1, DAC1, DAC_CHANNEL_1);
	ADC_Init(&hadc1, ADC1, ADC_CHANNEL_6);
	TIM_Init(&htim3, TIM3, TIM_CHANNEL_4);
	Term_Init();


	// Load Float Constants: s1=0.312500; s2=0.240385; s3=0.296875
	asm("VLDR.F32 s1, =0x3EA00000 \r\n VLDR.F32 s2, =0x3E76277C \r\n VLDR.F32 s3, =0x3E980000");

	HAL_ADC_Start_DMA(&hadc1, adc_reading, BUFFER_SIZE);
//	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, dac_response, BUFFER_SIZE, DAC_ALIGN_12B_R);

	while (1) {
//		HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, dac_response, BUFFER_SIZE, DAC_ALIGN_12B_R);
//		HAL_Delay(1000);
	}
}

// DMA
void DMA2_Stream0_IRQHandler() { // ADC
	HAL_DMA_IRQHandler(&hdmaadc1);
}

void DMA1_Stream5_IRQHandler() { // DAC
	HAL_DMA_IRQHandler(&hdmadac1);
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
//	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // DEBUG
//	Term_Init();
	count = 0;

//	printf("DMA transfer complete, adc_reading: ");
//	print_buffer(adc_reading);

	memcpy(dac_response, adc_reading, BUFFER_SIZE * sizeof(uint32_t));
//	printf("\r\nbuffer transfer complete, calculating_buf: ");
//	print_buffer(calculating_buf);

	for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
		asm("VCVT.F32.U32 s4, %[adc]" : :[adc] "t" (dac_response[i]));
		// s8 = 0.312500 * s4 + 0.240385 * s5 + 0.312500 * s6 + 0.296875 * s7
		asm("VMUL.F32 s8, s1, s4 \r\n VMLA.F32 s8, s2, s5 \r\n VMLA.F32 s8, s1, s6 \r\n VMLA.F32 s8, s3, s7");
		asm("VCVT.U32.F32 %[dac], s8" :[dac] "=t" (dac_response[i]));
		asm("VMOV s6, s5 \r\n VMOV s5, s4 \r\n VMOV s7, s8"); // store previous x and y values
	}
//	printf("\r\nbuffer calculation complete, calculating_buf: ");
//	print_buffer(dac_response);

	HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, dac_response, BUFFER_SIZE, DAC_ALIGN_12B_R);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac) {
//	printf("Output to DAC complete \r\n");
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
//	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
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
	sConfig.DAC_Trigger 		= DAC_TRIGGER_EXT_IT9;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

	HAL_DAC_ConfigChannel(hdac, &sConfig, Chn);

}

void EXTI9_5_IRQHandler(void) {

//	printf("%dEXTI9 triggered: %ld   \r\n", count++, HAL_DAC_GetValue(&hdac1, DAC_CHANNEL_1));
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
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

		// EXTI 9 -> B9 (Arduino D14)
		__GPIOB_CLK_ENABLE();

		GPIO_InitStruct.Mode 	= GPIO_MODE_IT_FALLING;	// IT on Rising
		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;		// No Push-pull
		GPIO_InitStruct.Pin 	= GPIO_PIN_9;		// Pin 4
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}

}


//------------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------------
void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn)
{

	__ADC1_CLK_ENABLE();
	// DMA
//	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	__DMA2_CLK_ENABLE();


	hdmaadc1.Instance 				= DMA2_Stream0;
//		hdmaadc1.Instance 				= DMA2_Stream4;
	hdmaadc1.Init.Channel 			= DMA_CHANNEL_0;
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
	hadc->Init.ExternalTrigConv 	= ADC_EXTERNALTRIGCONV_T3_CC4;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc->Init.ContinuousConvMode 	= DISABLE; // Trigger By Timer
	hadc->Init.DiscontinuousConvMode= DISABLE;
	hadc->Init.ScanConvMode 		= ENABLE;
	hadc->Init.NbrOfConversion		= 1;
//	hadc->DMA_Handle 				= &hdmaadc1;
	hadc->Init.DMAContinuousRequests= ENABLE;
	hadc->Init.EOCSelection			= ADC_EOC_SINGLE_CONV;

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
		__GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;
		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;
		GPIO_InitStruct.Pin 	= GPIO_PIN_6;

		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	}
}

//------------------------------------------------------------------------------------
// Timer 3
//------------------------------------------------------------------------------------

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM3) {
		__HAL_RCC_TIM3_CLK_ENABLE();

		// DEBUG: Output Clock to Arduino Ports (USE TIM3 CHANNEL 3)
		HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);

		__GPIOC_CLK_ENABLE();
		GPIO_InitTypeDef GPIO_InitStruct;

		GPIO_InitStruct.Mode 	= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;
		GPIO_InitStruct.Pin 	= GPIO_PIN_8;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}
}

void TIM_Init(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt, uint32_t chn) {
	htim->Instance = Tgt;
	htim->Init.Prescaler = (uint32_t) 1; // 108MHz
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = (uint32_t) 29700;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_OC_Init(htim);
	TIM_OC_InitTypeDef TIMConfig;
	TIMConfig.OCMode = TIM_OCMODE_TOGGLE;

	HAL_TIM_OC_ConfigChannel(htim, &TIMConfig, chn);
	HAL_TIM_OC_Start(htim, chn);

	HAL_TIM_OC_ConfigChannel(htim, &TIMConfig, TIM_CHANNEL_3);
	HAL_TIM_OC_Start(htim, TIM_CHANNEL_3);
}
