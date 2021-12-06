/* ERRORS
 *
 * 	- Bluetooth uses UART -> REQUIRES COM GND!
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
void Timer_Init		( void );		// Sets up Timers
void Interrupt_Init	( void );		// Sets up Interrupts

void DAC_Init	( DAC_HandleTypeDef *hdac );		// Configure and enable DAC (GPIO, TMR, etc.)

/*---------- HANDLER TYPEDEFS ------------------------------------------------------*/
UART_HandleTypeDef 	bt;
SPI_HandleTypeDef 	hspi1;
DAC_HandleTypeDef	hdac1;

/*---------- GLOBAL VARIABLE -------------------------------------------------------*/


/*---------- MAIN PROGRAM ----------------------------------------------------------*/
int main(void){
	Sys_Init();
	Timer_Init();
	Interrupt_Init();
	BT_Init(&bt);
	DAC_Init(&hdac1);

	uart_getchar_it(&bt, 0);
	Term_Init();

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

void DAC_Init( DAC_HandleTypeDef *hdac )
{
	// Enable the DAC Clock.
	__DAC_CLK_ENABLE();

	hdac->Instance = DAC1;

	HAL_DAC_Init(hdac); // Initialize the DAC

	// Configure the DAC channel
	DAC_ChannelConfTypeDef DAC1Config;
	DAC1Config.DAC_Trigger 		= DAC_TRIGGER_NONE;
	DAC1Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

	HAL_DAC_ConfigChannel(hdac, &DAC1Config, DAC_CHANNEL_1);

	HAL_DAC_Start(hdac, DAC_CHANNEL_1);
}

void Interrupt_Init( void )
{

}

/*---------- UART FUNCTIONS --------------------------------------------------------*/
void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&bt); }

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	if (huart->Instance == USART6)
	{
		char in = uart_getchar_it(huart, 0);
		switch (in)
		{
		case 'p':
			printf("Received!\r\n");
			HAL_UART_Transmit(huart, &in, 1, 100);
			break;
		default:
			HAL_UART_Transmit(huart, 1, 1, 100);
			printf("%c\r\n", in);
			break;
		}
	}
}

/*---------- DAC FUNCTIONS ---------------------------------------------------------*/
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{

	GPIO_InitTypeDef  GPIO_InitStruct;

	if (hdac->Instance == DAC1) {
		__GPIOA_CLK_ENABLE(); // GPIO A4 used for DAC1

		GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;	// Analog Mode
		GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;		// No Push-pull
		GPIO_InitStruct.Pin 	= GPIO_PIN_4;		// Pin 4
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}

}

/*---------- HELPER FUNCTIONS ------------------------------------------------------*/

