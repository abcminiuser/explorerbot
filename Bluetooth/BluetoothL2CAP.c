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

/** \file
 *
 *  Bluetooth L2CAP layer management code. This module managed the creation,
 *  configuration and teardown of L2CAP channels, and manages packet reception
 *  and sending to and from other Bluetooth devices.
 */

#include "BluetoothL2CAP.h"

void Bluetooth_L2CAP_NotifyHCIDisconnection(BT_StackConfig_t* const StackState,
                                            const uint16_t ConnectionHandle)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];
		
		/* Clear any open connections that are associated with the destroyed connection handle */
		if (CurrentChannel->ConnectionHandle == ConnectionHandle)
		{
			/* Notify the user of the closed connection if it is currently open */
			if (CurrentChannel->State == L2CAP_CHANSTATE_Open)
			  EVENT_Bluetooth_ChannelClosed(StackState, CurrentChannel);

			CurrentChannel->State = L2CAP_CHANSTATE_Closed;
		}
	}
}

BT_L2CAP_Channel_t* const Bluetooth_L2CAP_FindChannel(BT_StackConfig_t* const StackState,
                                                      const uint16_t ConnectionHandle,
                                                      const uint16_t LocalChannel,
                                                      const uint16_t RemoteChannel)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];

		/* Skip over closed (unused) channel entries and entries with the incorrect connection handle */
		if ((CurrentChannel->State == L2CAP_CHANSTATE_Closed) ||
		    (CurrentChannel->ConnectionHandle != ConnectionHandle))
		{
			continue;
		}
		
		/* If local channel number specified, check for match */
		if ((LocalChannel) && (StackState->State.L2CAP.Channels[i].LocalNumber == LocalChannel))
		  return CurrentChannel;

		/* If remote channel number specified, check for match */
		if ((RemoteChannel) && (StackState->State.L2CAP.Channels[i].RemoteNumber == RemoteChannel))
		  return CurrentChannel;
	}
	
	return NULL;
}

static BT_L2CAP_Channel_t* const Bluetooth_L2CAP_NewChannel(BT_StackConfig_t* const StackState,
                                                            const uint16_t ConnectionHandle,
                                                            const uint16_t RemoteChannel,
                                                            const uint16_t PSM)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];

		if (CurrentChannel->State != L2CAP_CHANSTATE_Closed)
		  continue;

		CurrentChannel->ConnectionHandle = ConnectionHandle;
		CurrentChannel->LocalNumber      = (BT_CHANNEL_BASEOFFSET + i);
		CurrentChannel->RemoteNumber     = RemoteChannel;
		CurrentChannel->LocalMTU         = DEFAULT_L2CAP_CHANNEL_MTU;
		CurrentChannel->State            = L2CAP_CHANSTATE_New;
		CurrentChannel->PSM              = PSM;
		return CurrentChannel;
	}
	
	return NULL;
}

static bool Bluetooth_L2CAP_SendSignalPacket(BT_StackConfig_t* const StackState,
                                             BT_HCI_Connection_t* HCIConnection,
                                             uint16_t Length,
                                             void* Data)
{
	/* Construct a temporary channel object with the signalling channel indexes */
	BT_L2CAP_Channel_t SignalChannel;
	SignalChannel.ConnectionHandle = cpu_to_le16(HCIConnection->Handle);
	SignalChannel.State            = L2CAP_CHANSTATE_Open;
	SignalChannel.LocalNumber      = CPU_TO_LE16(BT_CHANNEL_SIGNALING);
	SignalChannel.RemoteNumber     = CPU_TO_LE16(BT_CHANNEL_SIGNALING);

	return Bluetooth_L2CAP_SendPacket(StackState, &SignalChannel, Length, Data);
}

