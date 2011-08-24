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
			HCISTATE_Init_Reset             = 2,
			HCISTATE_Init_ReadBufferSize    = 3,
			HCISTATE_Init_GetBDADDR         = 4,
			HCISTATE_Init_SetLocalName      = 5,
			HCISTATE_Init_SetDeviceClass    = 6,
			HCISTATE_Init_WriteScanEnable   = 7,
			HCISTATE_Init_FinalizeInit      = 8,

			HCISTATE_Conn_AcceptConnection  = 9,
			HCISTATE_Conn_RejectConnection  = 10,
			HCISTATE_Conn_SendPINCode       = 11,
			HCISTATE_Conn_SendLinkKeyNAK    = 12,
		};

	/* Function Prototypes: */
		void Bluetooth_HCI_Init(void);
		void Bluetooth_HCI_ProcessPacket(uint8_t* Data);
		bool Bluetooth_HCI_Manage(void);

#endif