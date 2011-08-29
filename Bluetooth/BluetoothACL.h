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
		#include "BluetoothHCI.h"

	/* Macros: */
		/** Lowest possible channel number for L2CAP data channels. */
		#define BT_CHANNEL_BASEOFFSET             0x0040

		/** Bluetooth specification defined channel number for signaling commands. */
		#define BT_CHANNEL_SIGNALING              0x0001

		/** Bluetooth specification defined channel number for connectionless data. */
		#define BT_CHANNEL_CONNECTIONLESS         0x0002
		
		/** HCI connection flag to automatically flush packets to the receiver. */
		#define BT_ACL_FIRST_AUTOFLUSH            (1 << 13)

	/* Function Prototypes: */
		void Bluetooth_ACL_NotifyHCIDisconnection(BT_StackConfig_t* const StackState,
		                                          const uint16_t ConnectionHandle);

		BT_ACL_Channel_t* Bluetooth_ACL_FindChannel(BT_StackConfig_t* const StackState,
		                                            const uint16_t ConnectionHandle,
		                                            const uint16_t LocalChannel,
		                                            const uint16_t RemoteChannel);
		void Bluetooth_ACL_Init(BT_StackConfig_t* const StackState);
		void Bluetooth_ACL_ProcessPacket(BT_StackConfig_t* const StackState);
		bool Bluetooth_ACL_Manage(BT_StackConfig_t* const StackState);

		bool Bluetooth_ACL_SendPacket(BT_StackConfig_t* const StackState,
		                              BT_ACL_Channel_t* const ACLChannel,
		                              uint16_t Length,
		                              void* Data);
		BT_ACL_Channel_t* Bluetooth_ACL_OpenChannel(BT_StackConfig_t* const StackState,
                                                    BT_HCI_Connection_t* const HCIConnection,
                                                    const uint16_t PSM);
		void Bluetooth_ACL_CloseChannel(BT_StackConfig_t* const StackState,
		                                BT_ACL_Channel_t* const ACLChannel);
#endif