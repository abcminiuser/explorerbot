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
 *  Bluetooth RFCOMM service, for wireless serial ports.
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
		0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF,
	};

/** Indicates if the common RFCOMM channel list has been initialized by at least one stack. */
static bool RFCOMM_ChannelListValid = false;

/** RFCOMM channel state structure, to retain information about each open channel in the RFCOMM multiplexer. */
static RFCOMM_Channel_t RFCOMM_Channels[RFCOMM_MAX_OPEN_CHANNELS];


static uint8_t RFCOMM_ComputeFCS(const void* FrameStart,
                                 uint8_t Length)
{
	uint8_t* DataLoc = (uint8_t*)FrameStart;
	uint8_t  FCS     = 0xFF;

	/* Calculate new Frame CRC value via the given data bytes and the CRC table */
	while (Length--)
	  FCS = pgm_read_byte(&RFCOMM_CRC8Table[FCS ^ *(DataLoc++)]);

	return ~FCS;
}

static RFCOMM_Channel_t* const RFCOMM_FindChannel(BT_StackConfig_t* const StackState,
                                                  BT_L2CAP_Channel_t* const ACLChannel,
                                                  const uint8_t DLCI)
{
	/* Find corresponding entry in the RFCOMM channel multiplexer state array */
	for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
	{
		RFCOMM_Channel_t* RFCOMMChannel = &RFCOMM_Channels[i];

		/* Filter out closed channel objects */
		if (RFCOMMChannel->State == RFCOMM_Channel_Closed)
		  continue;

		/* If the DLCI matches against the current channel, return the found channel object */
		if ((RFCOMMChannel->DLCI == DLCI) && (RFCOMMChannel->ACLChannel == ACLChannel))
		  return RFCOMMChannel;
	}

	return NULL;
}

static RFCOMM_Channel_t* const RFCOMM_NewChannel(BT_StackConfig_t* const StackState,
                                                 BT_L2CAP_Channel_t* const ACLChannel,
                                                 const uint8_t DLCI)
{
	/* Find a free entry in the RFCOMM channel multiplexer state array */
	for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
	{
		RFCOMM_Channel_t* RFCOMMChannel = &RFCOMM_Channels[i];

		if (RFCOMMChannel->State != RFCOMM_Channel_Closed)
		  continue;

		RFCOMMChannel->Stack                    = StackState;
		RFCOMMChannel->ACLChannel               = ACLChannel;
		RFCOMMChannel->State                    = RFCOMM_Channel_Configure;
		RFCOMMChannel->DLCI                     = DLCI;
		RFCOMMChannel->ConfigFlags              = 0;

		RFCOMMChannel->DataLink.Priority        = (7 + (RFCOMMChannel->DLCI & 0xF8));
		RFCOMMChannel->DataLink.MTU             = 127;
		RFCOMMChannel->DataLink.RemoteSignals   = 0;
		RFCOMMChannel->DataLink.LocalSignals    = (RFCOMM_SIGNAL_RTC | RFCOMM_SIGNAL_RTR | RFCOMM_SIGNAL_DV);

		RFCOMMChannel->PortConfig.BaudRate      = RFCOMM_BaudRate_9600;
		RFCOMMChannel->PortConfig.DataBits      = RFCOMM_DataBits_8;
		RFCOMMChannel->PortConfig.StopBits      = RFCOMM_StopBits_1;
		RFCOMMChannel->PortConfig.ParameterMask = (RFCOMM_RPN_PMASK_BITRATE | RFCOMM_RPN_PMASK_DATABITS | RFCOMM_RPN_PMASK_STOPBITS | RFCOMM_RPN_PMASK_PARITY);
		return RFCOMMChannel;
	}

	return NULL;
}

