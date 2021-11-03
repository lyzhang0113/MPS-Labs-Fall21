//--------------------------------
// Lab 4 - Analog Conversion and Digital Signal Processing - task01.c
//--------------------------------
//	Simple Voltmeter: Sample a voltage on any ADC, calculate and display
//	the values read by the ADC and the voltage.


//------------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------------
#define TERM_HEIGHT 24
#define VREF 0b111111111111UL // 4095

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "init.h"
#include <stdio.h>
#include <float.h>

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
ADC_HandleTypeDef 		hADC;
ADC_InitTypeDef			hinitADC;
ADC_ChannelConfTypeDef 	sCONFIG;
GPIO_InitTypeDef		GPIO_A_ADC, GPIO_A_PB;

double 		ADC_val, ADC_avg, ADC_hi, ADC_lo, ADC_vals[16];
uint32_t 	ADC_hex = 0;
uint8_t 	ind = 0;

//------------------------------------------------------------------------------------
// ADC Configuration and Initialization
//------------------------------------------------------------------------------------

void ADC_Config( void )
{
	// Enable the ADC Clock.
	__ADC1_CLK_ENABLE();

	hADC.Instance 	= ADC1;

	// Setup Init Structure
	hinitADC.ClockPrescaler		= ADC_CLOCK_SYNC_PCLK_DIV8;
	hinitADC.DataAlign			= ADC_DATAALIGN_RIGHT;
	hinitADC.Resolution			= ADC_RESOLUTION_12B;
	hinitADC.ContinuousConvMode	= DISABLE;
	hinitADC.ExternalTrigConv	= ADC_SOFTWARE_START;


	hADC.Init		= hinitADC;

	HAL_ADC_Init(&hADC); // Initialize the ADC

	sCONFIG.Channel			= ADC_CHANNEL_6;
	sCONFIG.SamplingTime	= ADC_SAMPLETIME_480CYCLES;		// Total Time: 492 CYC
	sCONFIG.Rank			= ADC_REGULAR_RANK_1;
	sCONFIG.Offset			= 0;

	// Configure the ADC channel

	HAL_ADC_ConfigChannel(&hADC, &sCONFIG);
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	__GPIOA_CLK_ENABLE();	// ADC1_IN6 => PA6

	// GPIO init
	GPIO_A_ADC.Pin 		= GPIO_PIN_6;
	GPIO_A_ADC.Mode 	= GPIO_MODE_ANALOG;
	GPIO_A_ADC.Pull 	= GPIO_NOPULL;
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

//------------------------------------------------------------------------------------
// Misc. Helper Functions
//------------------------------------------------------------------------------------

void Terminal_Init( void )
{
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
    printf("\033[HLow: \033[1;25HHigh: \033[1;50HAverage: \r\n");
    printf("\033[2;2HCurrent Measurement (Voltage): \r\n");
    printf("\033[3;2HCurrent Measurement (Decimal): \r\n");
    printf("\033[H");
    fflush(stdout);
}

void Var_Init( void )
{
	ADC_val = (double) 0.0;
	ADC_avg = (double) 0.0;
	ADC_hi  = (double) 0.0;
	ADC_lo  = (double) 100.0;

	bzero(&ADC_vals, 16*sizeof(double));
}

double ADC_to_double( uint32_t val )
{
	if ( val < (uint32_t) 10) return (double)0.0;
	return (double)((double)3.3/VREF) * val;
}

void ADC_calc_avg( double val )
{
	double sum = 0;

	if (ind < 16) {
		ADC_vals[ind++] = val;
	} else {
		for (int i = 0; i < 15; i++) {
			ADC_vals[i] = ADC_vals[i+1];
		}
		ADC_vals[ind-1] = val;
	}
	for (int v = 0; v < ind; v++) {
		sum += ADC_vals[v];
	}
	ADC_avg = (double)(sum/ind);
}

void ADC_print( double val )
{
	Terminal_Init();

	printf("\033[1;5H %f V\033[1;30H %f V\033[1;59H %f V\r\n", ADC_lo, ADC_hi, ADC_avg);
	printf("\033[2;40H %f V", ADC_val);
	fflush(stdout);
	printf("\033[3;40H 0x%lX", ADC_hex);
	fflush(stdout);
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
	//Initialize the system
	Sys_Init();
	ADC_Config();
	GPIO_Init();
	Terminal_Init();
	Var_Init();

	while(1){
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));

			HAL_ADC_ConfigChannel(&hADC, &sCONFIG);
			HAL_ADC_Start(&hADC);

			while (HAL_ADC_PollForConversion(&hADC, 100000) != HAL_OK);
			ADC_hex = HAL_ADC_GetValue(&hADC);
			ADC_val = ADC_to_double(ADC_hex);

			ADC_calc_avg(ADC_val);
			ADC_print	(ADC_val);

			if (ADC_val > ADC_hi) ADC_hi = ADC_val;
			if (ADC_val < ADC_lo) ADC_lo = ADC_val;

			HAL_Delay(100);
		}
	}
}
