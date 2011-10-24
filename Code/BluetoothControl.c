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
 *  Bluetooth stack control event and callback functions, linking the Bluetooth stack to the user application.
 */

#include "BluetoothControl.h"

/** Handle for the currently open RFCOMM communication channel between the robot and a remote device. */
RFCOMM_Channel_t* RFCOMM_SensorStream = NULL;


void EVENT_Bluetooth_InitServices(BT_StackConfig_t* const StackState)
{
	SDP_Init(StackState);
	HID_Init(StackState);
	RFCOMM_Init(StackState);
}

void EVENT_Bluetooth_ManageServices(BT_StackConfig_t* const StackState)
{
	SDP_Manage(StackState);
	HID_Manage(StackState);
	RFCOMM_Manage(StackState);
}


bool CALLBACK_Bluetooth_ConnectionRequest(BT_StackConfig_t* const StackState,
                                          BT_HCI_Connection_t* const Connection)
{
	LCD_WriteFormattedString_P(PSTR("\fHCI Conn Request\n"
	                                  "%02X%02X:%02X%02X:%02X%02X"), Connection->RemoteBDADDR[5], Connection->RemoteBDADDR[4],
	                                                                 Connection->RemoteBDADDR[3], Connection->RemoteBDADDR[2],
	                                                                 Connection->RemoteBDADDR[1], Connection->RemoteBDADDR[0]);
	
	/* Accept all requests from all devices regardless of BDADDR */
	return true;
}

void EVENT_Bluetooth_ConnectionStateChange(BT_StackConfig_t* const StackState,
                                           BT_HCI_Connection_t* const Connection)
{	
	const char* StateMessage;
	
	/* Determine the connection event that has occurred */
	switch (Connection->State)
	{
		case HCI_CONSTATE_Connected:
			StateMessage = PSTR("Connected");
			Speaker_PlaySequence(SPEAKER_SEQUENCE_Connected);

			/* If connection was locally initiated, open the HID control L2CAP channels */
			if (Connection->LocallyInitiated)
			{
				Bluetooth_L2CAP_OpenChannel(StackState, Connection, CHANNEL_PSM_HIDCTL);
				Bluetooth_L2CAP_OpenChannel(StackState, Connection, CHANNEL_PSM_HIDINT);
			}
			break;
		case HCI_CONSTATE_Failed:
			StateMessage = PSTR("Connect Fail");
			Speaker_PlaySequence(SPEAKER_SEQUENCE_ConnectFailed);
			break;
		case HCI_CONSTATE_Closed:
			StateMessage = PSTR("Disconnected");
			Speaker_PlaySequence(SPEAKER_SEQUENCE_Disconnected);
			break;
		default:
			return;
	}
	
	LCD_WriteFormattedString_P(PSTR("\fHCI %S\n"
	                                  "%02X%02X:%02X%02X:%02X%02X"), StateMessage,
	                                                                 Connection->RemoteBDADDR[5], Connection->RemoteBDADDR[4],
	                                                                 Connection->RemoteBDADDR[3], Connection->RemoteBDADDR[2],
	                                                                 Connection->RemoteBDADDR[1], Connection->RemoteBDADDR[0]);
}

bool CALLBACK_Bluetooth_ChannelRequest(BT_StackConfig_t* const StackState,
                                       BT_HCI_Connection_t* const Connection,
                                       BT_L2CAP_Channel_t* const Channel)
{
	LCD_WriteFormattedString_P(PSTR("\fL2CAP Request\n"
	                                  "PSM:%04X"), Channel->PSM);

	/* Accept all channel requests from all devices regardless of PSM */
	return true;
}

void EVENT_Bluetooth_ChannelStateChange(BT_StackConfig_t* const StackState,
                                        BT_L2CAP_Channel_t* const Channel)
{
	const char* StateMessage;

	/* Determine the channel event that has occurred */
	switch (Channel->State)
	{
		case L2CAP_CHANSTATE_Open:
			StateMessage = PSTR("Opened");

			SDP_ChannelOpened(StackState, Channel);			
			HID_ChannelOpened(StackState, Channel);
			RFCOMM_ChannelOpened(StackState, Channel);
			break;
		case L2CAP_CHANSTATE_Closed:
			StateMessage = PSTR("Closed");

			SDP_ChannelClosed(StackState, Channel);
			HID_ChannelClosed(StackState, Channel);
			RFCOMM_ChannelClosed(StackState, Channel);
			break;
		default:
			return;
	}

	LCD_WriteFormattedString_P(PSTR("\fL2CAP %S\n"
	                                  "PSM:%04X C:%04X"), StateMessage, Channel->PSM, Channel->LocalNumber);
}

