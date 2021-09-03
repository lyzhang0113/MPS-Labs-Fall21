//------------------------------------------------------------------------------------
// Task01.c
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

#define ESC 0x1B

/*
 * LD1 ===(red)===> GPIO_J PIN 13
 * LD2 ==(green)==> GPIO_J PIN 5
 * LD3 ==(green)==> GPIO_A PIN 12
 * LD4 ===(red)===> GPIO_D PIN 4
*/


//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include<stdlib.h>
#include<stdint.h>

GPIO_InitTypeDef LD_J, LD_A, LD_D;

void LED_Init( void )
{
	/* Initialize Pin Numbers */
	LD_J.Pin = GPIO_PIN_13 | GPIO_PIN_5;
	LD_A.Pin = GPIO_PIN_12;
	LD_D.Pin = GPIO_PIN_4;

	/* Initialize Pin Modes */
	LD_J.Mode = LD_A.Mode = LD_D.Mode = GPIO_MODE_OUTPUT_PP;

	/* Initialize Pull */
	LD_J.Pull = LD_A.Pull = LD_D.Pull = GPIO_PULLUP;

	/* Initialize Speed */
	LD_J.Speed = LD_A.Speed = LD_D.Speed = GPIO_SPEED_MEDIUM;
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    char choice;
    printf("\033[0m\033[2J\033[;H"); // Reset all attributes & Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

    // Need to enable clock for peripheral bus on GPIO Port J
    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // Initialize GPIO structs
    HAL_GPIO_Init(GPIOJ, &LD_J);
    HAL_GPIO_Init(GPIOA, &LD_A);
    HAL_GPIO_Init(GPIOD, &LD_D);

}
