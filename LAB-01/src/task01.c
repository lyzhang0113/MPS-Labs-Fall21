//----------------------------------
// Lab 1 - IDE, ANSI Display, & GPIO - task01.c
//----------------------------------
// Objective:
//   Build a program that outputs back what the user types, and halt when ESC is pressed.
//

//
//
// -- Defines ---------------
//
#define ESC 0x1B

//
//
// -- Imports ---------------
//
#include "stm32f769xx.h"
#include "init.h"

#include<stdint.h>

//
//
// -- Prototypes ------------
//
void Terminal_Init(void);

//
//
// -- Code Body -------------
//
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)
    Terminal_Init();

    printf("PRESS <ESC> OR <CTL>+[ TO QUIT\r\n"); // print instruction msg

    while(1)
    {
    	char choice = getchar();
        if (choice == ESC) { // halt on ESC or CTL+[
        	while(1);
        }
        printf("The keyboard character is %c.\r\n", choice);
    }
}

//
//
// -- Init Functions ----------
//

// This function resets the terminal
void Terminal_Init(void) {
	// Reset all attributes & Erase screen & move cursor to home position & reset scroll section
    printf("\033[0m\033[2J\033[;H\033[r");
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}