static inline void Bluetooth_L2CAP_Signal_ConnRequest(BT_StackConfig_t* const StackState,
                                                      BT_HCI_Connection_t* const HCIConnection,
                                                      BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConnectionReq_t* ConnectionRequest = (BT_Signal_ConnectionReq_t*)SignalCommandHeader->Data;
				
	/* Create a new channel from the given connection request data */
	BT_L2CAP_Channel_t* L2CAPChannel      = Bluetooth_L2CAP_NewChannel(StackState, HCIConnection->Handle, le16_to_cpu(ConnectionRequest->SourceChannel), le16_to_cpu(ConnectionRequest->PSM));
	uint8_t           RejectionReason = BT_CONNECTION_REFUSED_RESOURCES;
	
	/* If there was space in the channel table, request action from the user to accept/reject the connection */
	if (L2CAPChannel)
	{
		RejectionReason   = CALLBACK_Bluetooth_ChannelRequest(StackState, HCIConnection, L2CAPChannel) ? BT_CONNECTION_SUCCESSFUL : BT_CONNECTION_REFUSED_PSM;
		L2CAPChannel->State = (RejectionReason == BT_CONNECTION_SUCCESSFUL) ? L2CAP_CHANSTATE_Config_WaitConfig : L2CAP_CHANSTATE_Closed;
	}
	
	/* Construct a response packet container */
	struct
	{
		BT_Signal_Header_t         SignalCommandHeader;
		BT_Signal_ConnectionResp_t ConnectionResponse;
	} ResponsePacket;

	/*  Fill out the Signal Command header in the response packet */
	ResponsePacket.SignalCommandHeader.Code              = BT_SIGNAL_CONNECTION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier        = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length            = CPU_TO_LE16(sizeof(ResponsePacket.ConnectionResponse));

	/* Fill out the Connection Response in the response packet */
	ResponsePacket.ConnectionResponse.DestinationChannel = cpu_to_le16(L2CAPChannel->LocalNumber);
	ResponsePacket.ConnectionResponse.SourceChannel      = cpu_to_le16(L2CAPChannel->RemoteNumber);
	ResponsePacket.ConnectionResponse.Result             = cpu_to_le16(RejectionReason);
	ResponsePacket.ConnectionResponse.Status             = 0x00;
	
	Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_L2CAP_Signal_ConnResp(BT_StackConfig_t* const StackState,
                                                   BT_HCI_Connection_t* const HCIConnection,
                                                   BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConnectionResp_t* ConnectionResponse = (BT_Signal_ConnectionResp_t*)SignalCommandHeader->Data;
		
	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, 0, le16_to_cpu(ConnectionResponse->SourceChannel));

	if (L2CAPChannel)
	{
		L2CAPChannel->RemoteNumber = le16_to_cpu(ConnectionResponse->SourceChannel);
		L2CAPChannel->State        = (ConnectionResponse->Result == CPU_TO_LE16(BT_CONNECTION_SUCCESSFUL)) ?
									L2CAP_CHANSTATE_Config_WaitConfig : L2CAP_CHANSTATE_Closed;
	}
}

static inline void Bluetooth_L2CAP_Signal_EchoReq(BT_StackConfig_t* const StackState,
                                                  BT_HCI_Connection_t* const HCIConnection,
                                                  BT_Signal_Header_t* const SignalCommandHeader)
{
	struct
	{
		BT_Signal_Header_t SignalCommandHeader;
	} ResponsePacket;

	/* Fill out the Signal Command header in the response packet */
	ResponsePacket.SignalCommandHeader.Code       = BT_SIGNAL_ECHO_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length     = CPU_TO_LE16(0);

	Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_L2CAP_Signal_DisconnectionReq(BT_StackConfig_t* const StackState,
                                                         BT_HCI_Connection_t* const HCIConnection,
                                                         BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_DisconnectionReq_t* DisconnectionRequest = (BT_Signal_DisconnectionReq_t*)SignalCommandHeader->Data;

	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, 0, le16_to_cpu(DisconnectionRequest->SourceChannel));
	
	struct
	{
		BT_Signal_Header_t            SignalCommandHeader;
		BT_Signal_DisconnectionResp_t DisconnectionResponse;
	} ResponsePacket;

	/* Fill out the Signal Command header in the response packet */
	ResponsePacket.SignalCommandHeader.Code       = BT_SIGNAL_DISCONNECTION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length     = CPU_TO_LE16(sizeof(ResponsePacket.DisconnectionResponse));

	/* Fill out the Disconnection Response in the response packet */
	ResponsePacket.DisconnectionResponse.DestinationChannel = cpu_to_le16(L2CAPChannel->RemoteNumber);
	ResponsePacket.DisconnectionResponse.SourceChannel      = cpu_to_le16(L2CAPChannel->LocalNumber);

	/* If a valid connection was found, close it and notify the user */
	if (L2CAPChannel)
	{
		L2CAPChannel->State = L2CAP_CHANSTATE_Closed;
		EVENT_Bluetooth_ChannelClosed(StackState, L2CAPChannel);
	}

	Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_L2CAP_Signal_DisconnectionResp(BT_StackConfig_t* const StackState,
                                                            BT_HCI_Connection_t* const HCIConnection,
                                                            BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_DisconnectionResp_t* DisconnectionResponse = (BT_Signal_DisconnectionResp_t*)SignalCommandHeader->Data;

	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, 0, le16_to_cpu(DisconnectionResponse->SourceChannel));
	
	/* If a valid connection was found, close it and notify the user */
	if (L2CAPChannel)
	{
		L2CAPChannel->State = L2CAP_CHANSTATE_Closed;
		EVENT_Bluetooth_ChannelClosed(StackState, L2CAPChannel);
	}
}

