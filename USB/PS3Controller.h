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

#ifndef _PS3_CONTROLLER_H_
#define _PS3_CONTROLLER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <stdlib.h>

		#include <LUFA/Drivers/USB/USB.h>

		#include "BluetoothAdapter.h"
		#include "../Bluetooth/Bluetooth.h"

	/* Macros: */
		/** Device Vendor ID value for the PS3 Controller. */
		#define PS3CONTROLLER_VID           0x054C

		/** Device Product ID value for the PS3 Controller. */
		#define PS3CONTROLLER_PID           0x0268

	/* Function Prototypes: */
		static inline void PS3Controller_PairLastAdapter(void)
		{
			/* Copy over the address of the last inserted USB Bluetooth adapter */
			uint8_t PS3AdapterPairRequest[2 + BT_BDADDR_LEN];
			
			PS3AdapterPairRequest[0] = 0x01;
			PS3AdapterPairRequest[1] = 0x00;
			eeprom_read_block(&PS3AdapterPairRequest[2], BluetoothAdapter_LastLocalBDADDR, BT_BDADDR_LEN);
			
			USB_ControlRequest = (USB_Request_Header_t)
				{
					.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
					.bRequest      = HID_REQ_SetReport,
					.wValue        = 0x03F5,
					.wIndex        = 0,
					.wLength       = sizeof(PS3AdapterPairRequest)
				};

			/* Request that the controller re-pair with the last connected Bluetooth adapter BDADDR */
			Pipe_SelectPipe(PIPE_CONTROLPIPE);
			USB_Host_SendControlRequest(PS3AdapterPairRequest);	
		}

#endif

