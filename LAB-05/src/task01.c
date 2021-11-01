#include "init.h" // Always need init.h, otherwise nothing will work.
#include<stdint.h>
#include<stdlib.h>

uint8_t 	 u8_10[10],  u8_100[100],  u8_1000[1000];
uint16_t	u16_10[10], u16_100[100], u16_1000[1000];
uint32_t	u32_10[10], u32_100[100], u32_1000[1000];

void Data_Init( void )
{
	for (uint8_t i = 0; i < 10; i++) {
		 u8_10[i] = ( uint8_t) i;
		u16_10[i] = (uint16_t) i;
		u32_10[i] = (uint32_t) i;
	}
	for (uint8_t j = 0; j < 100; j++) {
		 u8_100[i] = ( uint8_t) j;
		u16_100[i] = (uint16_t) j;
		u32_100[i] = (uint32_t) j;
	}
	for (uint8_t k = 0; k < 1000; k++) {
		 u8_1000[i] = ( uint8_t) i/4;
		u16_1000[i] = (uint16_t) i/2;
		u32_1000[i] = (uint32_t) i;
	}
}

int main(void)
{
	Sys_Init();

	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("\033[0m\033[44;33m\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);
	uint32_t cycles = DWT->CYCCNT; // Store the cycle counter

	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("Clearing and Moving the Cursor to Home took %lu CPU cycles\r\n",cycles);
	fflush(stdout);
	cycles = DWT->CYCCNT; // Store the cycle counter

	printf("And printing the above took %lu CPU cycles\r\n",cycles);
	fflush(stdout);

	while(1);
}
