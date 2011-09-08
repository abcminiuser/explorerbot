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

#include "HIDClient.h"

static HID_Service_t HIDConnections[MAX_HID_CONNECTIONS];

void Bluetooth_HID_Init(BT_StackConfig_t* const StackState)
{
	for (uint8_t i = 0; i < MAX_HID_CONNECTIONS; i++)
	{
		HID_Service_t* CurrentHIDEntry = &HIDConnections[i];
	
		/* Clear any HID entries in the table that are unassociated or associated with the given initialized stack */
		if ((CurrentHIDEntry->Stack == NULL) || (CurrentHIDEntry->Stack == StackState))
		{
			memset(CurrentHIDEntry, 0x00, sizeof(HID_Service_t));
			CurrentHIDEntry->State = HID_SSTATE_Free;
		}
	}
}

void Bluetooth_HID_Manage(BT_StackConfig_t* const StackState)
{

}

static void Bluetooth_HID_CtlPacket(BT_StackConfig_t* const StackState,
                                    BT_L2CAP_Channel_t* const Channel,
                                    uint16_t Length,
                                    uint8_t* Data)
{
	uint8_t Handshake = HID_HS_ERR_UNSUPPORTED_REQUEST;

	switch (Data[0] & HID_TRANSTYPE_MASK)
	{
		case HID_TRANS_CONTROL:
//			Handshake = HID_HS_SUCCESSFUL;
			break;
		case HID_TRANS_DATA:
		case HID_TRANS_DATAC:
			Handshake = HID_HS_ERR_INVALID_PARAMETER;
			break;
	}

	/* Send a handshake response to ACK/NAK the packet */
	uint8_t Response = (HID_TRANS_HANDSHAKE | Handshake);	
	Bluetooth_L2CAP_SendPacket(StackState, Channel, sizeof(Response), &Response);
}

static void Bluetooth_HID_IntPacket(BT_StackConfig_t* const StackState,
                                    BT_L2CAP_Channel_t* const Channel,
                                    uint16_t Length,
                                    uint8_t* Data)
{
	switch (Data[0] & HID_TRANSTYPE_MASK)
	{
		case HID_TRANS_DATA:
			CALLBACK_Bluetooth_HID_ReportReceived(StackState, Channel, (Data[0] & ~HID_TRANSTYPE_MASK), (Length - 1), &Data[1]);
			break;
	}
}

void Bluetooth_HID_ChannelOpened(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel)
{
	/* Find existing connection in the table if it exists and update the appropriate connection entry */
	for (uint8_t i = 0; i < MAX_HID_CONNECTIONS; i++)
	{
		HID_Service_t* CurrentHIDEntry = &HIDConnections[i];
	
		if ((CurrentHIDEntry->Stack == StackState) && (CurrentHIDEntry->State != HID_SSTATE_Free))
		{
			if (Channel->PSM == CHANNEL_PSM_HIDCTL)
			  CurrentHIDEntry->ControlChannel   = Channel;
			else
			  CurrentHIDEntry->InterruptChannel = Channel;
			
			return;
		}
	}

	/* Create a new connection entry */
	for (uint8_t i = 0; i < MAX_HID_CONNECTIONS; i++)
	{
		HID_Service_t* CurrentHIDEntry = &HIDConnections[i];
	
		/* Find a free entry and create a new HID connection from it */
		if ((CurrentHIDEntry->Stack == NULL) || (CurrentHIDEntry->State == HID_SSTATE_Free))
		{
			memset(CurrentHIDEntry, 0x00, sizeof(HID_Service_t));
			CurrentHIDEntry->Stack = StackState;
			CurrentHIDEntry->State = HID_SSTATE_New;

			if (Channel->PSM == CHANNEL_PSM_HIDCTL)
			  CurrentHIDEntry->ControlChannel   = Channel;
			else
			  CurrentHIDEntry->InterruptChannel = Channel;
		}
	}
}

void Bluetooth_HID_ChannelClosed(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel)
{
	for (uint8_t i = 0; i < MAX_HID_CONNECTIONS; i++)
	{
		HID_Service_t* CurrentHIDEntry = &HIDConnections[i];
	
		/* Find the assocated HID connection and terminate it */
		if ((CurrentHIDEntry->Stack == StackState) || (CurrentHIDEntry->State != HID_SSTATE_Free))
		{
			if (Channel->PSM == CHANNEL_PSM_HIDCTL)
			  CurrentHIDEntry->ControlChannel   = NULL;
			else
			  CurrentHIDEntry->InterruptChannel = NULL;
		
			if (!(CurrentHIDEntry->ControlChannel) && !(CurrentHIDEntry->InterruptChannel))
			  CurrentHIDEntry->State = HID_SSTATE_Free;
		}
	}
}

void Bluetooth_HID_ProcessPacket(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel,
                                 uint16_t Length,
                                 uint8_t* Data)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_HIDCTL:
			Bluetooth_HID_CtlPacket(StackState, Channel, Length, Data);
			break;
		case CHANNEL_PSM_HIDINT:
			Bluetooth_HID_IntPacket(StackState, Channel, Length, Data);
			break;
	}
}