static bool RFCOMM_SendFrame(BT_StackConfig_t* const StackState,
                             BT_L2CAP_Channel_t* const ACLChannel,
                             const uint8_t DLCI,
                             uint8_t Control,
                             const uint16_t DataLen,
                             const void* Data)
{
	struct
	{
		RFCOMM_Header_t FrameHeader;
		uint8_t         Size[(DataLen < 128) ? 1 : 2];
		uint8_t         Data[DataLen];
		uint8_t         FCS;
	} ATTR_PACKED ResponsePacket;

	/* Abort if invalid ACL connection handle is given */
	if (!(ACLChannel))
	  return false;

	bool CommandResp = ACLChannel->LocallyInitiated;

	if ((Control == RFCOMM_Frame_UA) || (Control == RFCOMM_Frame_DM))
	  CommandResp = !(CommandResp);

	if (Control != RFCOMM_Frame_UIH)
	  Control |= FRAME_POLL_FINAL;

	/* Set the frame header values to the specified address and frame type */
	ResponsePacket.FrameHeader.Control = Control;
	ResponsePacket.FrameHeader.Address = (RFCOMM_Address_t){.DLCI = DLCI, .EA = true, .CR = CommandResp};

	/* Set lower 7 bits of the Size field - LSB reserved for 16-bit field extension */
	ResponsePacket.Size[0] = (DataLen << 1);

	/* If length extension not required, set terminator bit, otherwise add upper length bits */
	if (DataLen < 128)
	  ResponsePacket.Size[0] |= 0x01;
	else
	  ResponsePacket.Size[1]  = (DataLen >> 7);

	/* Copy over the packet data from the source buffer to the response packet buffer */
	memcpy(ResponsePacket.Data, Data, DataLen);

	/* Determine the length of the frame which is to be used to calculate the CRC value */
	uint8_t CRCLength = sizeof(ResponsePacket.FrameHeader);

	/* UIH frames do not have the CRC calculated on the Size field in the response, all other frames do */
	if (Control != RFCOMM_Frame_UIH)
	  CRCLength += sizeof(ResponsePacket.Size);

	/* Calculate the frame checksum from the appropriate fields */
	ResponsePacket.FCS = RFCOMM_ComputeFCS(&ResponsePacket, CRCLength);

	/* Send the completed response packet to the sender */
	return Bluetooth_L2CAP_SendPacket(StackState, ACLChannel, sizeof(ResponsePacket), &ResponsePacket);
}

static bool RFCOMM_SendControlFrame(BT_StackConfig_t* const StackState,
                                    BT_L2CAP_Channel_t* const ACLChannel,
                                    const uint8_t Command,
                                    const bool CR,
                                    const uint16_t DataLen,
                                    const void* Data)
{
	struct
	{
		RFCOMM_Command_t CommandHeader;
		uint8_t          Length;
		uint8_t          Data[DataLen];
	} ATTR_PACKED ResponsePacket;

	/* Fill out the control header and copy over the payload */
	ResponsePacket.CommandHeader = (RFCOMM_Command_t){.Command = Command, .EA = true, .CR = CR};
	ResponsePacket.Length        = ((DataLen << 1) | 0x01);
	memcpy(&ResponsePacket.Data, Data, DataLen);

	/* Send the constructed control frame encapsulated as a UIH frame */
	return RFCOMM_SendFrame(StackState, ACLChannel, RFCOMM_CONTROL_DLCI, RFCOMM_Frame_UIH, sizeof(ResponsePacket), &ResponsePacket);
}

static void RFCOMM_ProcessTestCommand(BT_StackConfig_t* const StackState,
                                      BT_L2CAP_Channel_t* const ACLChannel,
                                      RFCOMM_Command_t* const CommandHeader,
                                      uint8_t* CommandData,
									  const uint16_t CommandDataLen)
{
	/* Send the response to acknowledge the command */
	RFCOMM_SendControlFrame(StackState, ACLChannel, RFCOMM_Control_Test, false, CommandDataLen, &CommandData);
}

static void RFCOMM_ProcessFCECommand(BT_StackConfig_t* const StackState,
                                     BT_L2CAP_Channel_t* const ACLChannel,
                                     RFCOMM_Command_t* const CommandHeader,
                                     uint8_t* CommandData)
{
	// TODO
}

static void RFCOMM_ProcessFCDCommand(BT_StackConfig_t* const StackState,
                                     BT_L2CAP_Channel_t* const ACLChannel,
                                     RFCOMM_Command_t* const CommandHeader,
                                     uint8_t* CommandData)
{
	// TODO
}