static inline void Bluetooth_L2CAP_Signal_InformationReq(BT_StackConfig_t* const StackState,
                                                         BT_HCI_Connection_t* const HCIConnection,
                                                         BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_InformationReq_t* InformationRequest = (BT_Signal_InformationReq_t*)SignalCommandHeader->Data;

	struct
	{
		BT_Signal_Header_t          SignalCommandHeader;
		BT_Signal_InformationResp_t InformationResponse;
		uint8_t                     Data[4];
	} ResponsePacket;

	uint8_t DataLen = 0;

	/* Retrieve the requested information and store it in the outgoing packet, if found */
	switch (le16_to_cpu(InformationRequest->InfoType))
	{
		case BT_INFOREQ_MTU:
			ResponsePacket.InformationResponse.Result = CPU_TO_LE16(BT_INFORMATION_SUCCESSFUL);
			DataLen = 2;

			*((uint16_t*)&ResponsePacket.Data) = CPU_TO_LE16(DEFAULT_L2CAP_CHANNEL_MTU);
			break;
		case BT_INFOREQ_EXTENDEDFEATURES:
			ResponsePacket.InformationResponse.Result = CPU_TO_LE16(BT_INFORMATION_SUCCESSFUL);
			DataLen = 4;

			*((uint32_t*)&ResponsePacket.Data) = CPU_TO_LE32(0);
			break;
		default:
			ResponsePacket.InformationResponse.Result = CPU_TO_LE16(BT_INFORMATION_NOTSUPPORTED);
			break;
	}

	/* Fill out the Signal Command header in the response packet */
	ResponsePacket.SignalCommandHeader.Code       = BT_SIGNAL_INFORMATION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length     = cpu_to_le16(sizeof(ResponsePacket.InformationResponse) + DataLen);

	/* Fill out the Information Response in the response packet */
	ResponsePacket.InformationResponse.InfoType = InformationRequest->InfoType;

	Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, (sizeof(ResponsePacket) - sizeof(ResponsePacket.Data) + DataLen), &ResponsePacket);
}

