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

// TODO: FIXME
#include "../../../Motors.h"

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
		case CHANNEL_PSM_HIDINT:
			// TODO: FIXME
			switch (*((uint16_t*)&Data[3]))
			{
				default:
					Motors_SetChannelSpeed(MOTOR_CHANNEL_All,    0);
					break;
				case 0xF600:
					Motors_SetChannelSpeed(MOTOR_CHANNEL_All,    MAX_MOTOR_POWER);
					break;
				case 0x00F6:
					Motors_SetChannelSpeed(MOTOR_CHANNEL_Left,   MAX_MOTOR_POWER);
					Motors_SetChannelSpeed(MOTOR_CHANNEL_Right, -MAX_MOTOR_POWER);					
					break;
				case 0x0A00:
					Motors_SetChannelSpeed(MOTOR_CHANNEL_All,   -MAX_MOTOR_POWER);					
					break;
				case 0x000A:
					Motors_SetChannelSpeed(MOTOR_CHANNEL_Left,  -MAX_MOTOR_POWER);
					Motors_SetChannelSpeed(MOTOR_CHANNEL_Right,  MAX_MOTOR_POWER);					
					break;
			}
			break;
	}
}