static void RFCOMM_ProcessMSCommand(BT_StackConfig_t* const StackState,
                                    BT_L2CAP_Channel_t* const ACLChannel,
                                    RFCOMM_Command_t* const CommandHeader,
                                    uint8_t* CommandData,
                                    const uint16_t CommandDataLen)
{
	RFCOMM_MSC_Parameters_t* MSCParams = (RFCOMM_MSC_Parameters_t*)CommandData;

	/* Ignore status flags sent to the control channel */
	if (MSCParams->Address.DLCI == RFCOMM_CONTROL_DLCI)
	  return;

	/* Find the corresponding entry in the RFCOMM channel list that the data is directed to */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_FindChannel(StackState, ACLChannel, MSCParams->Address.DLCI);

	/* Check if the frame is a command or a response to an issued command */
	if (CommandHeader->CR)
	{
		/* Cound not find entry, send an error frame and abort */
		if (!(RFCOMMChannel))
		{
			RFCOMM_SendFrame(StackState, ACLChannel, MSCParams->Address.DLCI, RFCOMM_Frame_DM, 0, NULL);
			return;
		}

		/* Save the new channel signals to the channel state structure */
		RFCOMMChannel->DataLink.RemoteSignals = (MSCParams->Signals & ~0x01);
		RFCOMMChannel->ConfigFlags |= RFCOMM_CONFIG_REMOTESIGNALS;

		/* Send the MSC response to the remote device (echo sent data to confirm) */
		RFCOMM_SendControlFrame(StackState, ACLChannel, RFCOMM_Control_ModemStatus, false, sizeof(*MSCParams), MSCParams);
	}
	else
	{
		/* Channel not found, abort */
		if (!(RFCOMMChannel))
		  return;

		/* Save the local signals ACK from the remote device */
		RFCOMMChannel->ConfigFlags |= RFCOMM_CONFIG_LOCALSIGNALS;
	}
}

static void RFCOMM_ProcessRPNCommand(BT_StackConfig_t* const StackState,
                                     BT_L2CAP_Channel_t* const ACLChannel,
                                     RFCOMM_Command_t* const CommandHeader,
                                     uint8_t* CommandData,
                                     const uint16_t CommandDataLen)
{
	RFCOMM_RPN_Parameters_t* RPNParams = (RFCOMM_RPN_Parameters_t*)CommandData;

	/* Find the corresponding entry in the RFCOMM channel list that the data is directed to */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_FindChannel(StackState, ACLChannel, RPNParams->Address.DLCI);

	/* Check if the frame is a command or a response to an issued command */
	if (CommandHeader->CR)
	{
		/* Cound not find entry, send an error frame and abort */
		if (!(RFCOMMChannel))
		{
			RFCOMM_SendFrame(StackState, ACLChannel, RPNParams->Address.DLCI, RFCOMM_Frame_DM, 0, NULL);
			return;
		}

		/* If a single byte was sent, it is a request for the current parameters, otherwise it is a request to set the parameters */
		if (CommandDataLen > 1)
		  memcpy(&RFCOMMChannel->PortConfig, &RPNParams->Params, sizeof(RFCOMMChannel->PortConfig));

		/* Send the RPN response to the remote device */
		RFCOMM_SendControlFrame(StackState, ACLChannel, RFCOMM_Control_RemotePortNegotiation, false, sizeof(*RPNParams), RPNParams);
	}
}

static void RFCOMM_ProcessRLSCommand(BT_StackConfig_t* const StackState,
                                     BT_L2CAP_Channel_t* const ACLChannel,
                                     RFCOMM_Command_t* const CommandHeader,
                                     uint8_t* CommandData)
{
	// TODO
}

static void RFCOMM_ProcessPNCommand(BT_StackConfig_t* const StackState,
                                    BT_L2CAP_Channel_t* const ACLChannel,
                                    RFCOMM_Command_t* const CommandHeader,
                                    uint8_t* CommandData)
{
	RFCOMM_PN_Parameters_t* PNParams = (RFCOMM_PN_Parameters_t*)CommandData;

	/* Find the corresponding entry in the RFCOMM channel list that the data is directed to */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_FindChannel(StackState, ACLChannel, PNParams->DLCI);

	/* Check if the frame is a command or a response to an issued command */
	if (CommandHeader->CR)
	{
		/* Existing channel not found, attempt to create a new one */
		if (!(RFCOMMChannel))
		  RFCOMMChannel = RFCOMM_NewChannel(StackState, ACLChannel, PNParams->DLCI);

		/* Cound not create new channel entry, abort */
		if (!(RFCOMMChannel))
		  return;

		/* Save remote requested channel priority and MTU */
		RFCOMMChannel->DataLink.Priority = PNParams->Priority;
		RFCOMMChannel->DataLink.MTU      = PNParams->MaximumFrameSize;

		PNParams->ConvergenceLayer = 0x00; // TODO: Enable credit based transaction support

		/* Send the PN response to the remote device */
		RFCOMM_SendControlFrame(StackState, ACLChannel, RFCOMM_Control_ParameterNegotiation, false, sizeof(*PNParams), PNParams);
	}
	else
	{
		if (!(RFCOMMChannel))
		  return;

		/* Send the PN response to acknowledge the command */
		RFCOMM_SendFrame(StackState, ACLChannel, PNParams->DLCI, RFCOMM_Frame_SABM, 0, NULL);
	}
}

