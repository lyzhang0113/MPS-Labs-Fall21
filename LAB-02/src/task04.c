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

// -- Prototypes ------------
//
void Timer_Init(void);
void Terminal_Init(void);
void GPIO_Init(void);
void Interrupt_Init(void);

void terminal_flash(void);
void print_warning(char* msg);
void erase_warning(void);
void gen_rand_interrupt_period(void);

// -- Global Variables ------
//
TIM_HandleTypeDef htim7;
volatile uint32_t num_trials = 0;
volatile uint32_t total_time_diff_in_mili = 0;
volatile uint32_t start_time = 0;
volatile uint16_t diff = 0;

// -- Code Body -------------
//
int main() {
	Sys_Init();
	Terminal_Init();
	GPIO_Init();
	Interrupt_Init();
	Timer_Init();

	gen_rand_interrupt_period();

	while (1) {
		// TODO: print num_trials, avg_reaction_time, last_reaction_time
		printf("\033[3;HThe previous reaction time is %d\r\n", diff);
		HAL_Delay(100); // Pause for a 100ms
		;
	}
}

// -- Interrupt Handlers -----
//
void TIM7_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM7) {
		printf("TIM7 INTERRUPT\r\n");
	    // stop timer interrupt
	    HAL_TIM_Base_Stop_IT(&htim7);
		// flash screen
	    terminal_flash();
	    // TODO use TIM6 for timeout?
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
		// Stop Timer Interrupt
		HAL_TIM_Base_Stop_IT(&htim7);
		uint32_t curr_time = HAL_GetTick();
		if (start_time == 0 || curr_time < start_time) {
			// pressed early, does not count
			;
		} else {
			// pressed after flashing, display reaction time
			diff = start_time - curr_time;
			total_time_diff_in_mili += diff;
			num_trials++;

			;
		}

		// After pressed, re-generate time duration for next flashing
		gen_rand_interrupt_period();
		break;
	default:
		break;
	}
	for (int i = 0; i < 10; i++);
}

// -- Utility Functions ------
//
void gen_rand_interrupt_period() {
	__HAL_TIM_SET_AUTORELOAD(&htim7, 5000);
	HAL_TIM_Base_Start_IT(&htim7);
}
void terminal_flash() {
	printf("terminal-flash\r\n");
//    printf("\033[0;1;33;44m\033[2J\033[;H"); // Set background color to blue, text to yellow
//    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
//	start_time = HAL_GetTick(); // set time to beginning of flashing
//    HAL_Delay(30); // Pause for a 30ms
//    Terminal_Init();
}
void erase_warning() {
	printf("\033[2;H\033[K"); // removes invalid key warning
	fflush(stdout);
}

void print_warning(char* msg) {
	int padlen = (TERM_WIDTH - strlen(msg)) / 2;
	printf("\a\033[2;H%*s%s%*s", padlen, "", msg, padlen, "");
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
	htim7.Init.Prescaler = (uint32_t) 108000; // 108Mhz / 108000 = 1kHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 5000; // 1kHz / 1000 = 1Hz
	// Period could be from 500 ~ 5000 (0.5s to 5s)
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim7);
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
