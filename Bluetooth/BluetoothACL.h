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
		void Bluetooth_ACL_Init(void);
		void Bluetooth_ACL_ProcessPacket(uint8_t* Data);
		bool Bluetooth_ACL_Manage(void);

#endif