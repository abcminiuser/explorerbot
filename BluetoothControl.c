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
	LCD_Clear();
	LCD_WriteString_P(PSTR("Conn Request:\n"));
	LCD_WriteBDADDR(Connection->RemoteBDADDR);
	
	/* Accept all requests from all devices regardless of BDADDR */
	return true;
}

void EVENT_Bluetooth_ConnectionComplete(BT_StackConfig_t* const StackState,
                                        BT_HCI_Connection_t* const Connection)
{	
	LCD_Clear();
	LCD_WriteString_P(PSTR("Connect:\n"));
	LCD_WriteBDADDR(Connection->RemoteBDADDR);

	/* If connection was locally initiated, open the HID control L2CAP channels */
	if (Connection->LocallyInitiated)
	{
		Bluetooth_L2CAP_OpenChannel(StackState, Connection, CHANNEL_PSM_HIDCTL);
		Bluetooth_L2CAP_OpenChannel(StackState, Connection, CHANNEL_PSM_HIDINT);
	}

	Speaker_PlaySequence(SPEAKER_SEQUENCE_Connected);
}

void EVENT_Bluetooth_ConnectionFailed(BT_StackConfig_t* const StackState,
                                      BT_HCI_Connection_t* const Connection)
{	
	LCD_Clear();
	LCD_WriteString_P(PSTR("Connect Fail:\n"));
	LCD_WriteBDADDR(Connection->RemoteBDADDR);

	Speaker_PlaySequence(SPEAKER_SEQUENCE_ConnectFailed);
}

void EVENT_Bluetooth_DisconnectionComplete(BT_StackConfig_t* const StackState,
                                           BT_HCI_Connection_t* const Connection)
{
	LCD_Clear();
	LCD_WriteString_P(PSTR("Disconnected:\n"));
	LCD_WriteBDADDR(Connection->RemoteBDADDR);

	Speaker_PlaySequence(SPEAKER_SEQUENCE_Disconnected);
}

bool CALLBACK_Bluetooth_ChannelRequest(BT_StackConfig_t* const StackState,
                                       BT_HCI_Connection_t* const Connection,
                                       BT_L2CAP_Channel_t* const Channel)
{
	LCD_Clear();
	LCD_WriteFormattedString_P(PSTR("L2CAP Request\n"
	                                "PSM:%04X"), Channel->PSM);

	/* Accept all channel requests from all devices regardless of PSM */
	return true;
}

void EVENT_Bluetooth_ChannelOpened(BT_StackConfig_t* const StackState,
                                   BT_L2CAP_Channel_t* const Channel)
{
	LCD_Clear();
	LCD_WriteFormattedString_P(PSTR("L2CAP Opened\n"
	                                "L:%04X R:%04X"), Channel->LocalNumber, Channel->RemoteNumber);

	SDP_ChannelOpened(StackState, Channel);			
	HID_ChannelOpened(StackState, Channel);
	RFCOMM_ChannelOpened(StackState, Channel);
}

void EVENT_Bluetooth_ChannelClosed(BT_StackConfig_t* const StackState,
                                   BT_L2CAP_Channel_t* const Channel)
{
	LCD_Clear();
	LCD_WriteFormattedString_P(PSTR("L2CAP Closed\n"
	                                "L:%04X R:%04X"), Channel->LocalNumber, Channel->RemoteNumber);

	SDP_ChannelClosed(StackState, Channel);			
	HID_ChannelClosed(StackState, Channel);
	RFCOMM_ChannelClosed(StackState, Channel);
}

void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
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
			LCD_Clear();
			LCD_WriteFormattedString_P(PSTR("P:%04X L:%04X\n"
			                                "LC:%04X RC:%04X"), Channel->PSM, Length, Channel->LocalNumber, Channel->RemoteNumber);
			break;
	}	
}

void EVENT_RFCOMM_ChannelOpened(BT_StackConfig_t* const StackState,
                                RFCOMM_Channel_t* const Channel)
{
	RFCOMM_SensorStream = Channel;
	
	RFCOMM_SendData(StackState, Channel, strlen("ExplorerBot"), "ExplorerBot");
}

void EVENT_RFCOMM_ChannelClosed(BT_StackConfig_t* const StackState,
                                RFCOMM_Channel_t* const Channel)
{
	RFCOMM_SensorStream = NULL;
}

void CALLBACK_RFCOMM_DataReceived(BT_StackConfig_t* const StackState,
                                  RFCOMM_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	LCD_Clear();
	LCD_WriteFormattedString_P(PSTR("RFCOMM DAT\n"
	                                "C:%02X L:%04X"), Channel->DLCI, Length);
}

void CALLBACK_HID_ReportReceived(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel,
                                 uint8_t ReportType,
                                 uint16_t Length,
                                 uint8_t* Data)
{
	/* Process output reports to look for key code changes */
	if (ReportType == HID_DATAT_Input)
	{
		// TODO: FIXME
		if (Length < 4) // Wiimote
		{
			switch (*((uint16_t*)&Data[1]) & 0x000F)
			{
				default:
					Motors_SetChannelSpeed(0, 0);
					break;
				case 0x0008:
					Motors_SetChannelSpeed( MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
					break;
				case 0x0004:
					Motors_SetChannelSpeed(-MAX_MOTOR_POWER, -MAX_MOTOR_POWER);					
					break;
				case 0x0001:
					Motors_SetChannelSpeed(-MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
					break;
				case 0x0002:
					Motors_SetChannelSpeed( MAX_MOTOR_POWER, -MAX_MOTOR_POWER);					
					break;
			}
			
			Headlights_SetState(Data[2] & 0x04);
			Speaker_Tone((Data[2] & 0x08) ? 30 : 0);
			
			static uint16_t PrevButtons = 0;
			
			if (!(PrevButtons & 0x0010) && (*((uint16_t*)&Data[1]) & 0x0010))
			  Headlights_ToggleState();
			
			if (!(PrevButtons & 0x1000) && (*((uint16_t*)&Data[1]) & 0x1000))
			  Speaker_PlaySequence(SPEAKER_SEQUENCE_LaCucaracha);

			PrevButtons = *((uint16_t*)&Data[1]);
		}
		else if (Length < 12) // Sony Ericson Z550i Phone
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
			Speaker_Tone((Data[1] & 0x02) ? 30 : 0);
		}
		else // PS3 Controller
		{
			switch (*((uint16_t*)&Data[2]))
			{
				default:
					Motors_SetChannelSpeed(0, 0);
					break;
				case 0x0010:
					Motors_SetChannelSpeed( MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
					break;
				case 0x0040:
					Motors_SetChannelSpeed(-MAX_MOTOR_POWER, -MAX_MOTOR_POWER);					
					break;
				case 0x0080:
					Motors_SetChannelSpeed(-MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
					break;
				case 0x0020:
					Motors_SetChannelSpeed( MAX_MOTOR_POWER, -MAX_MOTOR_POWER);					
					break;
			}
			
			Headlights_SetState(*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_R2 - 1)));
			Speaker_Tone((*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_L2 - 1))) ? 30 : 0);
			
			static uint16_t PrevButtons = 0;

			if (!(PrevButtons & (1 << (PS3CONTROLLER_BUTTON_R1 - 1))) && (*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_R1 - 1))))
			  Headlights_ToggleState();
			
			if (!(PrevButtons & (1 << (PS3CONTROLLER_BUTTON_L1 - 1))) && (*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_L1 - 1))))
			  Speaker_PlaySequence(SPEAKER_SEQUENCE_LaCucaracha);
			
			PrevButtons = *((uint16_t*)&Data[2]);
		}
	}
}

