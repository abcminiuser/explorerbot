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
 *  Header file for BluetoothL2CAP.c.
 */

#ifndef _BLUETOOTH_L2CAP_H_
#define _BLUETOOTH_L2CAP_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>
		
		#include "BluetoothCommon.h"
		#include "BluetoothL2CAPPacketTypes.h"
		#include "BluetoothL2CAPSignalCodes.h"
		#include "BluetoothHCI.h"

	/* Macros: */
		/** Lowest possible channel number for L2CAP data channels. */
		#define BT_CHANNEL_BASEOFFSET             0x0040

		/** Bluetooth specification defined channel number for signaling commands. */
		#define BT_CHANNEL_SIGNALING              0x0001

		/** Bluetooth specification defined channel number for connectionless data. */
		#define BT_CHANNEL_CONNECTIONLESS         0x0002
		
	/* Enums: */
		/** Enum for the possible states for a Bluetooth L2CAP channel. */
		enum BT_L2CAP_ChannelStates_t
		{
			L2CAP_CHANSTATE_Closed                = 0, /**< Channel is closed and inactive. No data may be sent or received. */
			L2CAP_CHANSTATE_New                   = 1, /**< Channel has been created, but no initial handshake has been sent or received. */
			L2CAP_CHANSTATE_WaitConnect           = 2, /**< A connection request has been received, but a response has not been sent. */
			L2CAP_CHANSTATE_WaitConnectRsp        = 3, /**< A connection request has been sent, but a response has not been received. */
			L2CAP_CHANSTATE_Config_WaitConfig     = 4, /**< Channel has been connected, but not yet configured on either end. */
			L2CAP_CHANSTATE_Config_WaitSendConfig = 5, /**< Channel configuration has been received and accepted, but not yet sent. */
			L2CAP_CHANSTATE_Config_WaitReqResp    = 6, /**< Channel configuration has been sent but not responded to, and a configuration
			                                            *   request from the remote end has not yet been received.
			                                            */
			L2CAP_CHANSTATE_Config_WaitResp       = 7, /**< Channel configuration has been sent but not accepted, but a configuration request
			                                            *   from the remote end has been accepted.
			                                            */
			L2CAP_CHANSTATE_Config_WaitReq        = 8, /**< Channel configuration has been sent and accepted, but a configuration request
			                                            *   from the remote end has not yet been accepted.
			                                            */
			L2CAP_CHANSTATE_Open                  = 9, /**< Channel is open and ready to send or receive data */
			L2CAP_CHANSTATE_WaitDisconnectRsp     = 10, /**< A disconnection request has been sent, but not yet acknowledged. */
		};
		
	/* Function Prototypes: */
		void Bluetooth_L2CAP_NotifyHCIDisconnection(BT_StackConfig_t* const StackState,
		                                            const uint16_t ConnectionHandle);

		BT_L2CAP_Channel_t* const Bluetooth_L2CAP_FindChannel(BT_StackConfig_t* const StackState,
		                                                      const uint16_t ConnectionHandle,
		                                                      const uint16_t LocalChannel,
		                                                      const uint16_t RemoteChannel);

		void Bluetooth_L2CAP_Init(BT_StackConfig_t* const StackState);
		void Bluetooth_L2CAP_ProcessPacket(BT_StackConfig_t* const StackState,
		                                   BT_HCI_Connection_t* const HCIConnection,
		                                   const void* Data);
		bool Bluetooth_L2CAP_Manage(BT_StackConfig_t* const StackState);

		BT_L2CAP_Channel_t* const Bluetooth_L2CAP_OpenChannel(BT_StackConfig_t* const StackState,
                                                              BT_HCI_Connection_t* const HCIConnection,
                                                              const uint16_t PSM);
		void Bluetooth_L2CAP_CloseChannel(BT_StackConfig_t* const StackState,
		                                  BT_L2CAP_Channel_t* const L2CAPChannel);

		bool Bluetooth_L2CAP_SendPacket(BT_StackConfig_t* const StackState,
		                                BT_L2CAP_Channel_t* const L2CAPChannel,
		                                const uint16_t Length,
		                                const void* Data);

#endif