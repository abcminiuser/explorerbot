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

void Bluetooth_HCI_Init(Bluetooth_Device_t* const StackState)
{
	StackState->State.HCI.State           = HCISTATE_Init_Reset;
	StackState->State.HCI.StateTransition = true;
}

void Bluetooth_HCI_ProcessPacket(Bluetooth_Device_t* const StackState, uint8_t* Data)
{
	BT_HCIEvent_Header_t* HCIEventHeader = (BT_HCIEvent_Header_t*)Data;
	uint8_t               NextHCIState   = StackState->State.HCI.State;
	
	if (HCIEventHeader->EventCode == EVENT_COMMAND_COMPLETE)
	{
		BT_HCIEvent_CommandComplete_t* CommandCompleteHeader = (BT_HCIEvent_CommandComplete_t*)&HCIEventHeader->Parameters;
		
		switch (StackState->State.HCI.State)
		{
			case HCISTATE_Init_Reset:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_RESET))
				  NextHCIState = HCISTATE_Init_ReadBufferSize;
				break;
			case HCISTATE_Init_ReadBufferSize:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE))
				  NextHCIState = HCISTATE_Init_GetBDADDR;
				break;
			case HCISTATE_Init_GetBDADDR:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBDADDR))
				{
					memcpy(StackState->State.HCI.LocalBDADDR, &CommandCompleteHeader->Parameters[1], sizeof(StackState->State.HCI.LocalBDADDR));				
					NextHCIState = HCISTATE_Init_SetLocalName;
				}
				break;
			case HCISTATE_Init_SetLocalName:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME))
				  NextHCIState = HCISTATE_Init_SetDeviceClass;
				break;
			case HCISTATE_Init_SetDeviceClass:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE))
				  NextHCIState = HCISTATE_Init_SetScanEnable;
				break;
			case HCISTATE_Init_SetScanEnable:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE))
				  NextHCIState = HCISTATE_Idle;
				break;
		}
	}
	
	StackState->State.HCI.StateTransition = (StackState->State.HCI.State != NextHCIState);
	StackState->State.HCI.State           = NextHCIState;
}

bool Bluetooth_HCI_Manage(Bluetooth_Device_t* const StackState)
{
	BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.OutputPacketBuffer;

	if (!(StackState->State.HCI.StateTransition))
	  return false;
	
	HCICommandHeader->OpCode = 0;
	
	switch (StackState->State.HCI.State)
	{
		case HCISTATE_Init_Reset:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_RESET);
			HCICommandHeader->ParameterLength = 0;
			break;
		case HCISTATE_Init_ReadBufferSize:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE),
			HCICommandHeader->ParameterLength = 0;
			break;
		case HCISTATE_Init_GetBDADDR:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBDADDR),
			HCICommandHeader->ParameterLength = 0;
			break;
		case HCISTATE_Init_SetLocalName:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME),
			HCICommandHeader->ParameterLength = 248;	
			strlcpy((void*)HCICommandHeader->Parameters, StackState->Config.Name, HCICommandHeader->ParameterLength);
			break;
		case HCISTATE_Init_SetDeviceClass:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE),
			HCICommandHeader->ParameterLength = 3;
			HCICommandHeader->Parameters[0]   = (StackState->Config.Class & 0xFF);
			HCICommandHeader->Parameters[1]   = (StackState->Config.Class >> 8);
			HCICommandHeader->Parameters[2]   = (StackState->Config.Class >> 16);
			break;
		case HCISTATE_Init_SetScanEnable:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE),
			HCICommandHeader->ParameterLength = 1;
			HCICommandHeader->Parameters[0]   = (StackState->Config.Discoverable ? BT_SCANMODE_InquiryAndPageScans : BT_SCANMODE_NoScansEnabled);
			break;
	}
	
	if (HCICommandHeader->OpCode)
	  CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + HCICommandHeader->ParameterLength), StackState->Config.OutputPacketBuffer);

	StackState->State.HCI.StateTransition = false;
	return true;
}
