// -- Imports ---------------
//
#include "stm32f769xx.h"
#include "init.h"

#include <stdint.h>

//
//
// -- Code Body -------------
//
int main() {
	Sys_Init();
	Init_GPIO();

	while (1) {
		// Main loop code goes here
		printf("\033c\033[36m\033[2J");

		HAL_Delay(1000);
	}
}


void Init_GPIO() {

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	// Enable GPIO clocks?
	// Looks like GPIO reg updates are synced to a base clock.
	//  for any changes to appear the clocks need to be running.
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
	// Enable clock to SYSCONFIG module to enable writing of EXTICRn registers
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// or __HAL_RCC_SYSCFG_CLK_ENABLE();
	// Delay after an RCC peripheral clock enabling
	asm ("nop");
	asm ("nop");

	// Set Pin 13/5 to output. (LED1 and LED2)
	//GPIOJ->MODER

	// GPIO Interrupt
	// By default pin PA0 will trigger the interrupt, change EXTICR1 to route proper pin
	//SYSCFG->EXTICR[0] // EXTICR1-4 are confusingly an array [0-3].

	// Set Pin 0 as input (button) with pull-down.
	//GPIOA->PUPDR

	// Set interrupt enable for EXTI0.
	// NVIC->ISER

	// Unmask interrupt.
	// EXTI->IMR

	// Register for rising edge.
	// EXTI->RTSR

	// And register for the falling edge.
	// EXTI->FTSR
}
