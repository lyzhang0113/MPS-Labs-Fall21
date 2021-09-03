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

    char choice;
    printf("\033[0m\033[2J\033[;H"); // Reset all attributes & Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

    // Need to enable clock for peripheral bus on GPIO Port J
    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL
    GPIOJ->MODER |= 1 << 5 * 2; //Bitmask for GPIO J Pin 5 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
    GPIOJ->BSRR = (uint16_t)GPIO_PIN_5; // Turn on Green LED (LED2)

    printf("PRESS <ESC> OR <CTL>+[ TO QUIT\r\n"); // print instruction msg

    while(1)
    {
        choice = getchar();
        if (choice == ESC) { // halt on ESC or CTL+[
        	GPIOJ->BSRR = (uint32_t)GPIO_PIN_5 << 16;
        	while(1);
        }
        printf("The keyboard character is %c.\r\n", choice);
    }
}
