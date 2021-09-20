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
volatile uint8_t C8_flag;

//
//
// -- Code Body -------------
//
int main() {
	Sys_Init();
	GPIO_Init();
	Interrupt_Init();
    Terminal_Init();

    C8_flag = 0;

	while (1) {
		if (C8_flag) {
			C8_flag = 0;
			printf("GPIO C Pin 8 Pushbutton has been pressed! \r\n");
		}
	}
}

// -- Init Functions --------
//
void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void GPIO_Init() {

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct_C;
	GPIO_InitStruct_C.Pin = GPIO_PIN_8;
	GPIO_InitStruct_C.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct_C.Pull = GPIO_PULLUP;
	GPIO_InitStruct_C.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct_C);

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
