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

/** 8-Bit CRC table used by the FCS field of each RFCOMM encoded frame, sourced from the ETSI TS 101 369 V7.2.0
 *  specification document, upon which the RFCOMM specification is based.
 */
static const uint8_t RFCOMM_CRC8Table[256] PROGMEM =
	{
		0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
		0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
		0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
		0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
		0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
		0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
		0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
		0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
		0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
		0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
		0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
		0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
		0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
		0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
		0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
		0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
	};

/** RFCOMM channel state structure, to retain information about each open channel in the RFCOMM multiplexer. */
static RFCOMM_Channel_t RFCOMM_Channels[RFCOMM_MAX_OPEN_CHANNELS];


void RFCOMM_Init(BT_StackConfig_t* const StackState)
{
	/* Reset the RFCOMM channel structures, to invalidate any configured RFCOMM channels */
	for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
	  RFCOMM_Channels[i].State = RFCOMM_Channel_Closed;

	/* Register the RFCOMM virtual serial port service with the SDP service */
	SDP_RegisterService(&ServiceEntry_RFCOMMSerialPort);
}

void RFCOMM_Manage(BT_StackConfig_t* const StackState)
{
	for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
	{
		RFCOMM_Channel_t* RFCOMMChannel = &RFCOMM_Channels[i];

		if (RFCOMMChannel->State == RFCOMM_Channel_Configure)
		{
			/* Check if the local signals have not yet been sent on the current channel */
			if (!(RFCOMMChannel->ConfigFlags & RFCOMM_CONFIG_LOCALSIGNALSSENT))
			{
				/* Indicate that the local signals have been sent, transmit them to the remote device */
				RFCOMMChannel->ConfigFlags |= RFCOMM_CONFIG_LOCALSIGNALSSENT;
				//RFCOMM_SendChannelSignals(RFCOMMChannel);
			}

			/* If signals have been configured in both directions, progress to the open state */
			if ((RFCOMMChannel->ConfigFlags & (RFCOMM_CONFIG_REMOTESIGNALS | RFCOMM_CONFIG_LOCALSIGNALS)) ==
			                                  (RFCOMM_CONFIG_REMOTESIGNALS | RFCOMM_CONFIG_LOCALSIGNALS))
			{
				RFCOMMChannel->State = RFCOMM_Channel_Open;
				//CALLBACK_RFCOMM_ChannelOpened(RFCOMMChannel);
			}
		}		
	}
}

void RFCOMM_ChannelOpened(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel)
{

}

void RFCOMM_ChannelClosed(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel)
{

}

static void RFCOMM_ProcessDM(BT_StackConfig_t* const StackState,
                             BT_L2CAP_Channel_t* const Channel,
                             RFCOMM_Header_t* FrameHeader)
{
	// TODO
}

static void RFCOMM_ProcessDISC(BT_StackConfig_t* const StackState,
                               BT_L2CAP_Channel_t* const Channel,
                               RFCOMM_Header_t* FrameHeader)
{
	// TODO
}

static void RFCOMM_ProcessSABM(BT_StackConfig_t* const StackState,
                               BT_L2CAP_Channel_t* const Channel,
                               RFCOMM_Header_t* FrameHeader)
{
	// TODO
}

static void RFCOMM_ProcessUA(BT_StackConfig_t* const StackState,
                             BT_L2CAP_Channel_t* const Channel,
                             RFCOMM_Header_t* FrameHeader)
{
	// TODO
}

static void RFCOMM_ProcessUIH(BT_StackConfig_t* const StackState,
                              BT_L2CAP_Channel_t* const Channel,
                              RFCOMM_Header_t* FrameHeader)
{
	// TODO
}

#include "../../../Drivers/LCD.h"
void RFCOMM_ProcessPacket(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel,
                          uint16_t Length,
                          uint8_t* Data)
{
	/* Ensure correct channel PSM before processing the data */
	if (Channel->PSM != CHANNEL_PSM_RFCOMM)
	  return;
	  
	RFCOMM_Header_t* FrameHeader = (RFCOMM_Header_t*)Data;
	
	/* Decode the RFCOMM frame type from the header */
	switch (FrameHeader->Control & ~FRAME_POLL_FINAL)
	{
		case RFCOMM_Frame_SABM:
			LCD_Clear();
			LCD_WriteString("SABM");

			RFCOMM_ProcessSABM(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_UA:
			LCD_Clear();
			LCD_WriteString("UA");

			RFCOMM_ProcessUA(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_DM:
			LCD_Clear();
			LCD_WriteString("DM");

			RFCOMM_ProcessDM(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_DISC:
			LCD_Clear();
			LCD_WriteString("DISC");

			RFCOMM_ProcessDISC(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_UIH:
			LCD_Clear();
			LCD_WriteString("UIH");

			RFCOMM_ProcessUIH(StackState, Channel, FrameHeader);
			break;
	}
}