void RFCOMM_ProcessMultiplexerCommand(BT_StackConfig_t* const StackState,
                                      BT_L2CAP_Channel_t* const ACLChannel,
                                      RFCOMM_Command_t* const CommandHeader)
{
	uint8_t* CommandData = (uint8_t*)CommandHeader + sizeof(RFCOMM_Command_t);
	uint16_t CommandDataLen;

	/* Unpack the command data length field - may be 8 or 16 bits depending on LSB */
	if (*CommandData & 0x01)
	{
		CommandDataLen = (*CommandData >> 1);
		CommandData   += 1;
	}
	else
	{
		CommandDataLen = ((*(CommandData + 1) << 7) | (*CommandData >> 1));
		CommandData   += 2;
	}

	switch (CommandHeader->Command)
	{
		case RFCOMM_Control_Test:
			RFCOMM_ProcessTestCommand(StackState, ACLChannel, CommandHeader, CommandData, CommandDataLen);
			break;
		case RFCOMM_Control_FlowControlEnable:
			RFCOMM_ProcessFCECommand(StackState, ACLChannel, CommandHeader, CommandData);
			break;
		case RFCOMM_Control_FlowControlDisable:
			RFCOMM_ProcessFCDCommand(StackState, ACLChannel, CommandHeader, CommandData);
			break;
		case RFCOMM_Control_ModemStatus:
			RFCOMM_ProcessMSCommand(StackState, ACLChannel, CommandHeader, CommandData, CommandDataLen);
			break;
		case RFCOMM_Control_RemotePortNegotiation:
			RFCOMM_ProcessRPNCommand(StackState, ACLChannel, CommandHeader, CommandData, CommandDataLen);
			break;
		case RFCOMM_Control_RemoteLineStatus:
			RFCOMM_ProcessRLSCommand(StackState, ACLChannel, CommandHeader, CommandData);
			break;
		case RFCOMM_Control_ParameterNegotiation:
			RFCOMM_ProcessPNCommand(StackState, ACLChannel, CommandHeader, CommandData);
			break;
		default:
			RFCOMM_SendFrame(StackState, ACLChannel, RFCOMM_CONTROL_DLCI, RFCOMM_Control_NonSupportedCommand, 0, NULL);
			break;
	}
}

static void RFCOMM_ProcessSABM(BT_StackConfig_t* const StackState,
                               BT_L2CAP_Channel_t* const ACLChannel,
                               RFCOMM_Header_t* const FrameHeader)
{
	/* ACK requests to the control channel (start the multiplexer) */
	if (FrameHeader->Address.DLCI == RFCOMM_CONTROL_DLCI)
	{
		RFCOMM_SendFrame(StackState, ACLChannel, RFCOMM_CONTROL_DLCI, RFCOMM_Frame_UA, 0, NULL);
		return;
	}

	/* Find the corresponding entry in the RFCOMM channel list that the data is directed to */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_FindChannel(StackState, ACLChannel, FrameHeader->Address.DLCI);

	/* If an existing channel wasn't found, create a new channel entry */
	if (!(RFCOMMChannel))
	  RFCOMMChannel = RFCOMM_NewChannel(StackState, ACLChannel, FrameHeader->Address.DLCI);

	uint8_t ResponseFrameType = RFCOMM_Frame_DM;

	/* If a channel was found or created sucessfully, set response to ACK the request and indicate channel ABM mode set */
	if (RFCOMMChannel)
	{
		ResponseFrameType = RFCOMM_Frame_UA;
		RFCOMMChannel->ConfigFlags |= RFCOMM_CONFIG_ABMMODESET;
	}

	/* Send the ACK or NAK request based on the success or failure of the channel lookup/creation */
	RFCOMM_SendFrame(StackState, ACLChannel, FrameHeader->Address.DLCI, ResponseFrameType, 0, NULL);
}

static void RFCOMM_ProcessUA(BT_StackConfig_t* const StackState,
                             BT_L2CAP_Channel_t* const ACLChannel,
                             RFCOMM_Header_t* const FrameHeader)
{
	// TODO
}

