/* ERRORS
 *
 * 	- Bluetooth uses UART -> REQUIRES COM GND!
 *
*	GPIO
*		ENA = PJ1	[D2]
*		IN1 = PF6	[D3]
*		IN2 = PJ0	.
*		IN3 = PC8	.
*		IN4 = PF7	.
*		ENB = PJ3	[D7]
 *
 */

/*---------- DEFINES ---------------------------------------------------------------*/
#define BUFFER_SIZE 100

/*---------- INCLUDES --------------------------------------------------------------*/
#include <stdio.h>

#include "init.h"
#include "bluetooth.h"

/*---------- FUNCTION PROTOTYPES ---------------------------------------------------*/
void Term_Init		( void );		// Clear and reset Terminal
void GPIO_Init		( void );		// Sets up GPIO
void Timer_Init		( void );

void enable_mtr ( void );
void forward	( void );	// MOVEMENT FUNCTIONS FOR MOTOR
void backward 	( void );
void turn_right ( void );
void turn_left	( void );
void front_r	( void );
void front_l	( void );
void back_r		( void );
void back_l		( void );
void stop		( void );

/*---------- HANDLER TYPEDEFS ------------------------------------------------------*/
UART_HandleTypeDef 	bt;
TIM_HandleTypeDef 	htim7;

/*---------- GLOBAL VARIABLE -------------------------------------------------------*/
uint32_t curr_time_in_mili = 0;

/*---------- MAIN PROGRAM ----------------------------------------------------------*/
int main(void){
	Sys_Init();
	GPIO_Init();
	Timer_Init();
	BT_Init(&bt);

	uart_getchar_it(&bt, 0);
	Term_Init();
	enable_mtr();

	// Read the README in the base directory of this project.
	while (1) {  }
}

/*--------- INITIALIZATION FUNCTIONS ----------------------------------------------*/
void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void GPIO_Init( void )
{
	GPIO_InitTypeDef initJ, initF, initC;

	__HAL_RCC_SYSCFG_CLK_ENABLE();

	__HAL_RCC_GPIOJ_CLK_ENABLE();
	initJ.Pin	= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3;
	initJ.Mode	= GPIO_MODE_OUTPUT_PP;
	initJ.Pull	= GPIO_PULLUP;
	initJ.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	__HAL_RCC_GPIOF_CLK_ENABLE();
	initF.Pin	= GPIO_PIN_6 | GPIO_PIN_7;
	initF.Mode	= GPIO_MODE_OUTPUT_PP;
	initF.Pull	= GPIO_PULLUP;
	initF.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	__HAL_RCC_GPIOC_CLK_ENABLE();
	initC.Pin	= GPIO_PIN_8;
	initC.Mode	= GPIO_MODE_OUTPUT_PP;
	initC.Pull	= GPIO_PULLUP;
	initC.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(GPIOJ, &initJ);
	HAL_GPIO_Init(GPIOF, &initF);
	HAL_GPIO_Init(GPIOC, &initC);
}

/*---------- UART FUNCTIONS --------------------------------------------------------*/
void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&bt); }

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	curr_time_in_mili = 0;

	if (huart->Instance == USART6)
	{
		char in = uart_getchar_it(huart, 0);
		switch (in)
		{
		case 'p':
			printf("received ping!\r\n");
			BT_Transmit(&bt, 'p');
			break;
		case 'w':
			printf("w\r\n");
			forward();
			break;
		case 'e':
			printf("e\r\n");
			front_r();
			break;
		case 'd':
			printf("d\r\n");
			turn_right();
			break;
		case 'x':
			printf("x\r\n");
			back_r();
			break;
		case 's':
			printf("s\r\n");
			backward();
			break;
		case 'z':
			printf("z\r\n");
			back_l();
			break;
		case 'a':
			printf("a\r\n");
			turn_left();
			break;
		case 'q':
			printf("q\r\n");
			front_l();
			break;
		default:
			break;
		}
	}
}

void Timer_Init()
{
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t) 1080; // 108Mhz / 1080 = 100kHz
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 100; // 100kHz / 100 = 1kHz -> every 1ms
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
	__HAL_RCC_TIM7_CLK_ENABLE();
//	HAL_NVIC_SetPriority(TIM7_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void TIM7_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM7) {
		curr_time_in_mili++;
	}
	if (curr_time_in_mili >= 200) stop();
}

/*==================================================================================*/
/*
*	GPIO
*		ENA = PJ1	[D2]
*		IN1 = PF6	[D3]
*		IN2 = PJ0	.
*		IN3 = PC8	.
*		IN4 = PF7	.
*		ENB = PJ3	[D7]
*
*/

/*---------- MOTOR FUNCTIONS ------------------------------------------------------*/
void enable_mtr( void )
{
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_1, 1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, 1);
}

void forward( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 0);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 0);
}

void backward( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 0);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);
}

void turn_right( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 0);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 0);
}

void turn_left( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 0);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);
}

void front_r( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 0);
}

void front_l( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 0);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);
}

void back_r( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);
}

void back_l( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 0);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);
}

void stop( void )
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);
}
