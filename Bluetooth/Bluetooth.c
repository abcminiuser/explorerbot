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

#include "Bluetooth.h"

void* Bluetooth_OutputPacketBuffer;

void Bluetooth_Init(void* const OutputPacketBuffer)
{
	Bluetooth_OutputPacketBuffer = OutputPacketBuffer;
	
	Bluetooth_HCI_Init();
	Bluetooth_ACL_Init();
}

bool Bluetooth_ManageConnections(void)
{
	return (Bluetooth_HCI_Manage() || Bluetooth_ACL_Manage());
}

void Bluetooth_ProcessPacket(const uint8_t Type, uint8_t* Data)
{
	switch (Type)
	{
		case BLUETOOTH_PACKET_HCIEvent:
			Bluetooth_HCI_ProcessPacket(Data);
			return;
		case BLUETOOTH_PACKET_ACLData:
			Bluetooth_ACL_ProcessPacket(Data);
			return;
	}
}
