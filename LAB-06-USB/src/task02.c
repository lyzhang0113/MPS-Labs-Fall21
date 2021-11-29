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
	USBH_RegisterClass(&husbh, USBH_MSC_CLASS);
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);

	FATFS_LinkDriver(&USBH_Driver, "0:/");

	// Start USBH Driver
	USBH_Start(&husbh);
	while(1){
		USBH_Process(&husbh);
		// Other stuff
	}
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	switch (id) {
	case HOST_USER_SELECT_CONFIGURATION:
		break;
	case HOST_USER_CLASS_ACTIVE:
		// A device has been attached and enumerated and is ready to use
		CONNECTED = 1;
		break;
	case HOST_USER_CLASS_SELECTED:
		break;
	case HOST_USER_CONNECTION:
		break;
	case HOST_USER_DISCONNECTION:
		CONNECTED = 0;
		break;
	case HOST_USER_UNRECOVERED_ERROR:
		break;
	}
}

void Term_Init(void) {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}
// Interrupts and Callbacks...

