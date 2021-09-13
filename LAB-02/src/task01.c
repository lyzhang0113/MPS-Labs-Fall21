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
/* GPIO JE8 ---> EXTI8 */   // HAL

#define LD1_ON GPIOJ->BSRR = (uint16_t)GPIO_PIN_13
#define LD1_OFF GPIOJ->BSRR = (uint32_t)GPIO_PIN_13 << 16
#define LD2_ON GPIOJ->BSRR = (uint16_t)GPIO_PIN_5
#define LD2_OFF GPIOJ->BSRR = (uint32_t)GPIO_PIN_5 << 16
#define LD3_ON GPIOA->BSRR = (uint16_t)GPIO_PIN_12
#define LD3_OFF GPIOA->BSRR = (uint32_t)GPIO_PIN_12 << 16
#define LD4_ON GPIOD->BSRR = (uint32_t)GPIO_PIN_4 << 16
#define LD4_OFF GPIOD->BSRR = (uint16_t)GPIO_PIN_4

#define D0_HIGH (GPIOC->IDR & 1 << 7) != 0
#define D1_HIGH (GPIOC->IDR & 1 << 6) != 0
#define D2_HIGH (GPIOJ->IDR & 1 << 1) != 0
#define D3_HIGH (GPIOF->IDR & 1 << 6) != 0

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include<stdint.h>


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
    __HAL_RCC_GPIOA_CLK_ENABLE(); 	// Through HAL
    __HAL_RCC_GPIOD_CLK_ENABLE(); 	// Through HAL
    __HAL_RCC_GPIOF_CLK_ENABLE(); 	// Through HAL
    __HAL_RCC_GPIOC_CLK_ENABLE(); 	// Through HAL

    // initialize GPIO ports
    GPIOA->MODER |= 1 << 12 * 2; // Bitmask for GPIO A, set pin 12 to output
    GPIOC->MODER &= 0x00; // Bitmask for GPIO C, default setting (input)
    GPIOJ->MODER |= 1 << 5 * 2; //Bitmask for GPIO J Pin 5 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
    GPIOJ->MODER |= 1 << 13 * 2; //Bitmask for GPIO J Pin 13 initialization (set it to Output mode)
    GPIOD->MODER |= 1 << 4 * 2; // Bitmask for GPIO D Pin 4 initialization (set it to Output mode)
    GPIOF->MODER &= 0x00; // Bitmask for GPIOF, default setting (input)

    // set inputs to pull-up
    GPIOC->PUPDR |= 1 << 7 * 2;// C7
    GPIOC->PUPDR |= 1 << 6 * 2;// C6
	GPIOF->PUPDR |= 1 << 6 * 2;// F6
	GPIOJ->PUPDR |= 1 << 1 * 2;// J1

    while(1)
    {
    	if (D0_HIGH) LD1_ON; else LD1_OFF;
    	if (D1_HIGH) LD2_ON; else LD2_OFF;
    	if (D2_HIGH) LD3_ON; else LD3_OFF;
    	if (D3_HIGH) LD4_ON; else LD4_OFF;
    }
}