static inline void Bluetooth_L2CAP_Signal_ConfigReq(BT_StackConfig_t* const StackState,
                                                    BT_HCI_Connection_t* const HCIConnection,
                                                    BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConfigurationReq_t* ConfigurationRequest = (BT_Signal_ConfigurationReq_t*)SignalCommandHeader->Data;

	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, le16_to_cpu(ConfigurationRequest->DestinationChannel), 0);

	/* Only look at the channel configuration options if a valid channel entry for the local channel number was found */
	if (L2CAPChannel)
	{
		/* Update channel state based on the current state */
		switch (L2CAPChannel->State)
		{
			case L2CAP_CHANSTATE_Config_WaitConfig:
				L2CAPChannel->State = L2CAP_CHANSTATE_Config_WaitSendConfig;
				break;
			case L2CAP_CHANSTATE_Config_WaitReqResp:
				L2CAPChannel->State = L2CAP_CHANSTATE_Config_WaitResp;
				break;
			case L2CAP_CHANSTATE_Config_WaitReq:
				L2CAPChannel->State = L2CAP_CHANSTATE_Open;
				EVENT_Bluetooth_ChannelOpened(StackState, L2CAPChannel);
				break;
		}
		
		uint8_t* OptionsPos = (uint8_t*)ConfigurationRequest->Options;
		uint8_t* OptionsEnd = (uint8_t*)((intptr_t)OptionsPos + (SignalCommandHeader->Length - sizeof(ConfigurationRequest)));

		while (OptionsPos < OptionsEnd)
		{
			BT_Config_Option_Header_t* OptionHeader = (BT_Config_Option_Header_t*)OptionsPos;
			
			/* Store the remote MTU option's value if present */
			if (OptionHeader->Type == BT_CONFIG_OPTION_MTU)
			  L2CAPChannel->RemoteMTU = le16_to_cpu(*((uint16_t*)OptionHeader->Data));

			/* Progress to the next option in the packet */
			OptionsPos += (sizeof(BT_Config_Option_Header_t) + OptionHeader->Length);			
		}
	}

	struct
	{
		BT_Signal_Header_t            SignalCommandHeader;
		BT_Signal_ConfigurationResp_t ConfigurationResponse;
	} ResponsePacket;

	/* Fill out the Signal Command header in the response packet */
	ResponsePacket.SignalCommandHeader.Code              = BT_SIGNAL_CONFIGURATION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier        = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length            = CPU_TO_LE16(sizeof(ResponsePacket.ConfigurationResponse));

	/* Fill out the Configuration Response in the response packet */
	ResponsePacket.ConfigurationResponse.SourceChannel   = cpu_to_le16(L2CAPChannel->RemoteNumber);
	ResponsePacket.ConfigurationResponse.Flags           = 0x00;
	ResponsePacket.ConfigurationResponse.Result          = (L2CAPChannel != NULL) ? CPU_TO_LE16(BT_CONFIGURATION_SUCCESSFUL) : CPU_TO_LE16(BT_CONFIGURATION_REJECTED);

	Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_L2CAP_Signal_ConfigResp(BT_StackConfig_t* const StackState,
                                                     BT_HCI_Connection_t* const HCIConnection,
                                                     BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConfigurationResp_t* ConfigurationResponse = (BT_Signal_ConfigurationResp_t*)SignalCommandHeader->Data;

	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, 0, le16_to_cpu(ConfigurationResponse->SourceChannel));

	if (!(L2CAPChannel))
	  return;

	/* Check if the channel configuration completed successfully */
	if (ConfigurationResponse->Result == CPU_TO_LE16(BT_CONFIGURATION_SUCCESSFUL))
	{
		switch (L2CAPChannel->State)
		{
			case L2CAP_CHANSTATE_Config_WaitReqResp:
				L2CAPChannel->State = L2CAP_CHANSTATE_Config_WaitReq;
				break;
			case L2CAP_CHANSTATE_Config_WaitResp:
				L2CAPChannel->State = L2CAP_CHANSTATE_Open;
				EVENT_Bluetooth_ChannelOpened(StackState, L2CAPChannel);
				break;
		}
	}
	else
	{
		/* Configuration failed, close the channel */
		L2CAPChannel->State = L2CAP_CHANSTATE_Closed;
	}
}

/** Initalizes the Bluetooth L2CAP layer, ready for new channel connections.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_L2CAP_Init(BT_StackConfig_t* const StackState)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	  StackState->State.L2CAP.Channels[i].State = L2CAP_CHANSTATE_Closed;
}

/** Processes a recieved Bluetooth L2CAP packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_L2CAP_ProcessPacket(BT_StackConfig_t* const StackState)
{
	BT_L2CAP_Header_t*      L2CAPPacketHeader = (BT_L2CAP_Header_t*)StackState->Config.PacketBuffer;
	BT_DataPacket_Header_t* L2CAPDataHeader   = (BT_DataPacket_Header_t*)L2CAPPacketHeader->Data;
	
	/* Find the associated HCI connection via the connection handle indicated in the L2CAP packet header */
	BT_HCI_Connection_t*    HCIConnection     = Bluetooth_HCI_FindConnection(StackState, NULL, le16_to_cpu(L2CAPPacketHeader->ConnectionHandle));
	
	/* Connection not found, abort processing of packet */
	if (!(HCIConnection))
	  return;

	/* Check if signalling (control) packet data, or user application connection data */
	if (L2CAPDataHeader->DestinationChannel == BT_CHANNEL_SIGNALING)
	{
		BT_Signal_Header_t* SignalCommandHeader = (BT_Signal_Header_t*)L2CAPDataHeader->Payload;

		switch (SignalCommandHeader->Code)
		{
			case BT_SIGNAL_CONNECTION_REQUEST:
				Bluetooth_L2CAP_Signal_ConnRequest(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONNECTION_RESPONSE:
				Bluetooth_L2CAP_Signal_ConnResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONFIGURATION_REQUEST:
				Bluetooth_L2CAP_Signal_ConfigReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONFIGURATION_RESPONSE:
				Bluetooth_L2CAP_Signal_ConfigResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_DISCONNECTION_REQUEST:
				Bluetooth_L2CAP_Signal_DisconnectionReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_DISCONNECTION_RESPONSE:
				Bluetooth_L2CAP_Signal_DisconnectionResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_ECHO_REQUEST:
				Bluetooth_L2CAP_Signal_EchoReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_INFORMATION_REQUEST:
				Bluetooth_L2CAP_Signal_InformationReq(StackState, HCIConnection, SignalCommandHeader);
				break;
		}
	}
	else
	{
		/* Find the associated L2CAP channel via the connection handle and destination channel */
		BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, le16_to_cpu(L2CAPDataHeader->DestinationChannel), 0);
		
		/* If the L2CAP channel was found, fire the user application callback, otherwise discard the packet */
		if (L2CAPChannel)
		  EVENT_Bluetooth_DataReceived(StackState, L2CAPChannel, L2CAPDataHeader->PayloadLength, L2CAPDataHeader->Payload);
	}
}

