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

#include "BluetoothSDP.h"

// TODO: FIXME
#include "../../../LCD.h"

void Bluetooth_SDP_Init(BT_StackConfig_t* const StackState)
{

}

void Bluetooth_SDP_Manage(BT_StackConfig_t* const StackState)
{

}

static void Bluetooth_SDP_ServiceSearch(BT_StackConfig_t* const StackState,
                                        BT_L2CAP_Channel_t* const Channel,
                                        BT_SDP_PDUHeader_t* const SDPHeader)
{
	LCD_Clear();
	LCD_WriteString("SDP - SS");
}

static void Bluetooth_SDP_ServiceAttribute(BT_StackConfig_t* const StackState,
                                           BT_L2CAP_Channel_t* const Channel,
                                           BT_SDP_PDUHeader_t* const SDPHeader)
{
	LCD_Clear();
	LCD_WriteString("SDP - SA");
}

static void Bluetooth_SDP_ServiceSearchAttribute(BT_StackConfig_t* const StackState,
                                                 BT_L2CAP_Channel_t* const Channel,
                                                 BT_SDP_PDUHeader_t* const SDPHeader)
{
	LCD_Clear();
	LCD_WriteString("SDP - SSA");
	
	uint8_t* CurrParameter = SDPHeader->Parameters;



	struct
	{
		BT_SDP_PDUHeader_t SDPHeader;
		uint16_t           AttributeListByteCount;
		uint8_t            ResponseData[100];
	} ResponsePacket;

	/* Create a pointer to the buffer to indicate the current location for response data to be added */
	void* CurrResponsePos = ResponsePacket.ResponseData;
	
	/* Add the outer Data Element Sequence header for all of the retrieved Attributes */
	uint16_t* TotalResponseSize = SDP_AddSequence16(&CurrResponsePos);
	
	/* Continuation state - always zero */
	SDP_WriteData8(&CurrResponsePos, 0);

	/* Set the total response list size to the size of the outer container plus its header size and continuation state */
	ResponsePacket.AttributeListByteCount    = SwapEndian_16(3 + *TotalResponseSize);

	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICESEARCHATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(sizeof(ResponsePacket.AttributeListByteCount) + ResponsePacket.AttributeListByteCount);
	
	/* Must swap the endianness of the attribute byte count before sending it */
	ResponsePacket.AttributeListByteCount    = cpu_to_be16(ResponsePacket.AttributeListByteCount);

	/* Send the completed response packet to the sender */
	Bluetooth_L2CAP_SendPacket(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + be16_to_cpu(ResponsePacket.SDPHeader.ParameterLength)), &ResponsePacket);
}

void Bluetooth_SDP_ChannelOpened(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel)
{
	
}

void Bluetooth_SDP_ChannelClosed(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel)
{
	
}

void Bluetooth_SDP_ProcessPacket(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel,
                                 uint16_t Length,
                                 uint8_t* Data)
{
	BT_SDP_PDUHeader_t* SDPHeader = (BT_SDP_PDUHeader_t*)Data;
	
	/* Ensure correct channel PSM before processing the data */
	if (Channel->PSM != CHANNEL_PSM_SDP)
	  return;
	
	/* Dispatch to the correct processing routine for the given SDP packet type */
	switch (SDPHeader->PDU)
	{
		case SDP_PDU_SERVICESEARCHREQUEST:
			Bluetooth_SDP_ServiceSearch(StackState, Channel, SDPHeader);
			break;
		case SDP_PDU_SERVICEATTRIBUTEREQUEST:
			Bluetooth_SDP_ServiceAttribute(StackState, Channel, SDPHeader);
			break;
		case SDP_PDU_SERVICESEARCHATTRIBUTEREQUEST:
			Bluetooth_SDP_ServiceSearchAttribute(StackState, Channel, SDPHeader);
			break;
	}
}