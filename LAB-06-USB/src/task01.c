//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

#include <stdio.h>

#include "init.h"
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

USBH_HandleTypeDef husbh;
HID_MOUSE_Info_TypeDef* mouse_info;

uint8_t poll_interval;
uint8_t CONNECTED = 0;

void Term_Init();
void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);


int main(void){
	 // System Initializations
	Sys_Init();
	Term_Init();

	// Application Initializations
	// USBH Driver Initialization
	USBH_Init(&husbh, USBH_UserProcess, 0);
	// USB Driver Class Registrations: Add device types to handle.
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);
	// Start USBH Driver
	USBH_Start(&husbh);
	while(1){
		USBH_Process(&husbh);
	}
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	switch (id) {
	case HOST_USER_SELECT_CONFIGURATION:
		printf("HOST_USER_SELECT_CONFIGURATION\r\n");
		break;
	case HOST_USER_CLASS_ACTIVE:
		printf("HOST_USER_CLASS_ACTIVE\r\n");
		// A device has been attached and enumerated and is ready to use
		CONNECTED = 1;
		// Init Mouse
//		if (USBH_HID_MouseInit(phost) != USBH_OK) {
//			printf("ERROR: Mouse Initialization\r\n");
//		}
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

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
	mouse_info = USBH_HID_GetMouseInfo(&husbh);
	printf("\rx: %d; y: %d; %d %d %d", mouse_info->x, mouse_info->y, mouse_info->buttons[0], mouse_info->buttons[1], mouse_info->buttons[2]);
	fflush(stdout);

}

void Term_Init(void) {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}
// Interrupts and Callbacks...