/** Manages the existing L2CAP layer connections of a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *
 *  \return Boolean \c true if more L2CAP management tasks are pending, \c false otherwise.
 */
bool Bluetooth_L2CAP_Manage(BT_StackConfig_t* const StackState)
{
	/* Check for any half-open channels, send configuration details to the remote device if found */
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];

		/* Check if we are in a channel state which requires a configuration request to be sent */
		switch (CurrentChannel->State)
		{
			case L2CAP_CHANSTATE_Config_WaitConfig:
				CurrentChannel->State = L2CAP_CHANSTATE_Config_WaitReqResp;
				break;
			case L2CAP_CHANSTATE_Config_WaitSendConfig:
				CurrentChannel->State = L2CAP_CHANSTATE_Config_WaitResp;
				break;
			default:
				continue;
		}

		/* Find the associated HCI connection via the connection handle indicated in the L2CAP packet header */
		BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, CurrentChannel->ConnectionHandle);

		/* If the HCI connection is invalid, abort the configuration */
		if (!(HCIConnection))
		  continue;

		struct
		{
			BT_Signal_Header_t           SignalCommandHeader;
			BT_Signal_ConfigurationReq_t ConfigurationRequest;

			struct
			{
				BT_Config_Option_Header_t Header;
				uint16_t Value;
			} Option_LocalMTU;
		} PacketData;

		/* Fill out the Signal Command header in the response packet */
		PacketData.SignalCommandHeader.Code            = BT_SIGNAL_CONFIGURATION_REQUEST;
		PacketData.SignalCommandHeader.Identifier      = ++HCIConnection->CurrentIdentifier;
		PacketData.SignalCommandHeader.Length          = CPU_TO_LE16(sizeof(PacketData.ConfigurationRequest) +
		                                                             sizeof(PacketData.Option_LocalMTU));

		/* Fill out the Configuration Request in the response packet, including local MTU information */
		PacketData.ConfigurationRequest.DestinationChannel = cpu_to_le16(CurrentChannel->RemoteNumber);
		PacketData.ConfigurationRequest.Flags          = 0;
		PacketData.Option_LocalMTU.Header.Type         = CPU_TO_LE16(BT_CONFIG_OPTION_MTU);
		PacketData.Option_LocalMTU.Header.Length       = sizeof(PacketData.Option_LocalMTU.Value);
		PacketData.Option_LocalMTU.Value               = cpu_to_le16(CurrentChannel->LocalMTU);

		Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, sizeof(PacketData), &PacketData);
		return true;
	}
	
	return false;
}

