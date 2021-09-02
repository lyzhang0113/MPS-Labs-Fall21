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

#define ESC 0x1B

#define TERM_WIDTH 80
#define TERM_HEIGHT 24

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
    int unprintable_cursor_pos = 12;

    printf("\033[0;1;33;44m"); // Set background color to blue, text to yellow
    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

    char* msg = "PRESS <ESC> OR <CTL>+[ TO QUIT";
    int padlen = (TERM_WIDTH - strlen(msg)) / 2;
    printf("\033[2;H%*s%s%*s\r\n", padlen, "", msg, padlen, "");

    while(1)
    {
        choice = getchar();
        if (choice == ESC)  { while(1); } // halt if ESC or CTL+[
        if (choice > 31 && choice != 127) {
        	// printable character
            printf("\033[6;HThe keyboard character is \033[1;31m%c\033[1;33m.\r\n", choice);
        } else {
        	// if bottom reached, all previous message should move up, and reset cursor to line 24
        	if (unprintable_cursor_pos == TERM_HEIGHT + 1) {
        		printf("\033[12;%ir\033D", TERM_HEIGHT);
        		unprintable_cursor_pos = TERM_HEIGHT;
        	}
        	// unprintable
        	printf("\033[%d;H\033[5;33;44mThe keyboard character \033[31m$%x\033[33m is \033[4m'not printable'\033[24m.\033[0;1;33;44m\r\n", unprintable_cursor_pos++, choice);
        }

    }
}
