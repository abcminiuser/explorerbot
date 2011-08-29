/*
            Bluetooth Stack
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  All rights reserved.
*/

#include "BluetoothACL.h"

BT_ACL_Channel_t* Bluetooth_ACL_FindChannel(BT_StackConfig_t* const StackState,
                                            const uint16_t ConnectionHandle,
                                            const uint16_t LocalChannel,
                                            const uint16_t RemoteChannel)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_ACL_Channel_t* CurrentChannel = &StackState->State.ACL.Channels[i];

		// Skip over closed (unused) channel entries and entries with the incorrect connection handle
		if ((CurrentChannel->State == ACL_CHANSTATE_Closed) ||
		    (CurrentChannel->ConnectionHandle != ConnectionHandle))
		{
			continue;
		}
		
		// If local channel number specified, check for match
		if ((LocalChannel) && (StackState->State.ACL.Channels[i].LocalNumber == LocalChannel))
		  return CurrentChannel;

		// If remote channel number specified, check for match
		if ((RemoteChannel) && (StackState->State.ACL.Channels[i].RemoteNumber == RemoteChannel))
		  return CurrentChannel;
	}
	
	return NULL;
}

static BT_ACL_Channel_t* Bluetooth_ACL_NewChannel(BT_StackConfig_t* const StackState,
                                                  const uint16_t ConnectionHandle,
                                                  const uint16_t RemoteChannel,
                                                  const uint16_t PSM)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_ACL_Channel_t* CurrentChannel = &StackState->State.ACL.Channels[i];

		if (CurrentChannel->State != ACL_CHANSTATE_Closed)
		  continue;
		  
		CurrentChannel->LocalNumber  = cpu_to_le16(BT_CHANNEL_BASEOFFSET + i);
		CurrentChannel->RemoteNumber = RemoteChannel;
		CurrentChannel->LocalMTU     = DEFAULT_ACL_CHANNEL_MTU;
		CurrentChannel->State        = ACL_CHANSTATE_New;
		CurrentChannel->PSM          = PSM;
		return CurrentChannel;
	}
	
	return NULL;
}

static bool Bluetooth_ACL_SendSignal(BT_StackConfig_t* const StackState,
                                     BT_HCI_Connection_t* HCIConnection,
                                     uint16_t Length,
                                     void* Data)
{
	BT_ACL_Channel_t SignalChannel;
	SignalChannel.ConnectionHandle = HCIConnection->Handle;
	SignalChannel.State            = ACL_CHANSTATE_Open;
	SignalChannel.LocalNumber      = BT_CHANNEL_SIGNALING;
	SignalChannel.RemoteNumber     = BT_CHANNEL_SIGNALING;

	return Bluetooth_ACL_Send(StackState, &SignalChannel, Length, Data);
}

