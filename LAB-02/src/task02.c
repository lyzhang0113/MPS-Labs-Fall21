//------------------------------------------------------------------------------------
// Lab 2 - Interrupts and Timers - task02.c
//------------------------------------------------------------------------------------
// Objective:
//   Use registers to configure TIM6 to count time,
// and display the time after the program is started.
//

/* TIMER VALUES USED (REGISTER) */
// Prescaler    = 10,800
// Auto-Reload  = 1000

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include <stdint.h>

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void Timer_Init(void);
void Terminal_Init(void);

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
volatile uint32_t TIME_ELAPSED = 0;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init();         // This always goes at the top of main (defined in init.c)
    Timer_Init();       // Enable Timers
    Terminal_Init();

    while(1);
}

//------------------------------------------------------------------------------------
// IRQs
//------------------------------------------------------------------------------------
void TIM6_DAC_IRQHandler()
{
    TIM6->SR &= (uint16_t) ~1;    // Reset Interrupt bit
    /* Task 2 print runtime */
	printf("%ld tenths of a second elapsed since start of program.\r", ++TIME_ELAPSED);
	fflush(stdout);
    for (int i = 0; i < 10; i++);
}


//------------------------------------------------------------------------------------
// Initialization
//------------------------------------------------------------------------------------
void Timer_Init( void )
{
    // enable NVIC ISER for TIM6 (pos 54)
    NVIC->ISER[1] = (uint32_t) 1 << (54 % 32);

    // enable TIM6 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    asm("nop");
    asm("nop");     // Two clock cycles

    TIM6->PSC   = (uint16_t) 10799;    // Get 10 kHz Clock from 108MHz source
    TIM6->ARR   = (uint16_t) 999;		// Every 1000 cycles is an overflow
    TIM6->EGR   |= (uint16_t) 1;        // Reinitialize timer counter and update registers
    TIM6->DIER  |= (uint16_t) 1;        // Enable Interrupts
    TIM6->CR1   |= (uint16_t) 1;        // Start Counter
}

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}
