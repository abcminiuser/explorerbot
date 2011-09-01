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

static void Bluetooth_SDP_ServiceSearch(BT_StackConfig_t* const StackState,
                                        BT_ACL_Channel_t* const Channel,
                                        BT_SDP_PDUHeader_t* const SDPHeader)
{
	LCD_Clear();
	LCD_WriteString("SDP - SS");
}

static void Bluetooth_SDP_ServiceAttribute(BT_StackConfig_t* const StackState,
                                           BT_ACL_Channel_t* const Channel,
                                           BT_SDP_PDUHeader_t* const SDPHeader)
{
	LCD_Clear();
	LCD_WriteString("SDP - SA");
}

static void Bluetooth_SDP_ServiceSearchAttribute(BT_StackConfig_t* const StackState,
                                                 BT_ACL_Channel_t* const Channel,
                                                 BT_SDP_PDUHeader_t* const SDPHeader)
{
	LCD_Clear();
	LCD_WriteString("SDP - SSA");

	// TODO
#if 0
	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICESEARCHATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(ParamLength);

	/* Send the completed response packet to the sender */
	Bluetooth_ACL_SendPacket(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + ParamLength), &ResponsePacket);
#endif
}

void Bluetooth_SDP_ProcessPacket(BT_StackConfig_t* const StackState,
                                 BT_HCI_Connection_t* const Connection,
                                 BT_ACL_Channel_t* const Channel,
                                 uint16_t Length,
                                 uint8_t* Data)
{
	BT_SDP_PDUHeader_t* SDPHeader = (BT_SDP_PDUHeader_t*)Data;
	
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