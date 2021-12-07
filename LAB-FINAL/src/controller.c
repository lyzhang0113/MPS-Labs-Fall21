//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

// ADC1_CHANNEL6 ---> PA6 ---> Arduino A0
// ADC3_CHANNEL8 ---> PF10 --> Arduino A3

#define CALIBRATION 0

#define NEU_X 1179
#define NEU_Y 1157
#define MIN_X 6
#define MIN_Y 7
#define MAX_X 3749
#define MAX_Y 3878

#include <stdio.h>
#include <stdlib.h>
#include "../Libraries/BSP/STM32F769I-Discovery/stm32f769i_discovery_lcd.h"
#include "init.h"
#include "bluetooth.h"

void LCD_Init(void);
void TIM2_Init(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt);
void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn, DMA_HandleTypeDef* hdma);

void Adjust_Joystick_Readings();
void Calibrate_Joystick(uint32_t round);
void Term_Init(void);

UART_HandleTypeDef bt;

TIM_HandleTypeDef htim2;
ADC_HandleTypeDef hadc1, hadc3;
DMA_HandleTypeDef hdmaadc1, hdmaadc3;

uint32_t raw_x, raw_y;
uint32_t neu_x, neu_y, max_x, min_x, max_y, min_y;
int8_t adj_x, adj_y;


int main(void) {
	// Initialization
	Sys_Init();
	Term_Init();
	BT_Init(&bt);
	ADC_Init(&hadc1, ADC1, ADC_CHANNEL_6, &hdmaadc1);
	ADC_Init(&hadc3, ADC3, ADC_CHANNEL_8, &hdmaadc3);
	TIM2_Init(&htim2, TIM2);
	LCD_Init();

	// Start Necessary Components
//	BT_Connect(&bt);
//	uart_getchar_it(&bt, 0);
	HAL_TIM_Base_Start(&htim2);
	HAL_ADC_Start_DMA(&hadc1, &raw_y, sizeof(uint32_t));
	HAL_ADC_Start_DMA(&hadc3, &raw_x, sizeof(uint32_t));

	if (CALIBRATION) {
		Calibrate_Joystick(300);
	} else {
		printf("Joystick Calibration Configuration Found!\r\n");
		neu_x = NEU_X; neu_y = NEU_Y;
		min_x = MIN_X; min_y = MIN_Y;
		max_x = MAX_X; max_y = MAX_Y;
	}


	while (1) {
		HAL_Delay(200);
		Adjust_Joystick_Readings();
		char jotstick_reading_buf[60];
		snprintf(jotstick_reading_buf, 60, "             x: %4d          y: %4d     ", adj_x, adj_y);
		BSP_LCD_DisplayStringAtLine(0, (uint8_t*)jotstick_reading_buf);

		if (abs(adj_x) < 50 && abs(adj_y) < 50) { // STOP
			BSP_LCD_ClearStringLine(2);
		} else if (adj_x > 50 && adj_y > 50) { // NORTHEAST
			BSP_LCD_DisplayStringAtLine(2, "          NORTHEAST          ");
			BT_Transmit(&bt, 'e');
		} else if (adj_x > 50 && adj_y < -50) { // SOUTHEAST
			BSP_LCD_DisplayStringAtLine(2, "          SOUTHEAST          ");
			BT_Transmit(&bt, 'x');
		} else if (adj_x < -50 && adj_y < -50) { // SOUTHWEST
			BSP_LCD_DisplayStringAtLine(2, "          SOUTHWEST          ");
			BT_Transmit(&bt, 'z');
		} else if (adj_x < -50 && adj_y > 50) { // NORTHWEST
			BSP_LCD_DisplayStringAtLine(2, "          NORTHWEST          ");
			BT_Transmit(&bt, 'q');
		} else if (adj_y > 50) { // NORTH
			BSP_LCD_DisplayStringAtLine(2, "            NORTH            ");
			BT_Transmit(&bt, 'w');
		} else if (adj_x > 50) { // EAST
			BSP_LCD_DisplayStringAtLine(2, "            EAST             ");
			BT_Transmit(&bt, 'd');
		} else if (adj_y < -50) { // SOUTH
			BSP_LCD_DisplayStringAtLine(2, "            SOUTH            ");
			BT_Transmit(&bt, 's');
		} else if (adj_x < -50) { // WEST
			BSP_LCD_DisplayStringAtLine(2, "            WEST             ");
			BT_Transmit(&bt, 'a');
		} else {
			BSP_LCD_DisplayStringAtLine(2, "            ERROR            ");
		}
	}
}


void LCD_Init(void)
{
  /* LCD Initialization */
  /* Two layers are used in this application but not simultaneously
     so "LCD_MAX_PCLK" is recommended to programme the maximum PCLK = 25,16 MHz */
  BSP_LCD_Init();

  /* LCD Initialization */
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS+(BSP_LCD_GetXSize()*BSP_LCD_GetYSize()*4));

  /* Enable the LCD */
  BSP_LCD_DisplayOn();

  /* Select the LCD Background Layer  */
  BSP_LCD_SelectLayer(0);

  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Select the LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);

  /* Clear the Foreground Layer */
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Configure the transparency for foreground and background :
     Increase the transparency */
  BSP_LCD_SetTransparency(0, 0);
  BSP_LCD_SetTransparency(1, 100);

  printf("Setting Font\r\n");
  BSP_LCD_SetFont(&Font32);
}

int8_t _adj_reading(uint32_t raw, uint32_t neu, uint32_t min, uint32_t max) {
	int32_t tmp = raw - neu;
	if (abs(tmp) < 20) {
		return 0;
	} else if (tmp > 0) {
		return 100.0 * tmp / (max - neu);
	} else {
		return 100.0 * tmp / (neu - min);
	}
}