void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	/* Determine if the PSM is known or not - indicate unknown PSM packets */
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
		case CHANNEL_PSM_RFCOMM:
			SDP_ProcessPacket(StackState, Channel, Length, Data);
			HID_ProcessPacket(StackState, Channel, Length, Data);
			RFCOMM_ProcessPacket(StackState, Channel, Length, Data);
			break;		
		default:
			LCD_WriteFormattedString_P(PSTR("\fL2CAP Recv:%04X\n"
			                                  "PSM:%04X C:%04X"), Length, Channel->PSM, Channel->LocalNumber);
			break;
	}
}

void EVENT_RFCOMM_ChannelStateChange(BT_StackConfig_t* const StackState,
                                     RFCOMM_Channel_t* const Channel)
{
	/* Determine the RFCOMM channel event that has occurred */
	switch (Channel->State)
	{
		case RFCOMM_Channel_Open:
			/* Save the handle to the opened RFCOMM channel object, so we can write to it later */
			RFCOMM_SensorStream = Channel;
			break;
		case RFCOMM_Channel_Closed:
			/* Delete the handle to the now closed RFCOMM channel object, to prevent us from trying to write to it */
			RFCOMM_SensorStream = NULL;
			break;
		default:
			return;
	}
}

void CALLBACK_RFCOMM_DataReceived(BT_StackConfig_t* const StackState,
                                  RFCOMM_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	/* Do nothing with received data from the RFCOMM channel */
}


void CALLBACK_HID_ReportReceived(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel,
                                 uint8_t ReportType,
                                 uint16_t Length,
                                 uint8_t* Data)
{
	uint8_t MotorX           = 1;
	uint8_t MotorY           = 1;
	bool    Headlights       = false;
	bool    HeadlightsToggle = false;
	bool    Horn             = false;
	bool    NoveltyHorn      = false;

	/* Process input reports to look for key code changes */
	if (ReportType == HID_DATAT_Input)
	{
		if (Length < 4) // Wiimote
		{
			static uint16_t PrevButtons = 0;

			/* Left/Right */
			if (Data[1] & 0x01)
			  MotorX = 0;
			else if (Data[1] & 0x02)
			  MotorX = 2;
			
			/* Forward/Backward */
			if (Data[1] & 0x08)
			  MotorY = 0;
			else if (Data[1] & 0x04)
			  MotorY = 2;

			/* Other Features */
			Headlights       = ((Data[2] & 0x04) ? true : false);
			HeadlightsToggle = (!(PrevButtons & 0x0010) && (*((uint16_t*)&Data[1]) & 0x0010));
			Horn             = ((Data[2] & 0x08) ? true : false);
			NoveltyHorn      = (!(PrevButtons & 0x1000) && (*((uint16_t*)&Data[1]) & 0x1000));

			PrevButtons = *((uint16_t*)&Data[1]);
		}
		else if (Length < 12) // Sony Ericson Z550i Phone
		{
			static uint8_t PrevButtons = 0;

			/* Left/Right */
			if (Data[2] == 0xF6)
			  MotorX = 0;
			else if (Data[2] == 0x0A)
			  MotorX = 2;

			/* Forward/Backward */
			if (Data[3] == 0xF6)
			  MotorY = 0;
			else if (Data[3] == 0x0A)
			  MotorY = 2;
			
			/* Other Features */
			HeadlightsToggle = (!(PrevButtons & 0x01) && (Data[1] & 0x01));
			Horn             = ((Data[1] & 0x02) ? true : false);
			
			PrevButtons = Data[1];
		}
		else // PS3 Controller
		{
			static uint16_t PrevButtons = 0;
			
			/* Left/Right */
			if (Data[2] & 0x80)
			  MotorX = 0;
			else if (Data[2] & 0x20)
			  MotorX = 2;

			/* Forward/Backward */
			if (Data[2] & 0x10)
			  MotorY = 0;
			else if (Data[2] & 0x40)
			  MotorY = 2;
			
			/* Other Features */
			Headlights       = ((*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_R2 - 1))) ? true : false);
			HeadlightsToggle = (!(PrevButtons & (1 << (PS3CONTROLLER_BUTTON_R1 - 1))) && (*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_R1 - 1))));
			Horn             = ((*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_L2 - 1))) ? true : false);
			NoveltyHorn      = (!(PrevButtons & (1 << (PS3CONTROLLER_BUTTON_L1 - 1))) && (*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_L1 - 1))));

			PrevButtons = *((uint16_t*)&Data[2]);
		}
		
		ProcessUserControl(MotorX, MotorY, Horn, NoveltyHorn, Headlights, HeadlightsToggle);
	}
}

