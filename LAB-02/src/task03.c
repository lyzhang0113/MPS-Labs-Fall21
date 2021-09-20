//------------------------------------------------------------------------------------
// task03.c
//------------------------------------------------------------------------------------
//

// -- Imports ---------------
//
#include "stm32f769xx.h"
#include "init.h"

#include <stdint.h>


// -- Prototypes ------------
//
void Timer_Init();
void Terminal_Init();

// -- Global Variables ------
TIM_HandleTypeDef htim7;
volatile uint32_t TIME_ELAPSED = 0;

//
//
// -- Code Body -------------
//
int main() {
	Sys_Init();
	Terminal_Init();
	Timer_Init();

	while (1) ;
}

// -- Init Functions --------
//
void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Timer_Init() {
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t) 5400; // 108Mhz / 5400 = 20kHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 2000; // 2k -> 0.1s
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	__HAL_RCC_TIM7_CLK_ENABLE();
//	HAL_NVIC_SetPriority(TIM7_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

// -- Interrupt Handler -----
void TIM7_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM7) {
	    printf("%ld tenths of a second elapsed since start of program.\r", ++TIME_ELAPSED);
	    fflush(stdout);
	}
}
