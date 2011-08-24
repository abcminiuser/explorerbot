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

#ifndef _BLUETOOTH_ACL_H_
#define _BLUETOOTH_ACL_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>
		
		#include "BluetoothCommon.h"
		#include "BluetoothACLPacketTypes.h"

	/* Function Prototypes: */
		void Bluetooth_ACL_Init(Bluetooth_Device_t* const StackState);
		void Bluetooth_ACL_ProcessPacket(Bluetooth_Device_t* const StackState, uint8_t* Data);
		bool Bluetooth_ACL_Manage(Bluetooth_Device_t* const StackState);

#endif