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
void GPIO_Init();
void Interrupt_Init();
void Terminal_Init();

// -- Global Variables ------
volatile uint32_t TIME_ELAPSED = 0;

//
//
// -- Code Body -------------
//
int main() {
	Sys_Init();
	Timer_Init();
	Interrupt_Init();
    Terminal_Init();

	while (1) {
	}
}

// -- Init Functions --------
//
void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Timer_Init() {
	TIM_HandleTypeDef htim2;
	htim2.Instance = TIM7;
	htim2.Init.Prescaler = (uint32_t) 5400; // 108Mhz / 5400 = 20kHz
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 20000; // 20k -> 1s
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim2);
	HAL_TIM_Base_Start(&htim2);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	__HAL_RCC_TIM7_CLK_ENABLE();
}

void Interrupt_Init() {
//	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

// -- Interrupt Handler -----
void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	C8_flag = 1;
	for (int i = 0; i < 10; i++);
}
