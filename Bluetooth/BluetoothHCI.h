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

#ifndef _BLUETOOTH_HCI_H_
#define _BLUETOOTH_HCI_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>
		
		#include "BluetoothCommon.h"
		#include "BluetoothClassCodes.h"
		#include "BluetoothHCIEventCodes.h"
		#include "BluetoothHCIPacketTypes.h"
	
	/* Enums: */
		enum BT_HCIStates_t
		{
			HCISTATE_Init_Reset             = 0,
			HCISTATE_Init_ReadBufferSize    = 1,
			HCISTATE_Init_GetBDADDR         = 2,
			HCISTATE_Init_SetLocalName      = 3,
			HCISTATE_Init_SetDeviceClass    = 4,
			HCISTATE_Init_SetScanEnable     = 5,
			HCISTATE_Idle                   = 6,
		};

	/* Function Prototypes: */
		void Bluetooth_HCI_Init(Bluetooth_Device_t* const StackState);
		void Bluetooth_HCI_ProcessPacket(Bluetooth_Device_t* const StackState, uint8_t* Data);
		bool Bluetooth_HCI_Manage(Bluetooth_Device_t* const StackState);

#endif