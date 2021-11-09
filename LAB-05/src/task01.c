/*
 * 8-bit DMA 	- CH1 Stream 0
 * 16-bit DMA 	- CH3 Stream 1
 * 32-bit DMA	- CH2 Stream 2
 *
 * ERRORS:
 * 	- Having to keep printing within callback
 * 	- Adding Abort to end of callback
 * 	- infinite while loop after HAL_DMA_START so that program doesn't terminate
 * 	- Weird printing from callback
 *
 */

#include "init.h" // Always need init.h, otherwise nothing will work.
#include<stdint.h>
#include<stdlib.h>

/* Arrays with pre-filled Data */
uint8_t 	 u8_10[10],  u8_100[100],  u8_1000[1000];
uint16_t	u16_10[10], u16_100[100], u16_1000[1000];
uint32_t	u32_10[10], u32_100[100], u32_1000[1000];

/* Destination arrays for data transfer */
uint8_t			a1[10], 	 a2[100],  		a3[1000];
uint16_t	 	b1[10],		 b2[100],		b3[1000];
uint32_t		c1[10],		 c2[100], 		c3[1000];

uint32_t cycles;

DMA_HandleTypeDef hdma2_8b, hdma2_16b, hdma2_32b;

int err_handler( void );
void HAL_DMA2_XferCpltCallback( DMA_HandleTypeDef * hdma );

void Terminal_Init() {
    printf("\033[0m\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void Data_Init( void )
{
	for (uint8_t i = 0; i < 10; i++) {
		 u8_10[i] = ( uint8_t) i;
		u16_10[i] = (uint16_t) i;
		u32_10[i] = (uint32_t) i;
	}
	for (uint8_t j = 0; j < 100; j++) {
		 u8_100[j] = ( uint8_t) j;
		u16_100[j] = (uint16_t) j;
		u32_100[j] = (uint32_t) j;
	}
	for (uint32_t k = 0; k < 1000; k++) {
		 u8_1000[k] = ( uint8_t) k/4;
		u16_1000[k] = (uint16_t) k/2;
		u32_1000[k] = (uint32_t) k;
	}
}

void DMA_Init( void )
{
	DMA_InitTypeDef initDMA;

	__DMA2_CLK_ENABLE();

	initDMA.Channel 			= DMA_CHANNEL_1;
	initDMA.Direction			= DMA_MEMORY_TO_MEMORY;
	initDMA.PeriphInc 			= DMA_PINC_DISABLE;
	initDMA.MemInc				= DMA_MINC_ENABLE;
	initDMA.Mode				= DMA_NORMAL;
	initDMA.MemDataAlignment	= DMA_MDATAALIGN_BYTE;
	initDMA.FIFOMode			= DMA_FIFOMODE_DISABLE;
	initDMA.Priority			= DMA_PRIORITY_VERY_HIGH;

	hdma2_8b.Instance 			= DMA2_Stream0;
	hdma2_8b.Init 				= initDMA;
	hdma2_8b.XferCpltCallback	= &HAL_DMA2_XferCpltCallback;

	if (HAL_DMA_Init(&hdma2_8b) != HAL_OK)
		err_handler();

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ	(DMA2_Stream0_IRQn);

}

void DMA2_Stream0_IRQHandler( void )
{
	HAL_DMA_IRQHandler(&hdma2_8b);
}

void HAL_DMA2_XferCpltCallback( DMA_HandleTypeDef * hdma )
{
	cycles = DWT->CYCCNT;
	printf("DMA Implementation: %ld cycles\r\n", cycles);

	cycles = 0;
	HAL_DMA_Abort_IT(hdma);
}

void test8bit( void )
{
	uint8_t sw_buf1[10], sw_buf2[100], sw_buf3[1000];

	printf("\033[4;1H*********** Unsigned 8-bit Integers (uint8_t) ***********\r\n");

	printf("Moving arrays of Size 10 -\r\n");

	DWT->CYCCNT = 0;
	memcpy(sw_buf1, u8_10, 10*sizeof(uint8_t));
	cycles = DWT->CYCCNT;
	printf("\tSofware Implementation: %ld cycles\r\n", cycles);

	cycles = 0;
	HAL_Delay(1000);

	DWT->CYCCNT = 0;
	HAL_DMA_Start_IT(&hdma2_8b, u8_10, a1, 10 * sizeof(uint8_t));

	while (DMA_NOT_DONE);

	printf("Moving arrays of Size 100 -\r\n");

	DWT->CYCCNT = 0;
	memcpy(sw_buf2, u8_100, 100*sizeof(uint8_t));
	cycles = DWT->CYCCNT;
	printf("\tSofware Implementation: %ld cycles\r\n", cycles);

	cycles = 0;
	HAL_Delay(1000);

	DWT->CYCCNT = 0;
	HAL_DMA_Start_IT(&hdma2_8b, u8_100, a2, 100 * sizeof(uint8_t));
	while (DMA_NOT_DONE);

	printf("\tDMA Implementation: %ld cycles\r\n", cycles);

	cycles = 0;
}

int main(void)
{
	Sys_Init();
	Terminal_Init();
	Data_Init();
	DMA_Init();

	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("\033[0m\033[44;33m\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);
	cycles = DWT->CYCCNT; // Store the cycle counter

	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("Clearing and Moving the Cursor to Home took %lu CPU cycles\r\n",cycles);
	fflush(stdout);
	cycles = DWT->CYCCNT; // Store the cycle counter

	printf("And printing the above took %lu CPU cycles\r\n",cycles);
	fflush(stdout);

	test8bit();

//	DWT->CYCCNT = 0;
//	HAL_DMA_Start_IT(&hdma2_16b, u16_10[0], b1[0], 10*sizeof(uint16_t));
//
//	DWT->CYCCNT = 0;
//	HAL_DMA_Start_IT(&hdma2_32b, u32_10[0], c1[0], 10*sizeof(uint32_t));
	while (1);
}

int err_handler( void )
{
	return EXIT_FAILURE;
}
