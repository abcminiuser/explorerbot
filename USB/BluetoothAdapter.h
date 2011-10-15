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
 *  Header file for BluetoothAdapter.c.
 */

#ifndef _BLUETOOTH_ADAPTER_H_
#define _BLUETOOTH_ADAPTER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/eeprom.h>
		#include <stdbool.h>
		#include <stdlib.h>

		#include <LUFA/Drivers/USB/USB.h>
		
		#include "../Drivers/ExternalSRAM.h"
		#include "../Drivers/RGB.h"
		
		#include "../Bluetooth/Bluetooth.h"

	/* Macros: */
		/** Device Class value for the Bluetooth Device class. */
		#define BLUETOOTH_DEVICE_CLASS           0xE0

		/** Device Subclass value for the Bluetooth Device class. */
		#define BLUETOOTH_DEVICE_SUBCLASS        0x01

		/** Device Protocol value for the Bluetooth Device class. */
		#define BLUETOOTH_DEVICE_PROTOCOL        0x01
		
		#define BLUETOOTH_DATA_IN_PIPE           1
		#define BLUETOOTH_DATA_OUT_PIPE          2
		#define BLUETOOTH_EVENTS_PIPE            3

	/* External Variables: */
		extern bool    BluetoothAdapter_IsActive;

		extern uint8_t BluetoothAdapter_LastLocalBDADDR[BT_BDADDR_LEN] EEMEM;
		extern uint8_t BluetoothAdapter_RemoteBDADDR[BT_BDADDR_LEN] EEMEM;

	/* Function Prototypes: */
		bool BluetoothAdapter_ConfigurePipes(USB_Descriptor_Device_t* const DeviceDescriptor,
		                                     uint16_t ConfigDescriptorSize,
		                                     void* ConfigDescriptorData);
		bool BluetoothAdapter_PostConfiguration(void);
		void BluetoothAdapter_USBTask(void);
		void BluetoothAdapter_TickElapsed(void);
		BT_HCI_Connection_t* BluetoothAdapter_ConnectToRemoteDevice(void);

		uint8_t DComp_NextInterfaceBluetoothDataEndpoint(void* CurrentDescriptor);

#endif

