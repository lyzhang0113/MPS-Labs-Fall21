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

#define USER_BUTTON_PRESSED GPIOA->IDR & 1 != 0

#define KEY_W 0x77
#define KEY_A 0x61
#define KEY_S 0x73
#define KEY_D 0x64

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
    __HAL_RCC_GPIOA_CLK_ENABLE(); 	// Through HAL

    while(1)
    {
    	if (USER_BUTTON_PRESSED) {
    		while (USER_BUTTON_PRESSED) ;
    		; // reset maze
    	}
    }
}
