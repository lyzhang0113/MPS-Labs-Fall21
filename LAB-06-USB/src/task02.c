//--------------------------------
// Lab 6 - Universal Serial Bus - task02.c
//--------------------------------

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <stdio.h>

#include "init.h"
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void Term_Init();
void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);
void print_line( const FILINFO * fin );

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------
USBH_HandleTypeDef husbh;
FATFS 	myFatFs;
DIR 	root;
FILINFO	fno;

uint8_t poll_interval;
uint8_t CONNECTED = 0;
uint8_t curr_color = 1;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void){
	 // System Initializations
	Sys_Init();
	Term_Init();

	// Application Initializations
	// USBH Driver Initialization
	USBH_Init(&husbh, USBH_UserProcess, 0);
	// USB Driver Class Registrations: Add device types to handle.
	USBH_RegisterClass(&husbh, USBH_MSC_CLASS);
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);

	// Start USBH Driver
	USBH_Start(&husbh);

	if ( FATFS_LinkDriver(&USBH_Driver, "0:") != 0 ) { printf("ERROR LINKING!\r\n"); }

	while(1){
		USBH_Process(&husbh);
		// Other stuff
	}
}

//------------------------------------------------------------------------------------
// Custom USB UserProcess
//------------------------------------------------------------------------------------
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	static FRESULT fr;

	switch (id) {
	case HOST_USER_SELECT_CONFIGURATION:
		printf("HOST_USER_SELECT_CONFIGURATION\r\n");
		break;
	case HOST_USER_CLASS_ACTIVE:
		printf("HOST_USER_CLASS_ACTIVE\r\n");
		// A device has been attached and enumerated and is ready to use
		CONNECTED = 1;

		fr = f_mount( &myFatFs, "0:", 1);
		if ( fr == FR_OK ) {
			fr = f_findfirst(&root, &fno, "0:", "*");
			if ( fr == FR_OK) {
				printf("USB CONTENTS:\r\n");
				while ( fr == FR_OK && fno.fname[0] ) {
					print_line(&fno);
					fr = f_findnext(&root, &fno);
				}

			}
		}

		break;
	case HOST_USER_CLASS_SELECTED:
		printf("HOST_USER_CLASS_SELECTED\r\n");
		break;
	case HOST_USER_CONNECTION:
		printf("HOST_USER_CONNECTION\r\n");
		break;
	case HOST_USER_DISCONNECTION:
		Term_Init();
		printf("HOST_USER_DISCONNECTION\r\n");
		CONNECTED = 0;
		break;
	case HOST_USER_UNRECOVERED_ERROR:
		printf("HOST_USER_UNRECOVERED_ERROR\r\n");
		break;
	}
}

//------------------------------------------------------------------------------------
// Misc. Helper Functions
//------------------------------------------------------------------------------------
void Term_Init(void)
{
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

void print_line( const FILINFO * fin )
{
	WORD date = fin->fdate;
	WORD time = fin->ftime;

	printf("%5.5s\t", (fin->fattrib == (BYTE)AM_DIR) ? "DIR":"FILE" ); fflush(stdout);
	printf("%2.2d %2.2d %4.4d\t", (date & 0x000F), ((date>>4) & 0x000F), ((date>>16) & 0x00FF + 1980) ); fflush(stdout);
	printf("%2.2d:%2.2d\t", (time>11), ((time>5) & 0x003F) ); fflush(stdout);
	printf("%s\r\n", fin->fname);
}

//------------------------------------------------------------------------------------
// USB HID Callback
//------------------------------------------------------------------------------------
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
	HID_MOUSE_Info_TypeDef* mouse_info;
	mouse_info = USBH_HID_GetMouseInfo(&husbh);
	int8_t x_raw = mouse_info->x;
	int8_t y_raw = mouse_info->y;
	uint8_t button_l = mouse_info->buttons[0];
	uint8_t button_r = mouse_info->buttons[1];
	uint8_t button_m = mouse_info->buttons[2];

	if (x_raw >= 0 && y_raw >= 0) {
		if (x_raw > y_raw) {
			printf("\033[C");
		} else {
			printf("\033[B");
		}
	} else {
		if (x_raw < y_raw) {
			printf("\033[D");
		} else {
			printf("\033[A");
		}
	}

	if (button_m) {
		curr_color = ++curr_color % 7;
	} else

	if (button_l) {
		printf("\033[%dm \033[D", curr_color + 41);
	} else

	if (button_r) {
		printf("\033[40m \033[D");
	}

//	printf("\033[;Hx: %d ; y: %d; %d %d %d       ", x_raw, y_raw, button_l, button_m, button_r);
	fflush(stdout);

}
