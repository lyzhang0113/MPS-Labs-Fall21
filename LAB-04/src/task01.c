//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"


ADC_HandleTypeDef 		hADC;
ADC_InitTypeDef			hinitADC;
ADC_ChannelConfTypeDef 	sCONFIG;
GPIO_InitTypeDef		GPIO_A_ADC, GPIO_A_PB;

uint32_t	vals[16];


void ADC_Config( void )
{
	// Enable the ADC Clock.
	__ADC1_CLK_ENABLE();

	hADC.Instance 	= ADC1;

	// Setup Init Structure
	hinitADC.ClockPrescaler		= ADC_CLOCK_SYNC_PCLK_DIV2;
	hinitADC.Resolution			= ADC_RESOLUTION_12B;
	hinitADC.ContinuousConvMode	= DISABLE;
	hinitADC.ExternalTrigConv	= ADC_SOFTWARE_START;


	hADC.Init		= hinitADC;

	HAL_ADC_Init(&hADC); // Initialize the ADC

	/* Available sampling times:

		ADC_SAMPLETIME_3CYCLES
	  	ADC_SAMPLETIME_15CYCLES
		ADC_SAMPLETIME_28CYCLES
		ADC_SAMPLETIME_56CYCLES
		ADC_SAMPLETIME_84CYCLES
		ADC_SAMPLETIME_112CYCLES
		ADC_SAMPLETIME_144CYCLES
		ADC_SAMPLETIME_480CYCLES

	*/
	sCONFIG.Channel			= ADC_CHANNEL_6;
	sCONFIG.SamplingTime	= ADC_SAMPLETIME_15CYCLES;		// Total Time: 492 CYC

	// Configure the ADC channel

	HAL_ADC_ConfigChannel(&hADC, &sCONFIG);
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	__GPIOA_CLK_ENABLE();	// ADC1_IN6 => PA6

	// GPIO init
	GPIO_A_ADC.Pin 		= GPIO_PIN_6;
	GPIO_A_ADC.Mode 	= GPIO_MODE_ANALOG;
	GPIO_A_ADC.Speed 	= GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_A_ADC);
}

void GPIO_Init( void )
{
	__GPIOA_CLK_ENABLE();

	GPIO_A_PB.Pin		= GPIO_PIN_0;
	GPIO_A_PB.Mode		= GPIO_MODE_INPUT;
	GPIO_A_PB.Speed		= GPIO_SPEED_HIGH;


	HAL_GPIO_Init(GPIOA, &GPIO_A_PB);
}

void calc_and_print( uint32_t val, uint8_t ind )
{

}

void Terminal_Init( void )
{
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	ADC_Config();
	GPIO_Init();
	Terminal_Init();

	uint8_t		index = 0, nonzero = 0;

	uint32_t 	ADC_value = 0, hi = 0, lo = INT32_MAX;
	double 		ADC_avg = 0;

	bzero(&vals, 16*sizeof(uint32_t));

	while(1){
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));

			HAL_ADC_ConfigChannel(&hADC, &sCONFIG);
			HAL_ADC_Start(&hADC);

			while (HAL_ADC_PollForConversion(&hADC, 100000) != HAL_OK);
			ADC_value = HAL_ADC_GetValue(&hADC);

			if (ADC_value > hi) hi = ADC_value;
			if (ADC_value < lo) lo = ADC_value;


			if (index > 15) {
				for (int i = 0; i < 15; i++) {
					vals[i] = vals[i+1];
				}
				vals[15] 	= ADC_value;
			} else
				vals[index] = ADC_value;


			for (int i = 0; i < 16; i++) {
				if (vals[i] != 0) nonzero++;
				ADC_avg += vals[i];
			}

			ADC_avg = (float)ADC_avg/nonzero;

			Terminal_Init();
			printf("\033[HLow: %ld\033[0;20HHigh: %ld\033[0;40HAverage:%f\r\n", lo, hi, ADC_avg);
			printf("\tMost Recent Measurement:\t%ld\r\n", ADC_value);
			printf("\tMost Recent Measurement (Hex):\t%lX", ADC_value);
			fflush(stdout);

			index++;
			nonzero = 0;
			ADC_avg = 0;
		}
	}
}
