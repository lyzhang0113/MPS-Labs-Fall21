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
void EXTI0_IRQHandler()
{
	EXTI->PR 		= 0x01;	    // Reset Interrupt flag
	EXTI0_DETECTED 	= 1;	    // Set global variable
	for (int i = 0; i < 10; i++);	// Small delay
}

//void TIM6_DAC_IRQHandler()
//{
//    TIM6->SR &= (uint16_t) 0xFE;    // Reset Interrupt bit
//    /* Task 2 print runtime */
//    printf("%d tenths of a second elapsed since start of program.\r\n", ++TIME_ELAPSED);
//}

void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	C8_flag = 1;
	for (int i = 0; i < 10; i++);
}

//------------------------------------------------------------------------------------
// Register Initialization
//------------------------------------------------------------------------------------
void Register_Init( void )
{
    // enable NVIC ISER for EXTI0 (pos 6)
    NVIC->ISER[0] = (uint32_t) 1 << (6 % 32);

    // enable the GPIO port peripheral clock
    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL

    // initialize GPIO ports
    GPIOJ->MODER &= 0x00; //Bitmask for GPIO J Pin 0 initialization (set it to Input)

    // set inputs to pull-up
  	GPIOJ->PUPDR |= 0x01; // J1

    EXTI->IMR 	|= 0x01;
    EXTI->RTSR	|= 0x01;
}

//------------------------------------------------------------------------------------
// Timer (Basic) Initialization
//------------------------------------------------------------------------------------
//void Timer_Init( void )
//{
//    // enable NVIC ISER for TIM6 (pos 55)
//    NVIC->ISER[1] = (uint32_t) 1 << (55 % 32);
//
//    // enable TIM6 clock
//    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
//    asm("nop");
//    asm("nop");     // Two clock cycles
//
//    TIM6->PSC    = (uint16_t) 10800;    // Get 1000 Hz Clock from 108MHz source
//    TIM6->ARR    = (uint16_t) 100;      // Every 100 cycles is an overflow
//    TIM6->EGR   |= (uint16_t) 1;        // Reinitialize timer counter and update registers
//    TIM6->DIER  |= (uint16_t) 1;        // Enable Interrupts
//    TIM6->CR1   |= (uint16_t) 1;        // Start Counter
//}

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
//    Timer_Init();       // Enable Timers
    GPIO_Init_HAL();
    Interrupt_Init_HAL();

    Terminal_Init();

    while(1)
    {
        /* Task 1 check PB */
    	if (EXTI0_DETECTED){
    	    printf("GPIO Pin J0 has been triggered!\r\n");
            EXTI0_DETECTED = 0;     // reset global variable
    	}
		if (C8_flag) {
			C8_flag = 0;
			printf("GPIO C Pin 8 Pushbutton has been pressed! \r\n");
		}
    }
}
