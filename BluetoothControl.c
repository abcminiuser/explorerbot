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

#include "BluetoothControl.h"

void EVENT_Bluetooth_InitServices(BT_StackConfig_t* const StackState)
{
	SDP_Server_Init(StackState);
	HID_Client_Init(StackState);
}

void EVENT_Bluetooth_ManageServices(BT_StackConfig_t* const StackState)
{
	SDP_Server_Manage(StackState);
	HID_Client_Manage(StackState);
}

bool CALLBACK_Bluetooth_ConnectionRequest(BT_StackConfig_t* const StackState,
                                          BT_HCI_Connection_t* const ConnectionHandle)
{
	LCD_Clear();
	LCD_WriteFormattedString("Conn Request:\n"
	                         "%02X%02X:%02X%02X:%02X%02X", ConnectionHandle->RemoteBDADDR[5], ConnectionHandle->RemoteBDADDR[4],
	                                                       ConnectionHandle->RemoteBDADDR[3], ConnectionHandle->RemoteBDADDR[2],
	                                                       ConnectionHandle->RemoteBDADDR[1], ConnectionHandle->RemoteBDADDR[0]);

	/* Accept all requests from all devices regardless of BDADDR */
	return true;
}

void EVENT_Bluetooth_ConnectionComplete(BT_StackConfig_t* const StackState,
                                        BT_HCI_Connection_t* const ConnectionHandle)
{	
	LCD_Clear();
	LCD_WriteFormattedString("Connected:\n"
	                         "%02X%02X:%02X%02X:%02X%02X", ConnectionHandle->RemoteBDADDR[5], ConnectionHandle->RemoteBDADDR[4],
	                                                       ConnectionHandle->RemoteBDADDR[3], ConnectionHandle->RemoteBDADDR[2],
	                                                       ConnectionHandle->RemoteBDADDR[1], ConnectionHandle->RemoteBDADDR[0]);
}

void EVENT_Bluetooth_DisconnectionComplete(BT_StackConfig_t* const StackState,
                                           BT_HCI_Connection_t* const ConnectionHandle)
{
	LCD_Clear();
	LCD_WriteFormattedString("Disconnected:\n"
	                         "%02X%02X:%02X%02X:%02X%02X", ConnectionHandle->RemoteBDADDR[5], ConnectionHandle->RemoteBDADDR[4],
	                                                       ConnectionHandle->RemoteBDADDR[3], ConnectionHandle->RemoteBDADDR[2],
	                                                       ConnectionHandle->RemoteBDADDR[1], ConnectionHandle->RemoteBDADDR[0]);
}

bool CALLBACK_Bluetooth_ChannelRequest(BT_StackConfig_t* const StackState,
                                       BT_HCI_Connection_t* const Connection,
                                       BT_L2CAP_Channel_t* const Channel)
{
	/* Accept all channel requests from all devices regardless of PSM */
	return true;
}

void EVENT_Bluetooth_ChannelOpened(BT_StackConfig_t* const StackState,
                                   BT_L2CAP_Channel_t* const Channel)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
			SDP_Server_ChannelOpened(StackState, Channel);			
			break;
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
			HID_Client_ChannelOpened(StackState, Channel);
			break;
	}

	LCD_Clear();
	LCD_WriteFormattedString("L2CAP Opened\n"
	                         "L:%04X R:%04X", Channel->LocalNumber, Channel->RemoteNumber);
}

void EVENT_Bluetooth_ChannelClosed(BT_StackConfig_t* const StackState,
                                   BT_L2CAP_Channel_t* const Channel)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
			SDP_Server_ChannelClosed(StackState, Channel);			
			break;
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
			HID_Client_ChannelClosed(StackState, Channel);
			break;
	}

	LCD_Clear();
	LCD_WriteFormattedString("L2CAP Closed\n"
	                         "L:%04X R:%04X", Channel->LocalNumber, Channel->RemoteNumber);
}

void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
			SDP_Server_ProcessPacket(StackState, Channel, Length, Data);
			break;
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
			HID_Client_ProcessPacket(StackState, Channel, Length, Data);
			break;
		default:
			LCD_Clear();
			LCD_WriteFormattedString("P:%04X L:%04X\n"
			                         "LC:%04X RC:%04X", Channel->PSM, Length, Channel->LocalNumber, Channel->RemoteNumber);
			break;
	}	
}

void CALLBACK_HID_Client_ReportReceived(BT_StackConfig_t* const StackState,
                                        BT_L2CAP_Channel_t* const Channel,
                                        uint8_t ReportType,
                                        uint16_t Length,
                                        uint8_t* Data)
{
	/* Process output reports to look for key code changes */
	if (ReportType == HID_DATAT_Input)
	{
		// TODO: FIXME
		switch (*((uint16_t*)&Data[2]))
		{
			default:
				Motors_SetChannelSpeed(0, 0);
				break;
			case 0xF600:
				Motors_SetChannelSpeed( MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
				break;
			case 0x0A00:
				Motors_SetChannelSpeed(-MAX_MOTOR_POWER, -MAX_MOTOR_POWER);					
				break;
			case 0x00F6:
				Motors_SetChannelSpeed(-MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
				break;
			case 0x000A:
				Motors_SetChannelSpeed( MAX_MOTOR_POWER, -MAX_MOTOR_POWER);					
				break;
		}
		
		Headlights_SetState(Data[1] & 0x01);
		Speaker_Tone((Data[1] & 0x02) ? 250 : 0);
	}
}

