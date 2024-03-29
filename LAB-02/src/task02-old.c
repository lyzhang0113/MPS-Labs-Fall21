// ****** IMPORTANT NOTE ******
/*
 * This file is only used in the process of developing,
 * and should not be uploaded as the lab solution.
 */
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
#define PJ0_HIGH (GPIOJ->IDR & 1) != 0

/* TIMER VALUES USED (REGISTER) */
// Prescaler    = 10,800
// Auto-Reload  = 100

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include <stdint.h>

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
volatile uint8_t EXTI0_DETECTED = 0;
volatile uint8_t C8_flag = 0;
//volatile uint32_t TIME_ELAPSED = 0;

//------------------------------------------------------------------------------------
// IRQs
//------------------------------------------------------------------------------------
//void EXTI0_IRQHandler()
//{
//	EXTI->PR 		= 0x01;	    // Reset Interrupt flag
//	EXTI0_DETECTED 	= 1;	    // Set global variable
//	for (int i = 0; i < 10; i++);	// Small delay
//}

void TIM6_DAC_IRQHandler()
{
    TIM6->SR &= (uint16_t) 0xFE;    // Reset Interrupt bit
    /* Task 2 print runtime */
    printf("%ld tenths of a second elapsed since start of program.\r", ++TIME_ELAPSED);
    fflush(stdout);
}


//------------------------------------------------------------------------------------
// Timer (Basic) Initialization
//------------------------------------------------------------------------------------
void Timer_Init( void )
{
    // enable NVIC ISER for TIM6 (pos 55)
    NVIC->ISER[1] = (uint32_t) 1 << (55 % 32);

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // enable TIM6 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    asm("nop");
    asm("nop");     // Two clock cycles

    TIM6->PSC    = (uint16_t) 10800;    // Get 1000 Hz Clock from 108MHz source
    TIM6->ARR    = (uint16_t) 100;      // Every 100 cycles is an overflow
    TIM6->EGR   |= (uint16_t) 1;        // Reinitialize timer counter and update registers
    TIM6->DIER  |= (uint16_t) 1;        // Enable Interrupts
    TIM6->CR1   |= (uint16_t) 1;        // Start Counter
}

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}


//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init();         // This always goes at the top of main (defined in init.c)
    Timer_Init();       // Enable Timers
    Terminal_Init();

    while(1)
    {

    }
}