bool Bluetooth_L2CAP_SendPacket(BT_StackConfig_t* const StackState,
                                BT_L2CAP_Channel_t* const L2CAPChannel,
                                uint16_t Length,
                                void* Data)
{
	/* If the destination channel is not the signaling channel and it is not currently fully open, abort */
	if (!(L2CAPChannel) || (L2CAPChannel->State != L2CAP_CHANSTATE_Open))
	  return false;

	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, L2CAPChannel->ConnectionHandle);

	/* If an open device connection with the correct connection handle was not foumd, abort */
	if (!(HCIConnection))
	  return false;
	
	/* TODO: Respect MTU */
	
	BT_L2CAP_Header_t* L2CAPPacketHeader  = (BT_L2CAP_Header_t*)StackState->Config.PacketBuffer;
	L2CAPPacketHeader->ConnectionHandle = cpu_to_le16(HCIConnection->Handle | BT_L2CAP_FIRST_AUTOFLUSH);
	L2CAPPacketHeader->DataLength       = cpu_to_le16(sizeof(BT_DataPacket_Header_t) + Length);

	BT_DataPacket_Header_t* L2CAPDataHeader = (BT_DataPacket_Header_t*)L2CAPPacketHeader->Data;
	L2CAPDataHeader->PayloadLength      = cpu_to_le16(Length);
	L2CAPDataHeader->DestinationChannel = cpu_to_le16(L2CAPChannel->RemoteNumber);

	memcpy(L2CAPDataHeader->Payload, Data, Length);
	
	CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_L2CAPData, (sizeof(BT_L2CAP_Header_t) + sizeof(BT_DataPacket_Header_t) + Length));
	return true;
}

BT_L2CAP_Channel_t* const Bluetooth_L2CAP_OpenChannel(BT_StackConfig_t* const StackState,
                                                      BT_HCI_Connection_t* const HCIConnection,
                                                      const uint16_t PSM)
{
	/* Create a new channel object from the requested parameters if space exists in the connection table */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_NewChannel(StackState, HCIConnection->Handle, 0, PSM);

	/* If no space left in the connection table, abort */
	if (L2CAPChannel == NULL)
	  return NULL;

	/* Update the channel state to indicate it is waiting for a connection response */
	L2CAPChannel->State = L2CAP_CHANSTATE_WaitConnectRsp;

	struct
	{
		BT_Signal_Header_t        SignalCommandHeader;
		BT_Signal_ConnectionReq_t ConnectionRequest;
	} PacketData;

	/* Fill out the Signal Command header in the response packet */
	PacketData.SignalCommandHeader.Code        = BT_SIGNAL_CONNECTION_REQUEST;
	PacketData.SignalCommandHeader.Identifier  = ++HCIConnection->CurrentIdentifier;
	PacketData.SignalCommandHeader.Length      = CPU_TO_LE16(sizeof(PacketData.ConnectionRequest));

	/* Fill out the Connection Request in the response packet */
	PacketData.ConnectionRequest.PSM           = cpu_to_le16(PSM);
	PacketData.ConnectionRequest.SourceChannel = cpu_to_le16(L2CAPChannel->LocalNumber);

	Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, sizeof(PacketData), &PacketData);
	return L2CAPChannel;
}

void Bluetooth_L2CAP_CloseChannel(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const L2CAPChannel)
{
	/* Don't try to close a non-existing or already closed channel */
	if ((L2CAPChannel == NULL) || (L2CAPChannel->State == L2CAP_CHANSTATE_Closed))
	  return;

	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, L2CAPChannel->ConnectionHandle);

	/* Set the channel's state to the start of the teardown process */
	L2CAPChannel->State = L2CAP_CHANSTATE_WaitDisconnect;

	struct
	{
		BT_Signal_Header_t           SignalCommandHeader;
		BT_Signal_DisconnectionReq_t DisconnectionRequest;
	} PacketData;

	/* Fill out the Signal Command header in the response packet */
	PacketData.SignalCommandHeader.Code            = BT_SIGNAL_DISCONNECTION_REQUEST;
	PacketData.SignalCommandHeader.Identifier      = ++HCIConnection->CurrentIdentifier;
	PacketData.SignalCommandHeader.Length          = CPU_TO_LE16(sizeof(PacketData.DisconnectionRequest));

	/* Fill out the Disconnection Request in the response packet */
	PacketData.DisconnectionRequest.DestinationChannel = cpu_to_le16(L2CAPChannel->RemoteNumber);
	PacketData.DisconnectionRequest.SourceChannel      = cpu_to_le16(L2CAPChannel->LocalNumber);

	Bluetooth_L2CAP_SendSignalPacket(StackState, HCIConnection, sizeof(PacketData), &PacketData);
}

