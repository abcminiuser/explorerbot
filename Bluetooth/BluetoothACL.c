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

void Bluetooth_ACL_NotifyHCIDisconnection(BT_StackConfig_t* const StackState,
                                          const uint16_t ConnectionHandle)
{
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_ACL_Channel_t* CurrentChannel = &StackState->State.ACL.Channels[i];
		
		// Clear any open connections that are associated with the destroyed connection handle
		if (CurrentChannel->ConnectionHandle == ConnectionHandle)
		{
			// Notify the user of the closed connection if it is currently open
			if (CurrentChannel->State == ACL_CHANSTATE_Open)
			  EVENT_Bluetooth_ChannelClosed(StackState, CurrentChannel);

			CurrentChannel->State = ACL_CHANSTATE_Closed;
		}
	}
}

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

static bool Bluetooth_ACL_SendSignalPacket(BT_StackConfig_t* const StackState,
                                           BT_HCI_Connection_t* HCIConnection,
                                           uint16_t Length,
                                           void* Data)
{
	BT_ACL_Channel_t SignalChannel;
	SignalChannel.ConnectionHandle = HCIConnection->Handle;
	SignalChannel.State            = ACL_CHANSTATE_Open;
	SignalChannel.LocalNumber      = BT_CHANNEL_SIGNALING;
	SignalChannel.RemoteNumber     = BT_CHANNEL_SIGNALING;

	return Bluetooth_ACL_SendPacket(StackState, &SignalChannel, Length, Data);
}

