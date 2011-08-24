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

	/* Enums: */
		enum Bluetooth_PacketType_t
		{
			BLUETOOTH_PACKET_HCICommand = 0x01,
			BLUETOOTH_PACKET_ACLData    = 0x02,
			BLUETOOTH_PACKET_SynchData  = 0x03,
			BLUETOOTH_PACKET_HCIEvent   = 0x04,
		};

	/* External Variables: */
		extern void* Bluetooth_OutputPacketBuffer;

	/* User Implemented Callback Functions: */
		void CALLBACK_Bluetooth_SendPacket(const uint8_t Type, const uint16_t Length, uint8_t* Data);
		
#endif

