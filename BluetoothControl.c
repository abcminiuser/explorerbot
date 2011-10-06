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
	                                "PSM:%04X LC:%04X"), Channel->PSM, Channel->LocalNumber);

	SDP_ChannelOpened(StackState, Channel);			
	HID_ChannelOpened(StackState, Channel);
	RFCOMM_ChannelOpened(StackState, Channel);
}

void EVENT_Bluetooth_ChannelClosed(BT_StackConfig_t* const StackState,
                                   BT_L2CAP_Channel_t* const Channel)
{
	LCD_Clear();
	LCD_WriteFormattedString_P(PSTR("L2CAP Closed\n"
	                                "PSM:%04X LC:%04X"), Channel->PSM, Channel->LocalNumber);

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
			LCD_WriteFormattedString_P(PSTR("PSM:%04X L:%04X\n"
			                                "LC:%04X RC:%04X"), Channel->PSM, Length, Channel->LocalNumber, Channel->RemoteNumber);
			break;
	}	
}


void EVENT_RFCOMM_ChannelOpened(BT_StackConfig_t* const StackState,
                                RFCOMM_Channel_t* const Channel)
{
	/* Save the handle to the opened RFCOMM channel object, so we can write to it later */
	RFCOMM_SensorStream = Channel;

	/* Construct the sensor CSV header lines and write them to the virtual serial port */
	char LineBuffer[200];
	Sensors_WriteSensorCSVHeader(LineBuffer);
	RFCOMM_SendData(RFCOMM_SensorStream, strlen(LineBuffer), LineBuffer);
}

void EVENT_RFCOMM_ChannelClosed(BT_StackConfig_t* const StackState,
                                RFCOMM_Channel_t* const Channel)
{
	/* Delete the handle to the now closed RFCOMM channel object, to prevent us from trying to write to it */
	RFCOMM_SensorStream = NULL;
}

void CALLBACK_RFCOMM_DataReceived(BT_StackConfig_t* const StackState,
                                  RFCOMM_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{

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

	/* Process output reports to look for key code changes */
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
		
		Motors_SetChannelSpeed(MotorPower[0], MotorPower[1]);
		Headlights_SetState(Headlights);
		Speaker_Tone(Horn ? 30 : 0);

		if (HeadlightsToggle)
		  Headlights_ToggleState();
		
		if (NoveltyHorn)
		  Speaker_PlaySequence(SPEAKER_SEQUENCE_LaCucaracha);
	}
}

