//------------------------------------------------------------------------------------
// task01.c
//------------------------------------------------------------------------------------
//
// Test program to demonstrate serial port I/O.  This program writes a message on
// the console using the printf() function, and reads characters using the getchar()
// function.  An ANSI escape sequence is used to clear the screen if a '2' is typed.
// A '1' repeats the message and the program responds to other input characters with
// an appropriate message.
//
// Any valid keystroke turns on the green LED on the board; invalid entries turn it off
//

/* GPIO PJ0 ---> EXTI0 */   // Register
/* GPIO PC8 ---> EXTI8 */   // HAL

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include <stdint.h>

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
#define EXTI0_HIGH (GPIOJ->IDR & 1) != 0

volatile uint8_t EXTI0_DETECTED = 0;
volatile uint8_t EXTI8_DETECTED = 0;

//------------------------------------------------------------------------------------
// IRQs
//------------------------------------------------------------------------------------
void EXTI0_IRQHandler()
{
	EXTI->PR 		|= 0x01;	    // Reset Interrupt flag
	EXTI0_DETECTED 	= 1;	    // Set global variable
	for (int i = 0; i < 10; i++) asm("nop");	// Small delay
}

void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	EXTI8_DETECTED = 1;
}

//------------------------------------------------------------------------------------
// Register Initialization
//------------------------------------------------------------------------------------
void Register_Init( void )
{
    // enable NVIC ISER for EXTI0 (pos 6)
    NVIC->ISER[0] = (uint32_t) 1 << (6 % 32);

    // enable the GPIO port peripheral clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // initialize GPIO ports
    GPIOJ->MODER &= 0x00; //Bitmask for GPIO J Pin 0 initialization (set it to Input)
    
    // set inputs to pull-up
  	GPIOJ->PUPDR |= 0x01; // J1

  	// Set up EXTI0
    EXTI->IMR 	|= 0x01;
    EXTI->FTSR	|= 0x01;

    SYSCFG->EXTICR[0] |= (uint8_t) 9;	// Setup GPIO Interrupt

    for (int i  = 0; i < 10; i++) asm("nop");
}

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void GPIO_Init_HAL() {

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct_C;
	GPIO_InitStruct_C.Pin = GPIO_PIN_8;
	GPIO_InitStruct_C.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct_C.Pull = GPIO_PULLUP;
	GPIO_InitStruct_C.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct_C);

}

void Interrupt_Init_HAL() {
//	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init();         // This always goes at the top of main (defined in init.c)
    Register_Init();    // Enable registers
    GPIO_Init_HAL();
    Interrupt_Init_HAL();

    Terminal_Init();

    while(1)
    {
        /* Task 1 check PB */
    	if (EXTI0_DETECTED){
    	    printf("GPIO Pin J0 has been triggered!\r\n");
    	    EXTI0_DETECTED = 0;
    	}
		if (EXTI8_DETECTED) {
			EXTI8_DETECTED = 0;
			printf("GPIO C Pin 8 Pushbutton has been pressed! \r\n");
		}
    }
}
