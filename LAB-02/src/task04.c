//----------------------------------
// Lab 2 - Interrupts and Timers - task04.c
//----------------------------------
// Objective:
//
//

// -- Defines ---------------
//
#define TERM_WIDTH 80
#define TERM_HEIGHT 24

// -- Imports ---------------
//
#include "stm32f769xx.h"
#include "init.h"

#include<stdint.h>
#include<stdlib.h>

// -- Prototypes ------------
//
void Timer_Init(void);
void Terminal_Init(void);
void GPIO_Init(void);
void Interrupt_Init(void);

void terminal_flash(void);
void print_banner(char* msg, uint8_t line_num, uint8_t beep);
void erase_warning(void);

// -- Global Variables ------
//
TIM_HandleTypeDef htim7;
volatile uint32_t curr_time_in_mili = 0;

volatile uint8_t user_btn_pressed = 0;

volatile uint16_t num_trials = 0;
volatile uint32_t react_time_sum = 0;
volatile uint32_t react_time_last = 0;

// -- Code Body -------------
//
int main() {
	Sys_Init();
	Terminal_Init();
	GPIO_Init();
	Interrupt_Init();
	Timer_Init();
	srand(HAL_GetTick());

	while (1) {
		user_btn_pressed = 0;
		// print num_trials, avg_reaction_time, last_reaction_time
		printf("\033[4;HNumber of Trials: %d   ", num_trials);
		printf("\033[5;HAverage Reaction Time: %ld ms  ", num_trials == 0 ? 0 : react_time_sum / num_trials);
		if (react_time_last) {
			printf("\033[6;HLast Reaction Time: %ld ms  ", react_time_last);
		}
		uint16_t rand_period = rand() % 5000 + 1000;
		print_banner("When ready, Please Press the Blue Button", 2, 0);
		//press USER_BTN to begin
		while (!user_btn_pressed);
		user_btn_pressed = 0;
		curr_time_in_mili = 0;
		// wait for some random time
		while (curr_time_in_mili < rand_period) ;
		if (user_btn_pressed) {
			user_btn_pressed = 0;
			// Pressed too early, doesn't count, display warning
			print_banner("You pressed the button too early, this doesn't count!", 10, 1);
			continue;
		}
		// flash & start counting (reset timer count)
		terminal_flash();
		curr_time_in_mili = 0;
		// add timeout
		while (!user_btn_pressed);
		user_btn_pressed = 0;
		react_time_last = curr_time_in_mili;
		num_trials++;
		react_time_sum += react_time_last;
	}
}

// -- Interrupt Handlers -----
//
void TIM7_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM7) {
		curr_time_in_mili++;
	}
}

void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void EXTI0_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case GPIO_PIN_8:
		// External PB Pressed ->  Reset
		NVIC_SystemReset();
		break;
	case GPIO_PIN_0:
		// USER_PB Pressed
		user_btn_pressed = 1;
		break;
	default:
		break;
	}
}

// -- Utility Functions ------
//

void terminal_flash() {
    printf("\a\033[0;1;33;44m\033[2J\033[;H"); // Set background color to blue, text to yellow
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
    HAL_Delay(30); // Pause for a 30ms
    Terminal_Init();
}
void erase_warning() {
	printf("\033[2;H\033[K"); // removes invalid key warning
	fflush(stdout);
}

void print_banner(char* msg, uint8_t line_num, uint8_t beep) {
	erase_warning();
	int padlen = (TERM_WIDTH - strlen(msg)) / 2;
	if (beep) printf("\a");
	printf("\033[%d;H%*s%s%*s", line_num, padlen, "", msg, padlen, "");
	fflush(stdout);
}

// -- Init Functions --------
//
void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Timer_Init() {
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t) 1080; // 108Mhz / 1080 = 100kHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 100; // 100kHz / 100 = 1kHz -> every 1ms
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	__HAL_RCC_TIM7_CLK_ENABLE();
//	HAL_NVIC_SetPriority(TIM7_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void GPIO_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStruct_A; // A0 for USER_BTN
	GPIO_InitTypeDef GPIO_InitStruct_C; // C8
    // enable the GPIO port peripheral clock
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE(); 	// Through HAL
	/* Initialize Pin Numbers */
	GPIO_InitStruct_A.Pin = GPIO_PIN_0; // USER BUTTON
	GPIO_InitStruct_C.Pin = GPIO_PIN_8;
	/* Initialize Pin Modes */
	GPIO_InitStruct_A.Mode = GPIO_InitStruct_C.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct_A.Pull = GPIO_InitStruct_C.Pull = GPIO_PULLUP;
	/* Initialize Speed */
	GPIO_InitStruct_A.Speed = GPIO_InitStruct_C.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_A);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct_C);
}

void Interrupt_Init() {
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

