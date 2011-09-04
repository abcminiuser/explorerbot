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

#ifndef _BLUETOOTH_HCI_H_
#define _BLUETOOTH_HCI_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>
		
		#include "BluetoothCommon.h"
		#include "BluetoothClassCodes.h"
		#include "BluetoothHCIEventCodes.h"
		#include "BluetoothHCIPacketTypes.h"
		#include "BluetoothL2CAP.h"
	
	/* Macros: */
		#define BT_HCI_CONNECTION_HANDLE_MASK     0x0FFF	
	
	/* Enums: */
		/** Enum for the possible states in the Bluetooth HCI state machine. */
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
		BT_HCI_Connection_t* const Bluetooth_HCI_FindConnection(BT_StackConfig_t* const StackState,
		                                                        const uint8_t* const BDADDR,
		                                                        const uint16_t Handle);

		void Bluetooth_HCI_Init(BT_StackConfig_t* const StackState);
		void Bluetooth_HCI_ProcessPacket(BT_StackConfig_t* const StackState);
		bool Bluetooth_HCI_Manage(BT_StackConfig_t* const StackState);

#endif