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

#ifndef _BLUETOOTH_COMMON_H_
#define _BLUETOOTH_COMMON_H_

	/* Includes: */
		#include <LUFA/Common/Common.h>

	/* Defines: */
		#define BT_BDADDR_LEN             6
		#define MAX_DEVICE_CONNECTIONS    1

	/* Enums: */
		enum Bluetooth_HCI_ConnectionStates_t
		{
			HCI_CONSTATE_Free          = 0,
			HCI_CONSTATE_New           = 1,
			HCI_CONSTATE_Connected     = 2,
		};

		enum Bluetooth_PacketType_t
		{
			BLUETOOTH_PACKET_HCICommand = 0x01,
			BLUETOOTH_PACKET_ACLData    = 0x02,
			BLUETOOTH_PACKET_SynchData  = 0x03,
			BLUETOOTH_PACKET_HCIEvent   = 0x04,
		};

	/* Type Defines: */
		typedef struct
		{
			uint8_t  State;
			uint8_t  BDADDR[BT_BDADDR_LEN];
			uint16_t Handle;
			uint8_t  LinkType;
		} BT_HCI_Connection_t;
	
		typedef struct
		{
			struct
			{
				uint32_t Class; /**< Class of the local device, a mask of DEVICE_CLASS_* masks. */
				char     PINCode[16]; /**< Pin code required to send or receive in order to authenticate with a remote device. */
				char*    Name; /**< Name of the local Bluetooth device, up to 248 characters. */
				bool     Discoverable;
				void*    OutputPacketBuffer;
			} Config;
			
			struct
			{
				struct
				{
					uint8_t             State;
					bool                StateTransition;
					uint8_t             LocalBDADDR[BT_BDADDR_LEN];
					BT_HCI_Connection_t Connections[MAX_DEVICE_CONNECTIONS];
				} HCI;
				
				struct
				{
				
				} ACL;
			} State;
		} Bluetooth_Device_t;

	/* External Variables: */
		extern Bluetooth_Device_t* Bluetooth_StackState;

	/* User Implemented Callback Functions: */
		void CALLBACK_Bluetooth_SendPacket(Bluetooth_Device_t* const StackState, const uint8_t Type, const uint16_t Length, uint8_t* Data);
		bool CALLBACK_Bluetooth_ConnectionRequest(Bluetooth_Device_t* const StackState, BT_HCI_Connection_t* const ConnectionHandle);
	
	/* User Implemented Callback Event Functions: */
		void EVENT_Bluetooth_ConnectionComplete(Bluetooth_Device_t* const StackState, BT_HCI_Connection_t* const ConnectionHandle);
		void EVENT_Bluetooth_DisconnectionComplete(Bluetooth_Device_t* const StackState, BT_HCI_Connection_t* const ConnectionHandle);
		
#endif

