//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

// ADC1_CHANNEL6 ---> PA6 ---> Arduino A0
// ADC3_CHANNEL8 ---> PF10 --> Arduino A3

//------------------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------------------
#define CALIBRATION 1

#define NEU_X 1179
#define NEU_Y 1157
#define MIN_X 6
#define MIN_Y 7
#define MAX_X 3749
#define MAX_Y 3878

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "../Libraries/BSP/STM32F769I-Discovery/stm32f769i_discovery_lcd.h"
#include "init.h"
#include "bluetooth.h"

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void Term_Init(void);
void LCD_Init(void);
void TIM2_Init(TIM_HandleTypeDef* htim, TIM_TypeDef* Tgt);
void ADC_Init(ADC_HandleTypeDef* hadc, ADC_TypeDef* Tgt, uint32_t Chn, DMA_HandleTypeDef* hdma);

void BT_Connect(UART_HandleTypeDef* hbt);
uint8_t construct_byte(uint8_t speed, uint8_t dir);
void Adjust_Joystick_Readings();
void Calibrate_Joystick(uint32_t round);

//------------------------------------------------------------------------------------
// HandleTypeDefs
//------------------------------------------------------------------------------------
UART_HandleTypeDef bt;
TIM_HandleTypeDef htim2;
ADC_HandleTypeDef hadc1, hadc3;
DMA_HandleTypeDef hdmaadc1, hdmaadc3;

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
uint8_t enabled = 0;
uint32_t raw_x, raw_y;
uint32_t neu_x, neu_y, max_x, min_x, max_y, min_y;
int8_t adj_x, adj_y;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void) {
	// Initialization
	Sys_Init();
	Term_Init();
	BT_Init(&bt);
	ADC_Init(&hadc1, ADC1, ADC_CHANNEL_6, &hdmaadc1);
	ADC_Init(&hadc3, ADC3, ADC_CHANNEL_8, &hdmaadc3);
	TIM2_Init(&htim2, TIM2);
	LCD_Init();
	BSP_LCD_DisplayStringAtLine(0, "System Initialized");

	// Start Necessary Components
	HAL_TIM_Base_Start(&htim2);
	HAL_ADC_Start_DMA(&hadc1, &raw_y, sizeof(uint32_t));
	HAL_ADC_Start_DMA(&hadc3, &raw_x, sizeof(uint32_t));
	BSP_LCD_DisplayStringAtLine(1, "Start Joystick DMA Reading");

	if (CALIBRATION) {
		Calibrate_Joystick(300);
	} else {
		neu_x = NEU_X; neu_y = NEU_Y;
		min_x = MIN_X; min_y = MIN_Y;
		max_x = MAX_X; max_y = MAX_Y;
	}
	BSP_LCD_DisplayStringAtLine(2, "Joystick Calibrated");
	BSP_LCD_DisplayStringAtLine(6, "     --- Move to Any Direction to Start ---");

	while (1) {
		HAL_Delay(SAMPLING_FREQ);
		Adjust_Joystick_Readings();

		if (!enabled) {
			if (abs(adj_x) > 50 || abs(adj_y) > 50) {
				enabled = 1;
				BSP_LCD_Clear(LCD_COLOR_WHITE);
			}
			continue;
		}

		char jotstick_reading_buf[60];
		snprintf(jotstick_reading_buf, 60, "             x: %4d          y: %4d     ", adj_x, adj_y);
		BSP_LCD_DisplayStringAtLine(0, (uint8_t*)jotstick_reading_buf);

		uint8_t speed = 0x00;
		uint8_t dir = 0x00;

		if (abs(adj_x) < 20 && abs(adj_y) < 10) { // STOP
			BSP_LCD_ClearStringLine(2);
		} else if (adj_x > 20 && adj_y > 10) { // NORTHEAST -> 001
			speed = (abs(adj_x) + abs(adj_y)) / 12;
			dir = 0b001;
			BSP_LCD_DisplayStringAtLine(2, "          NORTHEAST          ");
		} else if (adj_x > 20 && adj_y < -10) { // SOUTHEAST -> 011
			speed = (abs(adj_x) + abs(adj_y)) / 12;
			dir = 0b011;
			BSP_LCD_DisplayStringAtLine(2, "          SOUTHEAST          ");
		} else if (adj_x < -20 && adj_y < -10) { // SOUTHWEST -> 101
			speed = (abs(adj_x) + abs(adj_y)) / 12;
			dir = 0b101;
			BSP_LCD_DisplayStringAtLine(2, "          SOUTHWEST          ");
		} else if (adj_x < -20 && adj_y > 10) { // NORTHWEST -> 111
			speed = (abs(adj_x) + abs(adj_y)) / 12;
			dir = 0b111;
			BSP_LCD_DisplayStringAtLine(2, "          NORTHWEST          ");
		} else if (adj_y > 10) { // NORTH -> 000
			speed = (abs(adj_y)) / 6;
			dir = 0b000;
			BSP_LCD_DisplayStringAtLine(2, "            NORTH            ");
		} else if (adj_x > 20) { // EAST -> 010
			speed = (abs(adj_x)) / 6;
			dir = 0b010;
			BSP_LCD_DisplayStringAtLine(2, "            EAST             ");
		} else if (adj_y < -10) { // SOUTH -> 100
			speed = (abs(adj_y)) / 6;
			dir = 0b100;
			BSP_LCD_DisplayStringAtLine(2, "            SOUTH            ");
		} else if (adj_x < -20) { // WEST -> 110
			speed = (abs(adj_x)) / 6;
			dir = 0b110;
			BSP_LCD_DisplayStringAtLine(2, "            WEST             ");
		} else {
			continue;
		}
		char buf[60];
		snprintf(buf, 60, "           Speed: %d          ", speed);
		BSP_LCD_DisplayStringAtLine(3, (uint8_t*)buf);
		BT_Transmit(&bt, construct_byte(speed, dir));
	}
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

uint8_t construct_byte(uint8_t speed, uint8_t dir) {
	if (speed > 0b1111) speed = 0b1111;
	if (dir > 0b111) return 0x00;
	uint8_t res = (speed << 3) | dir;
	res = add_parity(res);
	printf("%d\r\n", res);
	return res;
}

//------------------------------------------------------------------------------------
// Joystick
//------------------------------------------------------------------------------------

void Adjust_Joystick_Readings() {
	adj_x = _adj_reading(raw_x, neu_x, min_x, max_x);
	adj_y = -1 * _adj_reading(raw_y, neu_y, min_y, max_y);
}

void Calibrate_Joystick(uint32_t round) {
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAtLine(0, "Start Joystick Calibration");
	uint32_t sum_x, sum_y, count;

	BSP_LCD_DisplayStringAtLine(1, "Calibrating Neutral Position");
	sum_x = sum_y = count = 0;
	while (1) {
		if (count >= round) break;
		if (count % (round/10) == 0) {
			BSP_LCD_DisplayChar(COLUMN(count/(round/10)), LINE(2), '.');
		}
		sum_x += raw_x;
		sum_y += raw_y;
		count++;
		HAL_Delay(10);
	}
	neu_x = sum_x / count;
	neu_y = sum_y / count;
	char buf[60];
	snprintf(buf, 60, "Complete! neu_x = %5ldneu_y = %5ld", neu_x, neu_y);
	BSP_LCD_DisplayStringAtLine(3, (uint8_t*)buf);
	BSP_LCD_DisplayStringAtLine(4, "** Move Joystick to Calibrate MAX Positions **");
	while (abs(raw_x - neu_x) < 80 && abs(raw_y - neu_y) < 80) HAL_Delay(10);

	BSP_LCD_ClearStringLine(1);
	BSP_LCD_ClearStringLine(2);
	BSP_LCD_ClearStringLine(3);
	BSP_LCD_ClearStringLine(4);
	BSP_LCD_DisplayStringAtLine(1, "Calibrating MAX Position");
	sum_x = sum_y = count = 0;
	while (1) {
		if (count >= round) break;
		if (count % (round/10) == 0) {
			BSP_LCD_DisplayChar(COLUMN(count/(round/10)), LINE(2), '.');
		}
		sum_x += raw_x;
		sum_y += raw_y;
		count++;
		HAL_Delay(10);
	}
	max_x = sum_x / count;
	max_y = sum_y / count;
	snprintf(buf, 60, "Complete! max_x = %5ldmax_y = %5ld", max_x, max_y);
	BSP_LCD_DisplayStringAtLine(3, (uint8_t*)buf);
	while (abs(raw_x - neu_x) > 30 || abs(raw_y - neu_y) > 30) HAL_Delay(10);
	BSP_LCD_DisplayStringAtLine(4, "** Move Joystick to Calibrate MIN Positions **");
	while (abs(raw_x - neu_x) < 80 && abs(raw_y - neu_y) < 80) HAL_Delay(10);

	BSP_LCD_ClearStringLine(1);
	BSP_LCD_ClearStringLine(2);
	BSP_LCD_ClearStringLine(3);
	BSP_LCD_ClearStringLine(4);
	BSP_LCD_DisplayStringAtLine(1, "Calibrating MIN Position");
	sum_x = sum_y = count = 0;
	while (1) {
		if (count >= round) break;
		if (count % (round/10) == 0) {
			BSP_LCD_DisplayChar(COLUMN(count/(round/10)), LINE(2), '.');
		}
		sum_x += raw_x;
		sum_y += raw_y;
		count++;
		HAL_Delay(10);
	}
	min_x = sum_x / count;
	min_y = sum_y / count;
	snprintf(buf, 60, "Complete! min_x = %5ldmin_y = %5ld", min_x, min_y);
	BSP_LCD_DisplayStringAtLine(3, (uint8_t*)buf);
	while (abs(raw_x - neu_x) > 30 || abs(raw_y - neu_y) > 30) HAL_Delay(10);
	BSP_LCD_DisplayStringAtLine(4, "** Move Joystick to Complete Calibration **");
	while (abs(raw_x - neu_x) < 80 && abs(raw_y - neu_y) < 80) HAL_Delay(10);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
}

//------------------------------------------------------------------------------------
// LCD and Terminal Init
//------------------------------------------------------------------------------------

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

void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

//------------------------------------------------------------------------------------
// UART6 Bluetooth
//------------------------------------------------------------------------------------

void USART6_IRQHandler( void ) { HAL_UART_IRQHandler(&bt); }

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	if (huart->Instance == USART6)
	{
		char in = uart_getchar_it(huart, 0);
		switch (in) {
		case 'p':
			break;
		default:
			break;
		}
	}
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
