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

#include "BluetoothHCI.h"

uint8_t Bluetooth_HCIState;
bool    Bluetooth_HCIStateTransition;

void Bluetooth_HCI_Init(void)
{
	Bluetooth_HCIState           = HCISTATE_Init_Reset;
	Bluetooth_HCIStateTransition = true;
}

void Bluetooth_HCI_ProcessPacket(uint8_t* Data)
{
	BT_HCIEvent_Header_t* HCIEventHeader = (BT_HCIEvent_Header_t*)Data;
	uint8_t               NextHCIState   = Bluetooth_HCIState;
	
	switch (Bluetooth_HCIState)
	{
		case HCISTATE_Init_Reset:
			if (HCIEventHeader->EventCode == EVENT_COMMAND_COMPLETE)
			  NextHCIState = HCISTATE_Init_ReadBufferSize;
			
			break;
		case HCISTATE_Init_ReadBufferSize:
			if (HCIEventHeader->EventCode == EVENT_COMMAND_COMPLETE)
			  NextHCIState = HCISTATE_Init_GetBDADDR;
			
			break;
	}
	
	Bluetooth_HCIStateTransition = (Bluetooth_HCIState != NextHCIState);
	Bluetooth_HCIState           = NextHCIState;
}

bool Bluetooth_HCI_Manage(void)
{
	BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)Bluetooth_OutputPacketBuffer;
	uint16_t                CommandLength    = 0;

	if (!(Bluetooth_HCIStateTransition))
	  return false;
	
	switch (Bluetooth_HCIState)
	{
		case HCISTATE_Init_Reset:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_RESET);
			HCICommandHeader->ParameterLength = 0;
			
			CommandLength = sizeof(BT_HCICommand_Header_t);
			break;
		case HCISTATE_Init_ReadBufferSize:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE),
			HCICommandHeader->ParameterLength = 0;

			CommandLength = sizeof(BT_HCICommand_Header_t);
			break;
		case HCISTATE_Init_GetBDADDR:

			break;
	}
	
	if (CommandLength)
	  CALLBACK_Bluetooth_SendPacket(BLUETOOTH_PACKET_HCICommand, CommandLength, Bluetooth_OutputPacketBuffer);

	Bluetooth_HCIStateTransition = false;
	return true;
}
