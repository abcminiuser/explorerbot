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
		#include "BluetoothACLSignalCodes.h"

	/* Macros: */
		/** Lowest possible channel number for L2CAP data channels. */
		#define BT_CHANNELNUMBER_BASEOFFSET       0x0040

		/** Bluetooth specification defined channel number for signaling commands. */
		#define BT_CHANNEL_SIGNALING              0x0001

		/** Bluetooth specification defined channel number for connectionless data. */
		#define BT_CHANNEL_CONNECTIONLESS         0x0002

	/* Function Prototypes: */
		void Bluetooth_ACL_Init(BT_StackConfig_t* const StackState);
		void Bluetooth_ACL_ProcessPacket(BT_StackConfig_t* const StackState);
		bool Bluetooth_ACL_Manage(BT_StackConfig_t* const StackState);

#endif