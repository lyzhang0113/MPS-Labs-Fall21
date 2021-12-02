
/*---------- INCLUDES --------------------------------------------------------------*/
#include <stdio.h>

#include "init.h"

/*---------- FUNCTION PROTOTYPES ---------------------------------------------------*/
void Term_Init		( void );		// Clear and reset Terminal
void Timer_Init		( void );		// Sets up Timers
void GPIO_Init		( void );		// Configure and enable GPIO
void Interrupt_Init	( void );		// Sets up Interrupts

/*---------- HANDLER TYPEDEFS ------------------------------------------------------*/

/*---------- GLOBAL VARIABLE -------------------------------------------------------*/

/*---------- MAIN PROGRAM ----------------------------------------------------------*/
int main(void){
	Sys_Init();
	Term_Init();
	Timer_Init();
	GPIO_Init();
	Interrupt_Init();

	// Read the README in the base directory of this project.
	while (1)
	{

	}
}

/*--------- INITIALIZATION FUNCTIONS ----------------------------------------------*/
void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Timer_Init( void )
{

}

void GPIO_Init( void )
{

}

void Interrupt_Init( void )
{

}
