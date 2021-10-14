//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"

void configureADC();

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();

	// Code goes here
}

void configureADC()
{
	// Enable the ADC Clock.

	HAL_ADC_Init(...); // Initialize the ADC

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

	// Configure the ADC channel

	HAL_ADC_ConfigChannel(...,...);
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

// GPIO init

}
