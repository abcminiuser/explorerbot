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
	LCD_WriteString_P(PSTR("\fHCI Conn Request\n"));
	LCD_WriteBDADDR(Connection->RemoteBDADDR);
	
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
	
	LCD_WriteFormattedString_P(PSTR("\fHCI %S\n"), StateMessage);
	LCD_WriteBDADDR(Connection->RemoteBDADDR);
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
			LCD_WriteFormattedString_P(PSTR("\fL2CAP DATA:%04X\n"
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

			/* Construct the sensor CSV header line(s) */
			char    LineBuffer[200];
			uint8_t LineLength = Sensors_WriteSensorCSVHeader(LineBuffer);

			/* Write sensor CSV data to the virtual serial port */
			RFCOMM_SendData(RFCOMM_SensorStream, LineLength, LineBuffer);
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
	uint16_t MotorPower[2]    = {0, 0};
	bool     Headlights       = false;
	bool     HeadlightsToggle = false;
	bool     Horn             = false;
	bool     NoveltyHorn      = false;

	/* Process input reports to look for key code changes */
	if (ReportType == HID_DATAT_Input)
	{
		if (Length < 4) // Wiimote
		{
			switch (*((uint16_t*)&Data[1]) & 0x000F)
			{
				case 0x0008:
					MotorPower[0] =  MAX_MOTOR_POWER;
					MotorPower[1] =  MAX_MOTOR_POWER;
					break;
				case 0x0004:
					MotorPower[0] = -MAX_MOTOR_POWER;
					MotorPower[1] = -MAX_MOTOR_POWER;
					break;
				case 0x0001:
					MotorPower[0] = -MAX_MOTOR_POWER;
					MotorPower[1] =  MAX_MOTOR_POWER;
					break;
				case 0x0002:
					MotorPower[0] =  MAX_MOTOR_POWER;
					MotorPower[1] = -MAX_MOTOR_POWER;
					break;
			}
			
			static uint16_t PrevButtons = 0;
			
			Headlights       = ((Data[2] & 0x04) ? true : false);
			HeadlightsToggle = (!(PrevButtons & 0x0010) && (*((uint16_t*)&Data[1]) & 0x0010));
			Horn             = ((Data[2] & 0x08) ? true : false);
			NoveltyHorn      = (!(PrevButtons & 0x1000) && (*((uint16_t*)&Data[1]) & 0x1000));

			PrevButtons = *((uint16_t*)&Data[1]);
		}
		else if (Length < 12) // Sony Ericson Z550i Phone
		{
			switch (*((uint16_t*)&Data[2]))
			{
				case 0xF600:
					MotorPower[0] =  MAX_MOTOR_POWER;
					MotorPower[1] =  MAX_MOTOR_POWER;
					break;
				case 0x0A00:
					MotorPower[0] = -MAX_MOTOR_POWER;
					MotorPower[1] = -MAX_MOTOR_POWER;
					break;
				case 0x00F6:
					MotorPower[0] = -MAX_MOTOR_POWER;
					MotorPower[1] =  MAX_MOTOR_POWER;
					break;
				case 0x000A:
					MotorPower[0] =  MAX_MOTOR_POWER;
					MotorPower[1] = -MAX_MOTOR_POWER;
					break;
			}
			
			static uint8_t PrevButtons = 0;

			HeadlightsToggle = (!(PrevButtons & 0x01) && (Data[1] & 0x01));
			Horn             = ((Data[1] & 0x02) ? true : false);
			
			PrevButtons = Data[1];
		}
		else // PS3 Controller
		{
			switch (*((uint16_t*)&Data[2]))
			{
				case 0x0010:
					MotorPower[0] =  MAX_MOTOR_POWER;
					MotorPower[1] =  MAX_MOTOR_POWER;
					break;
				case 0x0040:
					MotorPower[0] = -MAX_MOTOR_POWER;
					MotorPower[1] = -MAX_MOTOR_POWER;
					break;
				case 0x0080:
					MotorPower[0] = -MAX_MOTOR_POWER;
					MotorPower[1] =  MAX_MOTOR_POWER;
					break;
				case 0x0020:
					MotorPower[0] =  MAX_MOTOR_POWER;
					MotorPower[1] = -MAX_MOTOR_POWER;
					break;
			}
			
			static uint16_t PrevButtons = 0;

			Headlights       = ((*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_R2 - 1))) ? true : false);
			HeadlightsToggle = (!(PrevButtons & (1 << (PS3CONTROLLER_BUTTON_R1 - 1))) && (*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_R1 - 1))));
			Horn             = ((*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_L2 - 1))) ? true : false);
			NoveltyHorn      = (!(PrevButtons & (1 << (PS3CONTROLLER_BUTTON_L1 - 1))) && (*((uint16_t*)&Data[2]) & (1 << (PS3CONTROLLER_BUTTON_L1 - 1))));

			PrevButtons = *((uint16_t*)&Data[2]);
		}
		
		/* Change robot hardware states to match the values in the controller report */
		Motors_SetChannelSpeed(MotorPower[0], MotorPower[1]);
		Headlights_SetState(Headlights);
		Speaker_Tone(Horn ? 30 : 0);

		if (HeadlightsToggle)
		  Headlights_ToggleState();
		
		if (NoveltyHorn)
		  Speaker_PlaySequence(SPEAKER_SEQUENCE_LaCucaracha);
	}
}

