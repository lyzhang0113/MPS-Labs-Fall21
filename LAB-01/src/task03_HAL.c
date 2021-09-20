//------------------------------------------------------------------------------------
// task03_HAL.c
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

GPIO_InitTypeDef LD_J, LD_A, LD_D, AH_C, AH_J, AH_F;


void LED_Init( void )
{
	/* Initialize Pin Numbers */
    LD_J.Pin = GPIO_PIN_13 | GPIO_PIN_5;    // LED Pins
    LD_A.Pin = GPIO_PIN_12;
    LD_D.Pin = GPIO_PIN_4;
    AH_C.Pin = GPIO_PIN_6 | GPIO_PIN_7;     // Arduino Pins
    AH_J.Pin = GPIO_PIN_1;
    AH_F.Pin = GPIO_PIN_6;

	/* Initialize Pin Modes */
    LD_J.Mode = LD_A.Mode = LD_D.Mode = GPIO_MODE_OUTPUT_PP;
    AH_C.Mode = AH_J.Mode = AH_F.Mode = GPIO_MODE_INPUT;

	/* Initialize Pull */
    LD_J.Pull = LD_A.Pull = LD_D.Pull = GPIO_NOPULL;
    AH_C.Pull = AH_J.Pull = AH_F.Pull = GPIO_PULLUP;

	/* Initialize Speed */
    LD_J.Speed = LD_A.Speed = LD_D.Speed = GPIO_SPEED_MEDIUM;
    AH_C.Speed = AH_J.Speed = AH_F.Speed = GPIO_SPEED_MEDIUM;
}

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    GPIO_PinState PC7, PC6, PJ1, PF6;

    Sys_Init(); // This always goes at the top of main (defined in init.c)
    LED_Init();

    printf("\033[0m\033[2J\033[;H"); // Reset all attributes & Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

    // Need to enable clock for peripheral bus on GPIO Port J, A, and D
    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    // Initialize GPIO structs
    HAL_GPIO_Init(GPIOJ, &LD_J);
    HAL_GPIO_Init(GPIOA, &LD_A);
    HAL_GPIO_Init(GPIOD, &LD_D);
    HAL_GPIO_Init(GPIOJ, &AH_J);
    HAL_GPIO_Init(GPIOC, &AH_C);
    HAL_GPIO_Init(GPIOF, &AH_F);

    while (1)
    {
        /* Corresponding Pairs
        * -------------------
        * PJ13 == PC7
        * PJ5  == PC6
        * PA12 == PJ1
        * PD4  == PF6
        */
        
        /* Read Pins */
        PC7  = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
        PC6  = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
        PJ1  = HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1);
        PF6  = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) ^ 1;

        /* Write Pins */
        HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, PC7);
        HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, PC6);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, PJ1);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, PF6);
    }
}
