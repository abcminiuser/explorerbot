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

void Bluetooth_Init(Bluetooth_Device_t* const StackState)
{
	memset(&StackState->State, 0x00, sizeof(StackState->State));
	
	Bluetooth_HCI_Init(StackState);
	Bluetooth_ACL_Init(StackState);
}

bool Bluetooth_ManageConnections(Bluetooth_Device_t* const StackState)
{
	return (Bluetooth_HCI_Manage(StackState) || Bluetooth_ACL_Manage(StackState));
}

void Bluetooth_ProcessPacket(Bluetooth_Device_t* const StackState, const uint8_t Type, uint8_t* Data)
{
	switch (Type)
	{
		case BLUETOOTH_PACKET_HCIEvent:
			Bluetooth_HCI_ProcessPacket(StackState, Data);
			return;
		case BLUETOOTH_PACKET_ACLData:
			Bluetooth_ACL_ProcessPacket(StackState, Data);
			return;
	}
}
