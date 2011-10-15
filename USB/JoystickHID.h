/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.com
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for JoystickHID.c.
 */

#ifndef _JOYSTICK_CONTROL_H_
#define _JOYSTICK_CONTROL_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <stdlib.h>

		#include <LUFA/Drivers/USB/USB.h>
		
		#include "../Drivers/Headlights.h"
		#include "../Drivers/Motors.h"
		#include "../Drivers/RGB.h"
		#include "../Drivers/Speaker.h"
		
		#include "BluetoothAdapter.h"
		#include "../Bluetooth/Bluetooth.h"
		
	/* Macros: */
		/** HID Report Descriptor Usage Page value for a toggle button. */
		#define USAGE_PAGE_BUTTON           0x09

		/** HID Report Descriptor Usage Page value for a Generic Desktop Control. */
		#define USAGE_PAGE_GENERIC_DCTRL    0x01

		/** HID Report Descriptor Usage for a Joystick. */
		#define USAGE_JOYSTICK              0x04

		/** HID Report Descriptor Usage value for a X axis movement. */
		#define USAGE_X                     0x30

		/** HID Report Descriptor Usage value for a Y axis movement. */
		#define USAGE_Y                     0x31

		/** Device Vendor ID value for the PS3 Controller. */
		#define PS3CONTROLLER_VID           0x054C

		/** Device Product ID value for the PS3 Controller. */
		#define PS3CONTROLLER_PID           0x0268
	
	/* Enums: */
		enum PS3Controller_ButtonUsages_t
		{
			PS3CONTROLLER_BUTTON_Select   = 1,
			PS3CONTROLLER_BUTTON_LAnalog  = 2,
			PS3CONTROLLER_BUTTON_RAnalog  = 3,
			PS3CONTROLLER_BUTTON_Start    = 4,
			PS3CONTROLLER_BUTTON_Up       = 5,
			PS3CONTROLLER_BUTTON_Right    = 6,
			PS3CONTROLLER_BUTTON_Down     = 7,
			PS3CONTROLLER_BUTTON_Left     = 8,
			PS3CONTROLLER_BUTTON_L2       = 9,
			PS3CONTROLLER_BUTTON_R2       = 10,
			PS3CONTROLLER_BUTTON_L1       = 11,
			PS3CONTROLLER_BUTTON_R1       = 12,
			PS3CONTROLLER_BUTTON_Triangle = 13,
			PS3CONTROLLER_BUTTON_Circle   = 14,
			PS3CONTROLLER_BUTTON_Cross    = 15,
			PS3CONTROLLER_BUTTON_Square   = 16,
			PS3CONTROLLER_BUTTON_PS       = 17,
		};
		
	/* External Variables: */
		extern USB_ClassInfo_HID_Host_t Joystick_HID_Interface;

	/* Function Prototypes: */
		bool Joystick_ConfigurePipes(USB_Descriptor_Device_t* const DeviceDescriptor,
		                             uint16_t ConfigDescriptorSize,
		                             void* ConfigDescriptorData);
		bool Joystick_PostConfiguration(void);
		void Joystick_USBTask(void);

		bool CALLBACK_HIDParser_FilterHIDReportItem(HID_ReportItem_t* const CurrentItem);

#endif