static void RFCOMM_ProcessDM(BT_StackConfig_t* const StackState,
                             BT_L2CAP_Channel_t* const ACLChannel,
                             RFCOMM_Header_t* const FrameHeader)
{
	/* Find the corresponding entry in the RFCOMM channel list that the data is directed to */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_FindChannel(StackState, ACLChannel, FrameHeader->Address.DLCI);

	/* Cound not find corresponding channel entry, abort */
	if (!(RFCOMMChannel))
	  return;

	/* Mark the disconnected channel as closed in the RFCOMM channel list */
	RFCOMMChannel->State = RFCOMM_Channel_Closed;
}

static void RFCOMM_ProcessDISC(BT_StackConfig_t* const StackState,
                               BT_L2CAP_Channel_t* const ACLChannel,
                               RFCOMM_Header_t* const FrameHeader)
{
	/* ACK requests to the control channel (stop the multiplexer) */
	if (FrameHeader->Address.DLCI == RFCOMM_CONTROL_DLCI)
	{
		/* Terminate all open channels in the multiplexer */
		for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
		{
			RFCOMM_Channel_t* RFCOMMChannel = &RFCOMM_Channels[i];

			if ((RFCOMMChannel->ACLChannel == ACLChannel) && (RFCOMMChannel->State != RFCOMM_Channel_Closed))
			{
				RFCOMMChannel->State = RFCOMM_Channel_Closed;
				EVENT_RFCOMM_ChannelStateChange(StackState, RFCOMMChannel);
			}
		}

		/* ACK the session disconnection request */
		RFCOMM_SendFrame(StackState, ACLChannel, RFCOMM_CONTROL_DLCI, RFCOMM_Frame_UA, 0, NULL);
		return;
	}

	/* Find the corresponding entry in the RFCOMM channel list that the data is directed to */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_FindChannel(StackState, ACLChannel, FrameHeader->Address.DLCI);

	uint8_t ResponseFrameType = RFCOMM_Frame_UA;

	/* Cound not find corresponding channel entry, send disconnected frame */
	if (!(RFCOMMChannel))
	  ResponseFrameType = RFCOMM_Frame_DM;

	/* ACK the disconnection request */
	RFCOMM_SendFrame(StackState, ACLChannel, FrameHeader->Address.DLCI, ResponseFrameType, 0, NULL);

	/* If channel object was found, mark it as closed and notify the user application */
	if (RFCOMMChannel)
	{
		/* Mark the disconnected channel as closed in the RFCOMM channel list */
		RFCOMMChannel->State = RFCOMM_Channel_Closed;
		EVENT_RFCOMM_ChannelStateChange(StackState, RFCOMMChannel);
	}
}

static void RFCOMM_ProcessUIH(BT_StackConfig_t* const StackState,
                              BT_L2CAP_Channel_t* const ACLChannel,
                              RFCOMM_Header_t* const FrameHeader)
{
	uint8_t* FrameData = (uint8_t*)FrameHeader + sizeof(RFCOMM_Header_t);
	uint16_t FrameDataLen;

	/* Unpack the frame data length field - may be 8 or 16 bits depending on LSB */
	if (*FrameData & 0x01)
	{
		FrameDataLen = (*FrameData >> 1);
		FrameData   += 1;
	}
	else
	{
		FrameDataLen = ((*(FrameData + 1) << 7) | (*FrameData >> 1));
		FrameData   += 2;
	}

	/* Check if the data is directed to the control DLCI - if so run command processor */
	if (FrameHeader->Address.DLCI == RFCOMM_CONTROL_DLCI)
	{
		RFCOMM_ProcessMultiplexerCommand(StackState, ACLChannel, (RFCOMM_Command_t*)FrameData);
		return;
	}

	/* Find the corresponding entry in the RFCOMM channel list that the data is directed to */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_FindChannel(StackState, ACLChannel, FrameHeader->Address.DLCI);

	/* Cound not find corresponding channel entry, abort */
	if (!(RFCOMMChannel))
	{
		RFCOMM_SendFrame(StackState, ACLChannel, FrameHeader->Address.DLCI, RFCOMM_Frame_DM, 0, NULL);
		return;
	}

	CALLBACK_RFCOMM_DataReceived(StackState, RFCOMMChannel, FrameDataLen, FrameData);
}

void RFCOMM_Init(BT_StackConfig_t* const StackState)
{
	/* Reset the RFCOMM channel structures, to invalidate any configured RFCOMM channels */
	for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
	{
		if (!(RFCOMM_ChannelListValid) || (RFCOMM_Channels[i].Stack == StackState))
		  RFCOMM_Channels[i].State = RFCOMM_Channel_Closed;
	}

	/* Mark the channel table as having been globally initialized at least once */
	RFCOMM_ChannelListValid = true;

	/* Register the RFCOMM virtual serial port service with the SDP service */
	SDP_RegisterService(&ServiceEntry_RFCOMMSerialPort);
}

