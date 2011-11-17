/*
            Bluetooth Stack
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.com
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  All rights reserved.
*/

/** \file
 *
 *  Header file for Bluetooth.c.
 */

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>
		
		#include "BluetoothCommon.h"
		#include "BluetoothHCI.h"
		#include "BluetoothL2CAP.h"

	/* Function Prototypes: */
		void Bluetooth_Init(BT_StackConfig_t* const StackState);
		bool Bluetooth_ManageConnections(BT_StackConfig_t* const StackState);
		void Bluetooth_ProcessPacket(BT_StackConfig_t* const StackState,
		                             const uint8_t Type);
		void Bluetooth_TickElapsed(BT_StackConfig_t* const StackState);

#endif