static void Bluetooth_ACL_Signal_ConnRequest(BT_StackConfig_t* const StackState,
                                             BT_HCI_Connection_t* const HCIConnection,
                                             BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConnectionReq_t* ConnectionRequest = (BT_Signal_ConnectionReq_t*)SignalCommandHeader->Data;
				
	// Create a new channel from the given connection request data
	BT_ACL_Channel_t* ACLChannel      = Bluetooth_ACL_NewChannel(StackState, HCIConnection->Handle, ConnectionRequest->SourceChannel, ConnectionRequest->PSM);
	uint8_t           RejectionReason = BT_CONNECTION_REFUSED_RESOURCES;
	
	if (ACLChannel)
	{
		RejectionReason   = CALLBACK_Bluetooth_ChannelRequest(StackState, HCIConnection, ACLChannel) ? BT_CONNECTION_SUCCESSFUL : BT_CONNECTION_REFUSED_PSM;
		ACLChannel->State = (RejectionReason == BT_CONNECTION_SUCCESSFUL) ? ACL_CHANSTATE_Config_WaitConfig : ACL_CHANSTATE_Closed;
	}
	
	// Construct a response packet container
	struct
	{
		BT_Signal_Header_t         SignalCommandHeader;
		BT_Signal_ConnectionResp_t ConnectionResponse;
	} ResponsePacket;

	// Fill out the Signal Command header in the response packet
	ResponsePacket.SignalCommandHeader.Code              = BT_SIGNAL_CONNECTION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier        = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length            = CPU_TO_LE16(sizeof(ResponsePacket.ConnectionResponse));

	// Fill out the Connection Response in the response packet
	ResponsePacket.ConnectionResponse.DestinationChannel = ACLChannel->LocalNumber;
	ResponsePacket.ConnectionResponse.SourceChannel      = ACLChannel->RemoteNumber;
	ResponsePacket.ConnectionResponse.Result             = RejectionReason;
	ResponsePacket.ConnectionResponse.Status             = 0x00;
	
	Bluetooth_ACL_SendSignal(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static void Bluetooth_ACL_Signal_ConnResp(BT_StackConfig_t* const StackState,
                                          BT_HCI_Connection_t* const HCIConnection,
                                          BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConnectionResp_t* ConnectionResponse = (BT_Signal_ConnectionResp_t*)SignalCommandHeader->Data;
		
	// Find the existing channel in the channel table if it exists
	BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_FindChannel(StackState, HCIConnection->Handle, 0, ConnectionResponse->SourceChannel);

	if (ACLChannel)
	{
		ACLChannel->RemoteNumber = ConnectionResponse->SourceChannel;
		ACLChannel->State        = (ConnectionResponse->Result == BT_CONNECTION_SUCCESSFUL) ?
									ACL_CHANSTATE_Config_WaitConfig : ACL_CHANSTATE_Closed;
	}
}

static void Bluetooth_ACL_Signal_EchoReq(BT_StackConfig_t* const StackState,
                                         BT_HCI_Connection_t* const HCIConnection,
                                         BT_Signal_Header_t* const SignalCommandHeader)
{
	struct
	{
		BT_Signal_Header_t SignalCommandHeader;
	} ResponsePacket;

	// Fill out the Signal Command header in the response packet
	ResponsePacket.SignalCommandHeader.Code       = BT_SIGNAL_ECHO_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length     = CPU_TO_LE16(0);

	Bluetooth_ACL_SendSignal(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static void Bluetooth_ACL_Signal_DisconnectionReq(BT_StackConfig_t* const StackState,
                                                  BT_HCI_Connection_t* const HCIConnection,
                                                  BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_DisconnectionReq_t* DisconnectionRequest = (BT_Signal_DisconnectionReq_t*)SignalCommandHeader->Data;

	// Find the existing channel in the channel table if it exists
	BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_FindChannel(StackState, HCIConnection->Handle, 0, DisconnectionRequest->SourceChannel);
	
	struct
	{
		BT_Signal_Header_t            SignalCommandHeader;
		BT_Signal_DisconnectionResp_t DisconnectionResponse;
	} ResponsePacket;

	// Fill out the Signal Command header in the response packet
	ResponsePacket.SignalCommandHeader.Code       = BT_SIGNAL_DISCONNECTION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length     = CPU_TO_LE16(sizeof(ResponsePacket.DisconnectionResponse));

	// Fill out the Disconnection Response in the response packet
	ResponsePacket.DisconnectionResponse.DestinationChannel = ACLChannel->RemoteNumber;
	ResponsePacket.DisconnectionResponse.SourceChannel      = ACLChannel->LocalNumber;

	if (ACLChannel)
	  ACLChannel->State = ACL_CHANSTATE_Closed;

	Bluetooth_ACL_SendSignal(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static void Bluetooth_ACL_Signal_DisconnectionResp(BT_StackConfig_t* const StackState,
                                                   BT_HCI_Connection_t* const HCIConnection,
                                                   BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_DisconnectionResp_t* DisconnectionResponse = (BT_Signal_DisconnectionResp_t*)SignalCommandHeader->Data;

	// Find the existing channel in the channel table if it exists
	BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_FindChannel(StackState, HCIConnection->Handle, 0, DisconnectionResponse->SourceChannel);
	
	if (ACLChannel)
	  ACLChannel->State = ACL_CHANSTATE_Closed;
}

static void Bluetooth_ACL_Signal_InformationReq(BT_StackConfig_t* const StackState,
                                                BT_HCI_Connection_t* const HCIConnection,
                                                BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_InformationReq_t* InformationRequest = (BT_Signal_InformationReq_t*)SignalCommandHeader->Data;

	struct
	{
		BT_Signal_Header_t          SignalCommandHeader;
		BT_Signal_InformationResp_t InformationResponse;

		uint8_t Data[4];
	} ResponsePacket;

	uint8_t DataLen = 0;

	// Retrieve the requested information and store it in the outgoing packet, if found
	switch (InformationRequest->InfoType)
	{
		case BT_INFOREQ_MTU:
			ResponsePacket.InformationResponse.Result = BT_INFORMATION_SUCCESSFUL;
			DataLen = 2;

			*((uint16_t*)&ResponsePacket.Data) = CPU_TO_LE16(DEFAULT_ACL_CHANNEL_MTU);
			break;
		case BT_INFOREQ_EXTENDEDFEATURES:
			ResponsePacket.InformationResponse.Result = BT_INFORMATION_SUCCESSFUL;
			DataLen = 4;

			*((uint32_t*)&ResponsePacket.Data) = CPU_TO_LE32(0);
			break;
		default:
			ResponsePacket.InformationResponse.Result = BT_INFORMATION_NOTSUPPORTED;
			DataLen = 0;
			break;
	}

	/* Fill out the Signal Command header in the response packet */
	ResponsePacket.SignalCommandHeader.Code       = BT_SIGNAL_INFORMATION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length     = cpu_to_le16(sizeof(ResponsePacket.InformationResponse) + DataLen);

	/* Fill out the Information Response in the response packet */
	ResponsePacket.InformationResponse.InfoType = InformationRequest->InfoType;

	Bluetooth_ACL_SendSignal(StackState, NULL, (sizeof(ResponsePacket) - sizeof(ResponsePacket.Data) + DataLen), &ResponsePacket);
}

/** Initalizes the Bluetooth ACL layer, ready for new channel connections.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_ACL_Init(BT_StackConfig_t* const StackState)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	  StackState->State.ACL.Channels[i].State = ACL_CHANSTATE_Closed;
}

/** Processes a recieved Bluetooth ACL packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_ACL_ProcessPacket(BT_StackConfig_t* const StackState)
{
	BT_ACL_Header_t*        ACLPacketHeader = (BT_ACL_Header_t*)StackState->Config.PacketBuffer;
	BT_DataPacket_Header_t* ACLDataHeader   = (BT_DataPacket_Header_t*)ACLPacketHeader->Data;
	
	// Find the associated HCI connection via the connection handle indicated in the ACL packet header
	BT_HCI_Connection_t*    HCIConnection   = Bluetooth_HCI_FindConnection(StackState, NULL, le16_to_cpu(ACLPacketHeader->ConnectionHandle));
	
	// Connection not found, abort processing of packet
	if (!(HCIConnection))
	  return;

	// Check if signalling (control) packet data, or user application connection data
	if (ACLDataHeader->DestinationChannel == BT_CHANNEL_SIGNALING)
	{
		BT_Signal_Header_t* SignalCommandHeader = (BT_Signal_Header_t*)ACLDataHeader->Payload;

		switch (SignalCommandHeader->Code)
		{
			case BT_SIGNAL_CONNECTION_REQUEST:
				LCD_Clear();
				LCD_WriteString("CON REQ");
				Bluetooth_ACL_Signal_ConnRequest(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONNECTION_RESPONSE:
				LCD_Clear();
				LCD_WriteString("CON RESP");
				Bluetooth_ACL_Signal_ConnResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONFIGURATION_REQUEST:
				LCD_Clear();
				LCD_WriteString("CONFIG REQ");
				break;
			case BT_SIGNAL_CONFIGURATION_RESPONSE:
				LCD_Clear();
				LCD_WriteString("CONFIG RESP");
				break;
			case BT_SIGNAL_DISCONNECTION_REQUEST:
				LCD_Clear();
				LCD_WriteString("DISCON REQ");
				Bluetooth_ACL_Signal_DisconnectionReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_DISCONNECTION_RESPONSE:
				LCD_Clear();
				LCD_WriteString("DISCON RESP");
				Bluetooth_ACL_Signal_DisconnectionResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_ECHO_REQUEST:
				LCD_Clear();
				LCD_WriteString("ECHO REQ");
				Bluetooth_ACL_Signal_EchoReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_INFORMATION_REQUEST:
				LCD_Clear();
				LCD_WriteString("INFO REQ");
				Bluetooth_ACL_Signal_InformationReq(StackState, HCIConnection, SignalCommandHeader);
				break;
		}
	}
	else
	{
		// Find the associated ACL channel via the connection handle and destination channel
		BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_FindChannel(StackState, HCIConnection->Handle, ACLDataHeader->DestinationChannel, 0);
		
		if (ACLChannel)
		  EVENT_Bluetooth_DataReceived(StackState, HCIConnection, ACLChannel, ACLDataHeader->PayloadLength, ACLDataHeader->Payload);
	}
}

/** Manages the existing ACL layer connections of a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *
 *  \return Boolean \c true if more ACL management tasks are pending, \c false otherwise.
 */
bool Bluetooth_ACL_Manage(BT_StackConfig_t* const StackState)
{
	return false;
}

bool Bluetooth_ACL_Send(BT_StackConfig_t* const StackState,
                        BT_ACL_Channel_t* const ACLChannel,
                        uint16_t Length,
                        void* Data)
{
	/* If the destination channel is not the signaling channel and it is not currently fully open, abort */
	if (ACLChannel->State != ACL_CHANSTATE_Open)
	  return false;

	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, ACLChannel->ConnectionHandle);

	/* If an open device connection with the correct connection handle was not foumd, abort */
	if (!(HCIConnection))
	  return false;
	
	// TODO: Respect MTU
	
	BT_ACL_Header_t* ACLPacketHeader  = (BT_ACL_Header_t*)StackState->Config.PacketBuffer;
	ACLPacketHeader->ConnectionHandle = cpu_to_le16(HCIConnection->Handle | BT_ACL_FIRST_AUTOFLUSH);
	ACLPacketHeader->DataLength       = cpu_to_le16(sizeof(BT_DataPacket_Header_t) + Length);

	BT_DataPacket_Header_t* ACLDataHeader = (BT_DataPacket_Header_t*)ACLPacketHeader->Data;
	ACLDataHeader->PayloadLength      = cpu_to_le16(Length);
	ACLDataHeader->DestinationChannel = ACLChannel->RemoteNumber;

	memcpy(ACLDataHeader->Payload, Data, Length);
	
	CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_ACLData, (sizeof(BT_ACL_Header_t) + sizeof(BT_DataPacket_Header_t) + Length));
	return true;
}