void RFCOMM_Manage(BT_StackConfig_t* const StackState)
{
	for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
	{
		RFCOMM_Channel_t* RFCOMMChannel = &RFCOMM_Channels[i];

		/* Ignore channels allocated to different stack instances */
		if (RFCOMMChannel->Stack != StackState)
		  continue;

		if (RFCOMMChannel->State == RFCOMM_Channel_Configure)
		{
			/* Check if the local signals have not yet been sent on the current channel */
			if (!(RFCOMMChannel->ConfigFlags & RFCOMM_CONFIG_LOCALSIGNALSSENT) &&
			    (RFCOMMChannel->ConfigFlags & RFCOMM_CONFIG_ABMMODESET))
			{
				RFCOMM_MSC_Parameters_t MSCommand;
				MSCommand.Address = (RFCOMM_Address_t){.DLCI = RFCOMMChannel->DLCI, .EA = true, .CR = false};
				MSCommand.Signals = (RFCOMMChannel->DataLink.LocalSignals | 0x01);

				/* Send the MSC command to the remote device */
				if (RFCOMM_SendControlFrame(StackState, RFCOMMChannel->ACLChannel, RFCOMM_Control_ModemStatus, true, sizeof(MSCommand), &MSCommand))
				{
					/* Indicate that the local signals have now been sent */
					RFCOMMChannel->ConfigFlags |= RFCOMM_CONFIG_LOCALSIGNALSSENT;
				}
			}

			/* If signals have been configured in both directions, progress to the open state */
			if ((RFCOMMChannel->ConfigFlags & (RFCOMM_CONFIG_REMOTESIGNALS | RFCOMM_CONFIG_LOCALSIGNALS)) ==
			                                  (RFCOMM_CONFIG_REMOTESIGNALS | RFCOMM_CONFIG_LOCALSIGNALS))
			{
				RFCOMMChannel->State = RFCOMM_Channel_Open;
				EVENT_RFCOMM_ChannelStateChange(StackState, RFCOMMChannel);
			}
		}
	}
}

void RFCOMM_ChannelOpened(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel)
{
	/* If the RFCOMM session was initiated by the local device, attempt to reset the remote multiplexer ready for new channels */
	if (Channel->LocallyInitiated)
	  RFCOMM_SendFrame(StackState, Channel, RFCOMM_CONTROL_DLCI, RFCOMM_Frame_SABM, 0, NULL);
}

void RFCOMM_ChannelClosed(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel)
{
	for (uint8_t i = 0; i < RFCOMM_MAX_OPEN_CHANNELS; i++)
	{
		RFCOMM_Channel_t* RFCOMMChannel = &RFCOMM_Channels[i];

		if (RFCOMMChannel->ACLChannel == Channel)
		{
			RFCOMMChannel->State = RFCOMM_Channel_Closed;
			EVENT_RFCOMM_ChannelStateChange(StackState, RFCOMMChannel);
		}
	}
}

void RFCOMM_ProcessPacket(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel,
                          const uint16_t Length,
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
			if (FrameHeader->Control & FRAME_POLL_FINAL)
			  RFCOMM_ProcessSABM(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_UA:
			if (FrameHeader->Control & FRAME_POLL_FINAL)
			  RFCOMM_ProcessUA(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_DM:
			RFCOMM_ProcessDM(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_DISC:
			if (FrameHeader->Control & FRAME_POLL_FINAL)
			  RFCOMM_ProcessDISC(StackState, Channel, FrameHeader);
			break;
		case RFCOMM_Frame_UIH:
			RFCOMM_ProcessUIH(StackState, Channel, FrameHeader);
			break;
	}
}

bool RFCOMM_SendData(RFCOMM_Channel_t* const RFCOMMChannel,
                     const uint16_t DataLen,
                     const void* Data)
{
	if (!(RFCOMMChannel) || (RFCOMMChannel->State != RFCOMM_Channel_Open))
	  return false;

	/* Send the channel data to the remote device */
	return RFCOMM_SendFrame(RFCOMMChannel->Stack, RFCOMMChannel->ACLChannel, RFCOMMChannel->DLCI, RFCOMM_Frame_UIH, DataLen, Data);
}
