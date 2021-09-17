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

#define LD1_ON GPIOJ->BSRR = (uint16_t)GPIO_PIN_13 // Turn on Red LED (LED1)
#define LD1_OFF GPIOJ->BSRR = (uint32_t)GPIO_PIN_13 << 16 // Turn off Red LED (LED1)
#define LD2_ON GPIOJ->BSRR = (uint16_t)GPIO_PIN_5 // Turn on Green LED (LED2)
#define LD2_OFF GPIOJ->BSRR = (uint32_t)GPIO_PIN_5 << 16 // Turn off Green LED (LED2)

#define ESC 0x1B

#define TERM_WIDTH 80
#define TERM_HEIGHT 24

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include<stdint.h>

void beep() {
	GPIOB->BSRR = (uint16_t)GPIO_PIN_15; // Turn on beeper
	HAL_Delay(30); // Pause for a 100ms
	GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16; // Turn off beeper
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    char choice;

    printf("\033[0;1;33;44m"); // Set background color to blue, text to yellow
    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL
    GPIOJ->MODER |= 1 << 5 * 2; //Bitmask for GPIO J Pin 5 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
    GPIOJ->MODER |= 1 << 13 * 2; //Bitmask for GPIO J Pin 13 initialization (set it to Output mode)

    __HAL_RCC_GPIOB_CLK_ENABLE(); 	// Through HAL
    GPIOB->MODER |= 1 << 15 * 2; //Bitmask for GPIO B Pin 15 initialization (set it to Output mode)

    char* msg = "PRESS <ESC> OR <CTL>+[ TO QUIT";
    int padlen = (TERM_WIDTH - strlen(msg)) / 2;
    printf("\033[2;H%*s%s%*s\r\n", padlen, "", msg, padlen, "");
    printf("\033[12;%ir\033[11;H\033[s", TERM_HEIGHT); // Set scroll section, save cursor position

    GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16; // Turn off beeper
    LD2_ON;
    LD1_ON;

    while(1)
    {
        choice = getchar();
        if (choice == ESC)  {
        	LD2_OFF;
        	LD1_OFF;
        	while(1);
        } // halt if ESC or CTL+[
        if (choice > 31 && choice != 127) {
        	// printable character
        	LD2_ON;
        	LD1_OFF;
            printf("\033[6;HThe keyboard character is \033[1;31m%c\033[1;33m. \r\n", choice);
        } else {
        	// unprintable
        	LD2_OFF;
        	LD1_ON;
        	printf("\a\033[u\r\n\033[5;33;44mThe keyboard character \033[31m$%02x\033[33m is \033[4m'not printable'\033[24m.\033[0;1;33;44m \033[s", choice);
        	fflush(stdout);
        	beep();
        }

    }
}