void Adjust_Joystick_Readings() {
	adj_x = _adj_reading(raw_x, neu_x, min_x, max_x);
	adj_y = -1 * _adj_reading(raw_y, neu_y, min_y, max_y);
}

void Calibrate_Joystick(uint32_t round) {

	printf("Start Joystick Calibration\r\n");
	uint32_t sum_x, sum_y, count;

	printf("Calibrating Neutral Position");
	sum_x = sum_y = count = 0;
	while (1) {
		if (count >= round) break;
		if (count % (round/10) == 0) {
			printf(".");
			fflush(stdout);
		}
		sum_x += raw_x;
		sum_y += raw_y;
		count++;
		HAL_Delay(10);
	}
	neu_x = sum_x / count;
	neu_y = sum_y / count;
	printf("Complete!\r\nneu_x = %ld\tneu_y = %ld\r\n", neu_x, neu_y);
	printf("** Press Enter to Calibrate MAX Positions **\r\n");
	getchar();

	printf("Calibrating MAX Position");
	sum_x = sum_y = count = 0;
	while (1) {
		if (count >= round) break;
		if (count % (round/10) == 0) {
			printf(".");
			fflush(stdout);
		}
		sum_x += raw_x;
		sum_y += raw_y;
		count++;
		HAL_Delay(10);
	}
	max_x = sum_x / count;
	max_y = sum_y / count;
	printf("Complete!\r\nmax_x = %ld\tmax_y = %ld\r\n", max_x, max_y);
	printf("** Press Enter to Calibrate MIN Positions **\r\n");
	getchar();

	printf("Calibrating MIN Position");
	sum_x = sum_y = count = 0;
	while (1) {
		if (count >= round) break;
		if (count % (round/10) == 0) {
			printf(".");
			fflush(stdout);
		}
		sum_x += raw_x;
		sum_y += raw_y;
		count++;
		HAL_Delay(10);
	}
	min_x = sum_x / count;
	min_y = sum_y / count;
	printf("Complete!\r\nmin_x = %ld\tmin_y = %ld\r\n", min_x, min_y);
}

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	if (huart->Instance == USART6)
	{
		char in = uart_getchar_it(&bt, 0);
		switch (in) {
		default:
			break;
		}
	}
}

void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}



void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&bt); }

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	;
}

//------------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------------

void DMA2_Stream4_IRQHandler() { HAL_DMA_IRQHandler(&hdmaadc1); }

void DMA2_Stream1_IRQHandler() { HAL_DMA_IRQHandler(&hdmaadc3); }

void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn, DMA_HandleTypeDef* hdma)
{

	if (Tgt == ADC1) {
		__ADC1_CLK_ENABLE();
		__DMA2_CLK_ENABLE();
		hdma->Instance 			= DMA2_Stream4;
		hdma->Init.Channel 		= DMA_CHANNEL_0;
	}
	else if (Tgt == ADC3) {
		__ADC3_CLK_ENABLE();
		__DMA2_CLK_ENABLE();
		hdma->Instance 			= DMA2_Stream1;
		hdma->Init.Channel 		= DMA_CHANNEL_2;
	}
	hdma->Init.PeriphInc 		= DMA_PINC_ENABLE;
	hdma->Init.Direction 		= DMA_PERIPH_TO_MEMORY;
	hdma->Init.Mode 			= DMA_CIRCULAR; // Keep writing new ADC reading to same memloc
	hdma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD; // 32 bit
	hdma->Init.MemInc			= DMA_MINC_ENABLE;
	hdma->Init.FIFOMode 		= DMA_FIFOMODE_DISABLE; // Using DIRECT Mode
	HAL_DMA_Init(hdma);
	__HAL_LINKDMA(hadc, DMA_Handle, *hdma);


	hadc->Instance 					= Tgt;
	hadc->Init.ClockPrescaler 		= ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.DataAlign 			= ADC_DATAALIGN_RIGHT;
	hadc->Init.Resolution 			= ADC_RESOLUTION_12B;
	hadc->Init.ExternalTrigConv 	= ADC_EXTERNALTRIGCONV_T2_TRGO;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc->Init.ContinuousConvMode 	= DISABLE; // Trigger By Timer
	hadc->Init.DiscontinuousConvMode= DISABLE;
	hadc->Init.ScanConvMode 		= DISABLE;
	hadc->Init.NbrOfConversion		= 1;
	hadc->Init.DMAContinuousRequests= ENABLE;
	hadc->Init.EOCSelection			= ADC_EOC_SINGLE_CONV;
	HAL_ADC_Init(hadc);

	ADC_ChannelConfTypeDef ADCConfig;
	ADCConfig.Channel 		= Chn;
	ADCConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	ADCConfig.Rank 		= ADC_REGULAR_RANK_1;
	HAL_ADC_ConfigChannel(hadc, &ADCConfig);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;
	GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;

	if (hadc->Instance == ADC1) {
		// GPIO
		__GPIOA_CLK_ENABLE();
		GPIO_InitStruct.Pin 	= GPIO_PIN_6;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	} else if (hadc->Instance == ADC3) {
		// GPIO
		__GPIOF_CLK_ENABLE();
		GPIO_InitStruct.Pin 	= GPIO_PIN_10;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	}
}

//------------------------------------------------------------------------------------
// Timer 2 Initialization
//------------------------------------------------------------------------------------
void TIM2_Init(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt) {
	__TIM2_CLK_ENABLE();
	htim->Instance = Tgt;
	htim->Init.Prescaler = (uint32_t) 10800; // 108MHz
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = (uint32_t) 100;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(htim);

	TIM_ClockConfigTypeDef sClockSourceConfig;
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig);
	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
	HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig);

}
