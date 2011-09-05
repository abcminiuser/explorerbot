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

#include "BluetoothHID.h"

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
	
}

void Bluetooth_HID_ChannelClosed(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel)
{
	
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