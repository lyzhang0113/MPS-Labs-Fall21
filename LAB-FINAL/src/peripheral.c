/* ERRORS
 *
 * 	- Bluetooth uses UART -> REQUIRES COM GND!
 *
*	GPIO
*		ENA = PF6	[D3]
*		IN1 = PJ1	[D2]
*		IN2 = PJ0	.
*		IN3 = PC8	.
*		IN4 = PJ3	[D7]
*		ENB = PF7	[D6]
 *
 *	BYTE STRUCTURE:
 * [ Parity check [1] | Speed [4] | Direction [3] ]
 *
 *
 */

/*---------- DEFINES ---------------------------------------------------------------*/
#define BUFFER_SIZE 100
#define K_PWM		650

/*---------- INCLUDES --------------------------------------------------------------*/
#include <stdio.h>

#include "init.h"
#include "bluetooth.h"

/*---------- FUNCTION PROTOTYPES ---------------------------------------------------*/
void Term_Init		( void );		// Clear and reset Terminal
void GPIO_Init		( void );		// Sets up GPIO
void Timer_Init		( void );


void forward	( void );	// MOVEMENT FUNCTIONS FOR MOTOR
void backward 	( void );
void turn_right ( void );
void turn_left	( void );
void front_r	( void );
void front_l	( void );
void back_r		( void );
void back_l		( void );
void stop		( void );

void Set_PWM	( char speed );

/*---------- HANDLER TYPEDEFS ------------------------------------------------------*/
UART_HandleTypeDef 	bt;
TIM_HandleTypeDef 	htim7, htim10, htim11;
TIM_OC_InitTypeDef	htim10_ch1, htim11_ch1;

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
	GPIO_InitTypeDef initJ, initF1, initF2, initC;

	__HAL_RCC_SYSCFG_CLK_ENABLE();

	__HAL_RCC_GPIOJ_CLK_ENABLE();
	initJ.Pin	= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3;
	initJ.Mode	= GPIO_MODE_OUTPUT_PP;
	initJ.Pull	= GPIO_PULLUP;
	initJ.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	__HAL_RCC_GPIOF_CLK_ENABLE();
	initF1.Pin	= GPIO_PIN_6;
	initF1.Mode	= GPIO_MODE_AF_PP;
	initF1.Pull	= GPIO_NOPULL;
	initF1.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	initF1.Alternate = GPIO_AF3_TIM10;

	initF2.Pin	= GPIO_PIN_7;
	initF2.Mode	= GPIO_MODE_AF_PP;
	initF2.Pull	= GPIO_NOPULL;
	initF2.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	initF2.Alternate = GPIO_AF3_TIM11;

	__HAL_RCC_GPIOC_CLK_ENABLE();
	initC.Pin	= GPIO_PIN_8;
	initC.Mode	= GPIO_MODE_OUTPUT_PP;
	initC.Pull	= GPIO_PULLUP;
	initC.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(GPIOJ, &initJ);
	HAL_GPIO_Init(GPIOF, &initF1);
	HAL_GPIO_Init(GPIOF, &initF2);
	HAL_GPIO_Init(GPIOC, &initC);
}

/*---------- UART FUNCTIONS --------------------------------------------------------*/
void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&bt); }


/*
*	BYTE STRUCTURE:
* [ Parity check [1] | Speed [4] | Direction [3] ]
*
*
*/
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	char in, dir, spd;

	curr_time_in_mili = 0;

	if (huart->Instance == USART6)
	{
		in = uart_getchar_it(huart, 0);

		if (!parity_check(in)) return;
		else {
			dir = (in & 0b00000111);
			spd = ((in >> 3) & 0b00001111) * K_PWM;
		}

		Set_PWM(spd);

		switch (dir)
		{
		case 0b000:
			printf("w\r\n");
			forward();
			break;
		case 0b001:
			printf("e\r\n");
			front_r();
			break;
		case 0b010:
			printf("d\r\n");
			turn_right();
			break;
		case 0b011:
			printf("x\r\n");
			back_r();
			break;
		case 0b100:
			printf("s\r\n");
			backward();
			break;
		case 0b101:
			printf("z\r\n");
			back_l();
			break;
		case 0b110:
			printf("a\r\n");
			turn_left();
			break;
		case 0b111:
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

	htim10.Instance = TIM10;
	htim10.Init.Prescaler = (uint32_t) 108;
	htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim10.Init.Period = 10000;
	htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim10);

	htim10_ch1.OCMode = TIM_OCMODE_PWM1;
	htim10_ch1.Pulse = 0;
	htim10_ch1.OCPolarity = TIM_OCPOLARITY_HIGH;
	htim10_ch1.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim10, &htim10_ch1, TIM_CHANNEL_1);


	htim11.Instance = TIM11;
	htim11.Init.Prescaler = (uint32_t) 108;
	htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim11.Init.Period = 10000;
	htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim10);

	htim11_ch1.OCMode = TIM_OCMODE_PWM1;
	htim11_ch1.Pulse = 0;
	htim11_ch1.OCPolarity = TIM_OCPOLARITY_HIGH;
	htim11_ch1.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim11, &htim11_ch1, TIM_CHANNEL_1);

	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
}

void HAL_TIM_PWM_MspInit( TIM_HandleTypeDef *htim )
{
	__HAL_RCC_TIM10_CLK_ENABLE();
	__HAL_RCC_TIM11_CLK_ENABLE();

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
	if (curr_time_in_mili >= SAMPLING_FREQ) stop();
}

/*==================================================================================*/
/*
 *	GPIO
*		ENA = PF6	[D3]
*		IN1 = PJ1	[D2]
*		IN2 = PJ0	.
*		IN3 = PC8	.
*		IN4 = PJ3	[D7]
*		ENB = PF7	[D6]
*/

/*---------- MOTOR FUNCTIONS ------------------------------------------------------*/

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
//	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_1, 0);
//	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, 0);

//	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, 1);
//	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);
//
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
//	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);

	Set_PWM((char)0);
}

void Set_PWM(char speed)
{
	TIM10->CCR1 = (uint32_t)speed;
	TIM11->CCR1 = (uint32_t)speed;
}
