/*
            Bluetooth Stack
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  All rights reserved.
*/

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>
		
		#include "BluetoothCommon.h"
		#include "BluetoothACL.h"
		#include "BluetoothHCI.h"

	/* Function Prototypes: */
		void Bluetooth_Init(Bluetooth_Device_t* const StackState);
		bool Bluetooth_ManageConnections(Bluetooth_Device_t* const StackState);
		void Bluetooth_ProcessPacket(Bluetooth_Device_t* const StackState, const uint8_t Type, uint8_t* Data);

#endif