//------------------------------------------------------------------------------------
// Hello.c
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
/* GPIO PE8 ---> EXTI8 */   // HAL
#define PJ0_HIGH (GPIOJ->IDR & 1) != 0

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include<stdint.h>

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
volatile uint8_t EXTI0_DETECTED = 0;

//------------------------------------------------------------------------------------
// EXTI0 IRQ
//------------------------------------------------------------------------------------
void EXTI0_IRQHandler()
{
	EXTI0->PR 		= 0x01;	// Reset interrupt flag
	EXTI0_DETECTED 	= 1;	// Set global variable
	for (int i = 0; i < 10; i++);	// Small delay
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

    // enable the GPIO port peripheral clock
    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL

    // initialize GPIO ports
    GPIOJ->MODER &= 0x00; //Bitmask for GPIO J Pin 0 initialization (set it to Input)
    
    // set inputs to pull-up
  	GPIOJ->PUPDR |= 0x01; // J1

    EXTI->IMR 	|= 0x01;
    EXTI->RTSR	|= 0x01;

    // enable NVIC ISER for EXTI0 (pos 6)
    NVIC->ISER = (uint32_t) 1 << 6;

    while(1)
    {
    	if (EXTI0_DETECTED){
    		/* do something */
    		EXTI0_DETECTED = 0;
    	}
    }
}
