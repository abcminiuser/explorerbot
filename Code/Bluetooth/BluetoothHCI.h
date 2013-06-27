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
 *  Header file for BluetoothHCI.c.
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
		/** Mask for a HCI connection handle value to remove all HCI packet connection flags, leaving only the unique handle value. */
		#define BT_HCI_CONNECTION_HANDLE_MASK     0x0EFF

		/** Number of milliseconds before an issued HCI command times out and is re-tried. */
		#define BT_HCI_COMMAND_TIMEOUT_MS         1000

		/** HCI connection flag to automatically flush packets to the receiver. */
		#define BT_L2CAP_FIRST_AUTOFLUSH          (2 << 12)

		/** HCI connection flag to indicate a continuation of a higher layer packet. */
		#define BT_L2CAP_PACKET_CONTINUATION      (1 << 12)

	/* Enums: */
		/** Enum for the possible device connection states. */
		enum BT_HCI_ConnectionStates_t
		{
			HCI_CONSTATE_Closed                   = 0, /**< Connnection is closed and inactive. No data may be sent or received. */
			HCI_CONSTATE_New                      = 1, /**< Connection has been created, but no initial handshake has been sent or received. */
			HCI_CONSTATE_Connecting               = 2, /**< Connection is being established, but has not been completed. */
			HCI_CONSTATE_Connected                = 3, /**< Connection is complete and ready for data transfer. */
			HCI_CONSTATE_Failed                   = 4, /**< Connection to a remote device failed to complete. */
		};

		/** Enum for the possible states in the Bluetooth HCI state machine. */
		enum BT_HCIStates_t
		{
			HCISTATE_Init_Reset                   = 0, /**< The HCI layer is currently resetting the Bluetooth adapter. */
			HCISTATE_Init_ReadBufferSize          = 1, /**< The HCI layer is currently retrieving the buffer size of the Bluetooth adapter. */
			HCISTATE_Init_GetBDADDR               = 2, /**< The HCI layer is currently retrieving the local BDADDR the Bluetooth adapter. */
			HCISTATE_Init_SetLocalName            = 3, /**< The HCI layer is currently setting the local name of the Bluetooth adapter. */
			HCISTATE_Init_SetDeviceClass          = 4, /**< The HCI layer is currently setting the local device class of the Bluetooth adapter. */
			HCISTATE_Init_SetScanEnable           = 5, /**< The HCI layer is currently setting the scan mode of the Bluetooth adapter. */
			HCISTATE_Idle                         = 6, /**< The HCI layer is ready to accept and send data. */
		};

	/* Function Prototypes: */
		BT_HCI_Connection_t* const Bluetooth_HCI_FindConnection(BT_StackConfig_t* const StackState,
		                                                        const uint8_t* const BDADDR,
		                                                        const uint16_t Handle);

		void Bluetooth_HCI_Init(BT_StackConfig_t* const StackState);
		void Bluetooth_HCI_TickElapsed(BT_StackConfig_t* const StackState);
		void Bluetooth_HCI_ProcessEventPacket(BT_StackConfig_t* const StackState);
		void Bluetooth_HCI_ProcessDataPacket(BT_StackConfig_t* const StackState);
		bool Bluetooth_HCI_Manage(BT_StackConfig_t* const StackState);

		BT_HCI_Connection_t* Bluetooth_HCI_Connect(BT_StackConfig_t* const StackState,
		                                           const uint8_t* const RemoteBDADDR,
		                                           const uint8_t LinkType);
		bool Bluetooth_HCI_Disconnect(BT_StackConfig_t* const StackState,
		                              BT_HCI_Connection_t* const HCIConnection);

		bool Bluetooth_HCI_SendControlPacket(BT_StackConfig_t* const StackState,
                                             const uint16_t OpCode,
		                                     const uint8_t Length,
		                                     const void* Data);
		bool Bluetooth_HCI_SendPacket(BT_StackConfig_t* const StackState,
		                              BT_HCI_Connection_t* const HCIConnection,
		                              const bool PacketContinuation,
		                              const uint16_t Length,
		                              const void* Data);

#endif
