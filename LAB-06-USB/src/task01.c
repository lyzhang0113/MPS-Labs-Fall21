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

void Term_Init();
void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);

uint8_t curr_color = 1;


int main(void){
	 // System Initializations
	Sys_Init();
	Term_Init();

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
		break;
	case HOST_USER_CLASS_ACTIVE:
//		Term_Init();
		// A device has been attached and enumerated and is ready to use
		break;
	case HOST_USER_CLASS_SELECTED:
		break;
	case HOST_USER_CONNECTION:
		break;
	case HOST_USER_DISCONNECTION:
		Term_Init();
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
