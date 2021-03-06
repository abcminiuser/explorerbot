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
 *  Bluetooth Human Interface Device (HID) service.
 */

#include "HID.h"

static void HID_CtlPacket(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel,
                          uint16_t Length,
                          uint8_t* Data)
{
	uint8_t Handshake = HID_HS_ERR_UNSUPPORTED_REQUEST;

	switch (Data[0] & HID_TRANSTYPE_MASK)
	{
		case HID_TRANS_DATA:
		case HID_TRANS_DATAC:
			Handshake = HID_HS_ERR_INVALID_PARAMETER;
			break;
	}

	/* Send a handshake response to ACK/NAK the packet */
	uint8_t Response = (HID_TRANS_HANDSHAKE | Handshake);
	Bluetooth_L2CAP_SendPacket(StackState, Channel, sizeof(Response), &Response);
}

static void HID_IntPacket(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel,
                          uint16_t Length,
                          uint8_t* Data)
{
	switch (Data[0] & HID_TRANSTYPE_MASK)
	{
		case HID_TRANS_DATA:
		case HID_TRANS_DATAC:
			/* HID report data received, pass it to the client */
			CALLBACK_HID_ReportReceived(StackState, Channel, (Data[0] & ~HID_TRANSTYPE_MASK), (Length - 1), &Data[1]);
			break;
	}
}

void HID_Init(BT_StackConfig_t* const StackState)
{

}

void HID_Manage(BT_StackConfig_t* const StackState)
{

}

void HID_ChannelOpened(BT_StackConfig_t* const StackState,
                       BT_L2CAP_Channel_t* const Channel)
{
	static BT_L2CAP_Channel_t* CChannel = NULL;
	static BT_L2CAP_Channel_t* DChannel = NULL;

	switch (Channel->PSM)
	{
		case CHANNEL_PSM_HIDCTL:
			CChannel = Channel;
			break;
		case CHANNEL_PSM_HIDINT:
			DChannel = Channel;
			break;
	}

	/* Send special SET REPORT request to start PS3 controller reporting over Bluetooth */
	if (CChannel && DChannel)
	{
		uint8_t PS3_StartReport[] = {HID_TRANS_SET_REPORT | 0x03, 0xF4, 0x42, 0x03, 0x00, 0x00};
		Bluetooth_L2CAP_SendPacket(StackState, CChannel, sizeof(PS3_StartReport), PS3_StartReport);

		CChannel = NULL;
		DChannel = NULL;
	}
}

void HID_ChannelClosed(BT_StackConfig_t* const StackState,
                       BT_L2CAP_Channel_t* const Channel)
{

}

void HID_ProcessPacket(BT_StackConfig_t* const StackState,
                       BT_L2CAP_Channel_t* const Channel,
                       const uint16_t Length,
                       uint8_t* Data)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_HIDCTL:
			HID_CtlPacket(StackState, Channel, Length, Data);
			break;
		case CHANNEL_PSM_HIDINT:
			HID_IntPacket(StackState, Channel, Length, Data);
			break;
	}
}