static inline void Bluetooth_ACL_Signal_ConnRequest(BT_StackConfig_t* const StackState,
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
	ResponsePacket.ConnectionResponse.Result             = cpu_to_le16(RejectionReason);
	ResponsePacket.ConnectionResponse.Status             = 0x00;
	
	Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_ACL_Signal_ConnResp(BT_StackConfig_t* const StackState,
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

static inline void Bluetooth_ACL_Signal_EchoReq(BT_StackConfig_t* const StackState,
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

	Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_ACL_Signal_DisconnectionReq(BT_StackConfig_t* const StackState,
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

	// If a valid connection was found, close it and notify the user
	if (ACLChannel)
	{
		ACLChannel->State = ACL_CHANSTATE_Closed;
		EVENT_Bluetooth_ChannelClosed(StackState, ACLChannel);
	}

	Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_ACL_Signal_DisconnectionResp(BT_StackConfig_t* const StackState,
                                                          BT_HCI_Connection_t* const HCIConnection,
                                                          BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_DisconnectionResp_t* DisconnectionResponse = (BT_Signal_DisconnectionResp_t*)SignalCommandHeader->Data;

	// Find the existing channel in the channel table if it exists
	BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_FindChannel(StackState, HCIConnection->Handle, 0, DisconnectionResponse->SourceChannel);
	
	// If a valid connection was found, close it and notify the user
	if (ACLChannel)
	{
		ACLChannel->State = ACL_CHANSTATE_Closed;
		EVENT_Bluetooth_ChannelClosed(StackState, ACLChannel);
	}
}

static inline void Bluetooth_ACL_Signal_InformationReq(BT_StackConfig_t* const StackState,
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

	// Retrieve the requested information and store it in the outgoing packet, if found
	switch (InformationRequest->InfoType)
	{
		case BT_INFOREQ_MTU:
			ResponsePacket.InformationResponse.Result = CPU_TO_LE16(BT_INFORMATION_SUCCESSFUL);
			DataLen = 2;

			*((uint16_t*)&ResponsePacket.Data) = CPU_TO_LE16(DEFAULT_ACL_CHANNEL_MTU);
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

	Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, (sizeof(ResponsePacket) - sizeof(ResponsePacket.Data) + DataLen), &ResponsePacket);
}

static inline void Bluetooth_ACL_Signal_ConfigReq(BT_StackConfig_t* const StackState,
                                                  BT_HCI_Connection_t* const HCIConnection,
                                                  BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConfigurationReq_t* ConfigurationRequest = (BT_Signal_ConfigurationReq_t*)SignalCommandHeader->Data;

	// Find the existing channel in the channel table if it exists
	BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_FindChannel(StackState, HCIConnection->Handle, 0, ConfigurationRequest->DestinationChannel);

	// Only look at the channel configuration options if a valid channel entry for the local channel number was found
	if (ACLChannel != NULL)
	{
		// Update channel state based on the current state
		switch (ACLChannel->State)
		{
			case ACL_CHANSTATE_Config_WaitConfig:
				ACLChannel->State = ACL_CHANSTATE_Config_WaitSendConfig;
				break;
			case ACL_CHANSTATE_Config_WaitReqResp:
				ACLChannel->State = ACL_CHANSTATE_Config_WaitResp;
				break;
			case ACL_CHANSTATE_Config_WaitReq:
				ACLChannel->State = ACL_CHANSTATE_Open;
				EVENT_Bluetooth_ChannelOpened(StackState, ACLChannel);
				break;
		}
		
		uint8_t* OptionsPos = (uint8_t*)ConfigurationRequest->Options;
		uint8_t* OptionsEnd = (uint8_t*)((intptr_t)OptionsPos + (SignalCommandHeader->Length - sizeof(ConfigurationRequest)));

		while (OptionsPos < OptionsEnd)
		{
			BT_Config_Option_Header_t* OptionHeader = (BT_Config_Option_Header_t*)OptionsPos;
			
			/* Store the remote MTU option's value if present */
			if (OptionHeader->Type == BT_CONFIG_OPTION_MTU)
			  ACLChannel->RemoteMTU = le16_to_cpu(*((uint16_t*)OptionHeader->Data));

			/* Progress to the next option in the packet */
			OptionsPos += (sizeof(BT_Config_Option_Header_t) + OptionHeader->Length);			
		}
	}

	struct
	{
		BT_Signal_Header_t            SignalCommandHeader;
		BT_Signal_ConfigurationResp_t ConfigurationResponse;
	} ResponsePacket;

	// Fill out the Signal Command header in the response packet
	ResponsePacket.SignalCommandHeader.Code              = BT_SIGNAL_CONFIGURATION_RESPONSE;
	ResponsePacket.SignalCommandHeader.Identifier        = SignalCommandHeader->Identifier;
	ResponsePacket.SignalCommandHeader.Length            = CPU_TO_LE16(sizeof(ResponsePacket.ConfigurationResponse));

	/* Fill out the Configuration Response in the response packet */
	ResponsePacket.ConfigurationResponse.SourceChannel   = ACLChannel->RemoteNumber;
	ResponsePacket.ConfigurationResponse.Flags           = 0x00;
	ResponsePacket.ConfigurationResponse.Result          = (ACLChannel != NULL) ? CPU_TO_LE16(BT_CONFIGURATION_SUCCESSFUL) : CPU_TO_LE16(BT_CONFIGURATION_REJECTED);

	Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, sizeof(ResponsePacket), &ResponsePacket);
}

static inline void Bluetooth_ACL_Signal_ConfigResp(BT_StackConfig_t* const StackState,
                                                   BT_HCI_Connection_t* const HCIConnection,
                                                   BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConfigurationResp_t* ConfigurationResponse = (BT_Signal_ConfigurationResp_t*)SignalCommandHeader->Data;

	// Find the existing channel in the channel table if it exists
	BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_FindChannel(StackState, HCIConnection->Handle, 0, ConfigurationResponse->SourceChannel);
	
	if (!(ACLChannel))
	  return;

	// Check if the channel configuration completed successfully
	if (ConfigurationResponse->Result == CPU_TO_LE16(BT_CONFIGURATION_SUCCESSFUL))
	{
		switch (ACLChannel->State)
		{
			case ACL_CHANSTATE_Config_WaitReqResp:
				ACLChannel->State = ACL_CHANSTATE_Config_WaitReq;
				break;
			case ACL_CHANSTATE_Config_WaitResp:
				ACLChannel->State = ACL_CHANSTATE_Open;
				EVENT_Bluetooth_ChannelOpened(StackState, ACLChannel);
				break;
		}
	}
	else
	{
		// Configuration failed, close the channel
		ACLChannel->State = ACL_CHANSTATE_Closed;
	}
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
				Bluetooth_ACL_Signal_ConnRequest(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONNECTION_RESPONSE:
				Bluetooth_ACL_Signal_ConnResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONFIGURATION_REQUEST:
				Bluetooth_ACL_Signal_ConfigReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONFIGURATION_RESPONSE:
				Bluetooth_ACL_Signal_ConfigResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_DISCONNECTION_REQUEST:
				Bluetooth_ACL_Signal_DisconnectionReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_DISCONNECTION_RESPONSE:
				Bluetooth_ACL_Signal_DisconnectionResp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_ECHO_REQUEST:
				Bluetooth_ACL_Signal_EchoReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_INFORMATION_REQUEST:
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
	// Check for any half-open channels, send configuration details to the remote device if found
	for (uint8_t i = 0; i < MAX_LOGICAL_CHANNELS; i++)
	{
		BT_ACL_Channel_t* CurrentChannel = &StackState->State.ACL.Channels[i];

		// Check if we are in a channel state which requires a configuration request to be sent
		switch (CurrentChannel->State)
		{
			case ACL_CHANSTATE_Config_WaitConfig:
				CurrentChannel->State = ACL_CHANSTATE_Config_WaitReqResp;
				break;
			case ACL_CHANSTATE_Config_WaitSendConfig:
				CurrentChannel->State = ACL_CHANSTATE_Config_WaitResp;
				break;
			default:
				continue;
		}

		// Find the associated HCI connection via the connection handle indicated in the ACL packet header
		BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, CurrentChannel->ConnectionHandle);

		// If the HCI connection is invalid, abort the configuration
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
		PacketData.ConfigurationRequest.DestinationChannel = CurrentChannel->RemoteNumber;
		PacketData.ConfigurationRequest.Flags          = 0;
		PacketData.Option_LocalMTU.Header.Type         = CPU_TO_LE16(BT_CONFIG_OPTION_MTU);
		PacketData.Option_LocalMTU.Header.Length       = sizeof(PacketData.Option_LocalMTU.Value);
		PacketData.Option_LocalMTU.Value               = cpu_to_le16(CurrentChannel->LocalMTU);

		Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, sizeof(PacketData), &PacketData);
		return true;
	}
	
	return false;
}

bool Bluetooth_ACL_SendPacket(BT_StackConfig_t* const StackState,
                              BT_ACL_Channel_t* const ACLChannel,
                              uint16_t Length,
                              void* Data)
{
	// If the destination channel is not the signaling channel and it is not currently fully open, abort
	if (!(ACLChannel) || (ACLChannel->State != ACL_CHANSTATE_Open))
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

BT_ACL_Channel_t* Bluetooth_ACL_OpenChannel(BT_StackConfig_t* const StackState,
                                            BT_HCI_Connection_t* const HCIConnection,
                                            const uint16_t PSM)
{
	// Create a new channel object from the requested parameters if space exists in the connection table
	BT_ACL_Channel_t* ACLChannel = Bluetooth_ACL_NewChannel(StackState, HCIConnection->Handle, 0, PSM);

	// If no space left in the connection table, abort
	if (ACLChannel == NULL)
	  return NULL;

	// Update the channel state to indicate it is waiting for a connection response
	ACLChannel->State = ACL_CHANSTATE_WaitConnectRsp;

	struct
	{
		BT_Signal_Header_t        SignalCommandHeader;
		BT_Signal_ConnectionReq_t ConnectionRequest;
	} PacketData;

	// Fill out the Signal Command header in the response packe
	PacketData.SignalCommandHeader.Code              = BT_SIGNAL_CONNECTION_REQUEST;
	PacketData.SignalCommandHeader.Identifier        = ++HCIConnection->CurrentIdentifier;
	PacketData.SignalCommandHeader.Length            = CPU_TO_LE16(sizeof(PacketData.ConnectionRequest));

	/* Fill out the Connection Request in the response packet */
	PacketData.ConnectionRequest.PSM                 = PSM;
	PacketData.ConnectionRequest.SourceChannel       = ACLChannel->LocalNumber;

	Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, sizeof(PacketData), &PacketData);
	return ACLChannel;
}

void Bluetooth_ACL_CloseChannel(BT_StackConfig_t* const StackState,
                                BT_ACL_Channel_t* const ACLChannel)
{
	// Don't try to close a non-existing or already closed channel
	if ((ACLChannel == NULL) || (ACLChannel->State == ACL_CHANSTATE_Closed))
	  return;

	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, ACLChannel->ConnectionHandle);

	/* Set the channel's state to the start of the teardown process */
	ACLChannel->State = ACL_CHANSTATE_WaitDisconnect;

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
	PacketData.DisconnectionRequest.DestinationChannel = ACLChannel->RemoteNumber;
	PacketData.DisconnectionRequest.SourceChannel      = ACLChannel->LocalNumber;

	Bluetooth_ACL_SendSignalPacket(StackState, HCIConnection, sizeof(PacketData), &PacketData);
}

