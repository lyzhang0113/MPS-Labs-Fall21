//----------------------------------
// Lab 1 - IDE, ANSI Display, & GPIO - task04.c
//----------------------------------
// Objective:
//   Build a terminal maze game that is at least 10x10 characters. Use WASD to control
// the player. Use pushbuttons to reset. Use LEDs to display status of game.
//

//
//
// -- Defines ---------------
//
#define TERM_WIDTH 80
#define TERM_HEIGHT 24
#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10

#define KEY_W 0x77
#define KEY_A 0x61
#define KEY_S 0x73
#define KEY_D 0x64

#define ON GPIO_PIN_SET
#define OFF GPIO_PIN_RESET
#define RED_LED(STATE) HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, STATE)
#define GREEN_LED(STATE) HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, STATE)

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
void GPIO_Init(void);
void Interrupt_Init(void);

void erase_warning(void);
void print_warning(char* msg);
void reset_maze(void);

//
//
// -- Code Body -------------
//

// 0: empty; 1: wall; 2: entry; 3: exit
char maze[MAZE_HEIGHT*2+1][MAZE_WIDTH*2+1] = {
		"111111111111111111111",
		"200000001000100010001",
		"111010111010101010111",
		"100010101010001010001",
		"111011101011111101011",
		"100010000000100000001",
		"111011101111101110111",
		"101010000000100010001",
		"101011101111111011101",
		"100010100010100000101",
		"101110111010101111101",
		"100000101010001010001",
		"101011101010111011111",
		"101010100000100000101",
		"101110111110101111101",
		"100010100000100000001",
		"111010111011101111101",
		"101000101000000000101",
		"101110101110111111111",
		"100000000000000000003",
		"111111111111111111111"
};

int user_x, user_y; // user location

int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)
    GPIO_Init();
    Interrupt_Init();

    reset_maze();

    while(1)
    {
    	// check if user reached the exit
    	if (maze[user_y][user_x] == '3') {
    		print_warning("You've Reached the EXIT! Congrats!");
    		GREEN_LED(ON);
    		while (1); // wait for reset
    	}

    	// check user input for movement
    	switch(getchar()) {

    	// MOVE UP
    	case KEY_W:
    		erase_warning();
    		if (user_y == 0 || maze[user_y - 1][user_x] == '1') { // wall reached
    			print_warning("You've Reached a Wall!");
    			RED_LED(ON);
    			break;
    		}
    		user_y--;
    		RED_LED(OFF);
    		printf("\033[0;47m  "); // restore cursor position, and erase last position mark
    		printf("\033[2D\033[A\033[42;30m**\033[2D\033[s"); // move UP
    		fflush(stdout);
    		break;

    	// MOVE LEFT
    	case KEY_A:
    		erase_warning();
    		if (user_x == 0 || maze[user_y][user_x - 1] == '1') { // wall reached
    			print_warning("You've Reached a Wall!");
    			RED_LED(ON);
    			break;
    		}
    		user_x--;
    		RED_LED(OFF);
    		printf("\033[0;47m  "); // restore cursor position, and erase last position mark
    		printf("\033[4D\033[42;30m**\033[2D\033[s"); // move LEFT
    		fflush(stdout);
    		break;

    	// MOVE DOWN
    	case KEY_S:
    		erase_warning();
    		if (user_y == MAZE_HEIGHT*2 || maze[user_y + 1][user_x] == '1') { // wall reached
    			print_warning("You've Reached a Wall!");
    			RED_LED(ON);
    			break;
    		}
    		user_y++;
    		RED_LED(OFF);
    		printf("\033[0;47m  "); // restore cursor position, and erase last position mark
    		printf("\033[2D\033[B\033[42;30m**\033[2D\033[s"); // move DOWN
    		fflush(stdout);
    		break;

    	// MOVE RIGHT
    	case KEY_D:
    		erase_warning();
    		if (user_x == MAZE_WIDTH*2 || maze[user_y][user_x + 1] == '1') { // wall reached
    			print_warning("You've Reached a Wall!");
    			RED_LED(ON);
    			break;
    		}
    		user_x++;
    		RED_LED(OFF);
    		printf("\033[0;47m  "); // restore cursor position, and erase last position mark
    		printf("\033[42;30m**\033[2D\033[s"); // move RIGHT
    		fflush(stdout);
    		break;

    	default:
    		print_warning("Invalid Key Pressed!");
    	}

    }
}

//
//
// -- Utility Functions ------
//
void erase_warning() {
	printf("\033[0m\033[2;H\033[K\033[u"); // removes invalid key warning
}

void print_warning(char* msg) {
	int padlen = (TERM_WIDTH - strlen(msg)) / 2;
	printf("\a\033[0m\033[2;H%*s\033[43;34m%s\033[0m%*s\033[u", padlen, "", msg, padlen, "");
	fflush(stdout);
}

void reset_maze() {

    Terminal_Init();

    char* msg = "USE <W><A><S><D> TO MOVE AROUND IN THE MAZE";
    int padlen = (TERM_WIDTH - strlen(msg)) / 2;
    printf("\033[1;H%*s%s%*s\r\n", padlen, "", msg, padlen, "");

    GREEN_LED(OFF);
    RED_LED(OFF);

	user_x = 0; user_y = 1;
	printf("\033[2;H\033[1K");
	printf("\033[0m\033[3;H                   ");
	for (int i = 0; i <= 2 * MAZE_HEIGHT; i++) {
		for (int j = 0; j <= 2 * MAZE_WIDTH; j++) {
			switch (maze[i][j]) {
			case '0':
				printf("\033[47m  ");
				break;
			case '1':
				printf("\033[41m  ");
				break;
			case '2':
				printf("\033[7D\033[0mEnter >\033[s\033[44m**");
				break;
			case '3':
				printf("\033[44m  \033[0m> Exit");
				break;
			default:
				printf("invalid maze");
			}
		}
		printf("\r\n\033[0m                   ");
	}
	printf("\033[u");
	fflush(stdout);
}

//
//
// -- ISRs (IRQs) -------------
//
void EXTI0_IRQHandler(void) {
	EXTI->PR = 0x01;
	NVIC_SystemReset();
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

void GPIO_Init( void )
{
	GPIO_InitTypeDef GPIO_A, GPIO_J;
    // enable the GPIO port peripheral clock
	__HAL_RCC_GPIOA_CLK_ENABLE(); 	// Through HAL
	__HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL
	/* Initialize Pin Numbers */
	GPIO_A.Pin = GPIO_PIN_0; // USER BUTTON
	GPIO_J.Pin = GPIO_PIN_13 | GPIO_PIN_5; // LEDs
	/* Initialize Pin Modes */
	GPIO_A.Mode = GPIO_MODE_INPUT;
	GPIO_J.Mode = GPIO_MODE_OUTPUT_PP;
	/* Initialize Pull */
	GPIO_A.Pull = GPIO_PULLUP;
	GPIO_J.Pull = GPIO_NOPULL;
	/* Initialize Speed */
	GPIO_A.Speed = GPIO_J.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOA, &GPIO_A);
	HAL_GPIO_Init(GPIOJ, &GPIO_J);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET);
}

void Interrupt_Init(void) {
	// Set interrupt on GPIOA Port 0 (USER BUTTON)
	EXTI->IMR |= 0x01;
	EXTI->RTSR |= 0x01;

	NVIC->ISER[0] = (uint32_t) 1 << 6;
}


