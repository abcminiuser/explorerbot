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

#include "RFCOMM.h"

void RFCOMM_Init(BT_StackConfig_t* const StackState)
{
	/* Register the RFCOMM virtual serial port service with the SDP service */
	SDP_RegisterService(&ServiceEntry_RFCOMMSerialPort);
}

void RFCOMM_Manage(BT_StackConfig_t* const StackState)
{

}

void RFCOMM_ChannelOpened(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel)
{

}

void RFCOMM_ChannelClosed(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel)
{

}

#include "../../../LCD.h"
void RFCOMM_ProcessPacket(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel,
                          uint16_t Length,
                          uint8_t* Data)
{
	LCD_Clear();
	LCD_WriteString("RFCOMM");
}

