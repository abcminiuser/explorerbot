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


/** Notifies the L2CAP layer of a HCI connection layer disconnection, so that any associated L2CAP channels can also be terminated.
 *
 *  \param[in, out] StackState        Pointer to a Bluetooth Stack state table.
 *  \param[in]      ConnectionHandle  Handle of the HCI connection that was disconnected.
 */
void Bluetooth_L2CAP_NotifyHCIDisconnection(BT_StackConfig_t* const StackState,
                                            const uint16_t ConnectionHandle)
{
	uint8_t EventIndex = 0;
	
	/* Need to remove any pending L2CAP signal events that belong to the terminated connection */
	while (EventIndex < StackState->State.L2CAP.PendingEvents)
	{
		BT_L2CAP_Event_t* CurrentEvent = &StackState->State.L2CAP.Events[EventIndex];
		
		/* Current queued event is associated with the now terminated connection, dequeue it */
		if (CurrentEvent->ConnectionHandle == ConnectionHandle)
		{
			memmove(&StackState->State.L2CAP.Events[EventIndex], &StackState->State.L2CAP.Events[EventIndex + 1], (sizeof(BT_L2CAP_Event_t) * (StackState->State.L2CAP.PendingEvents - EventIndex)));
			StackState->State.L2CAP.PendingEvents--;
			continue;
		}

		EventIndex++;
	}

	/* Need to close any channel objects associated with the terminated connection, and notify the user */
	for (uint8_t i = 0; i < BT_MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];
		
		/* Clear any open connections that are associated with the destroyed connection handle */
		if (CurrentChannel->ConnectionHandle == ConnectionHandle)
		{
			/* Check if we need to notify the user of a channel close event */
			bool MustNotify = (CurrentChannel->State == L2CAP_CHANSTATE_Open);
			
			/* Mark the channel as being fully closed - do this before notifying the user app, so that channel object is in the correct state */
			CurrentChannel->State = L2CAP_CHANSTATE_Closed;
			
			/* Notify the user of the closed connection if it was currently open */
			if (MustNotify)
			  EVENT_Bluetooth_ChannelStateChange(StackState, CurrentChannel);
		}
	}
}

/** Finds a Bluetooth L2CAP connection handle associated with a given HCI connection handle and
 *  local or remote L2CAP channel number.
 *
 *  \param[in, out] StackState        Pointer to a Bluetooth Stack state table.
 *  \param[in]      ConnectionHandle  Handle of the HCI connection to search on.
 *  \param[in]      LocalChannel      Local L2CAP channel index to search for, or zero if this should be ignored.
 *  \param[in]      RemoteChannel     Remote L2CAP channel index to search for, or zero if this should be ignored.
 *
 *  \return Handle to the L2CAP channel object found matching the input search parameter(s) if found, \c NULL otherwise.
 */
BT_L2CAP_Channel_t* const Bluetooth_L2CAP_FindChannel(BT_StackConfig_t* const StackState,
                                                      const uint16_t ConnectionHandle,
                                                      const uint16_t LocalChannel,
                                                      const uint16_t RemoteChannel)
{
	for (uint8_t i = 0; i < BT_MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];

		/* Skip over closed (unused) channel entries and entries with the incorrect connection handle */
		if ((CurrentChannel->State == L2CAP_CHANSTATE_Closed) ||
		    (CurrentChannel->ConnectionHandle != ConnectionHandle))
		{
			continue;
		}
		
		/* If local channel number specified, check for match */
		if ((LocalChannel) && (CurrentChannel->LocalNumber != LocalChannel))
		  continue;

		/* If remote channel number specified, check for match */
		if ((RemoteChannel) && (CurrentChannel->RemoteNumber != RemoteChannel))
		  continue;
		
		/* All search parameters match, return the found channel object */
		return CurrentChannel;
	}
	
	return NULL;
}

static BT_L2CAP_Channel_t* const Bluetooth_L2CAP_NewChannel(BT_StackConfig_t* const StackState,
                                                            const uint16_t ConnectionHandle,
                                                            const uint16_t RemoteChannel,
                                                            const uint16_t PSM)
{
	for (uint8_t i = 0; i < BT_MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];

		if (CurrentChannel->State != L2CAP_CHANSTATE_Closed)
		  continue;

		CurrentChannel->ConnectionHandle = ConnectionHandle;
		CurrentChannel->LocalNumber      = StackState->State.L2CAP.LastAllocatedChannel;
		CurrentChannel->RemoteNumber     = RemoteChannel;
		CurrentChannel->LocalMTU         = BT_DEFAULT_L2CAP_CHANNEL_MTU;
		CurrentChannel->RemoteMTU        = 0xFFFF;
		CurrentChannel->State            = L2CAP_CHANSTATE_New;
		CurrentChannel->PSM              = PSM;
		CurrentChannel->LocallyInitiated = false;
		
		/* Wrap the allocated channel number back to the starting address when all channel indexes have been allocated */
		if (StackState->State.L2CAP.LastAllocatedChannel++ == 0xFFFF)
		  StackState->State.L2CAP.LastAllocatedChannel = BT_CHANNEL_BASEOFFSET;
		  
		return CurrentChannel;
	}
	
	return NULL;
}

static BT_L2CAP_Event_t* Bluetooth_L2CAP_NewEvent(BT_StackConfig_t* const StackState,
                                                  const uint16_t ConnectionHandle,
                                                  const uint8_t Identifier)
{
	if (StackState->State.L2CAP.PendingEvents == BT_MAX_QUEUED_L2CAP_EVENTS)
	  return NULL;
	
	BT_L2CAP_Event_t* Event = &StackState->State.L2CAP.Events[StackState->State.L2CAP.PendingEvents++];
	
	Event->ConnectionHandle   = ConnectionHandle;
	Event->Identifier         = Identifier;
	Event->LocalNumber        = 0;
	Event->RemoteNumber       = 0;
	Event->Result             = 0;
	return Event;
}

static bool Bluetooth_L2CAP_SendSignalPacket(BT_StackConfig_t* const StackState,
                                             const uint16_t ConnectionHandle,
                                             const uint8_t SignalCode,
                                             const uint8_t SignalIdentifier,
                                             const uint16_t Length,
                                             void* Data)
{
	/* Construct a temporary channel object with the signalling channel indexes */
	BT_L2CAP_Channel_t SignalChannel;
	SignalChannel.ConnectionHandle = cpu_to_le16(ConnectionHandle);
	SignalChannel.State            = L2CAP_CHANSTATE_Open;
	SignalChannel.LocalNumber      = CPU_TO_LE16(BT_CHANNEL_SIGNALING);
	SignalChannel.RemoteNumber     = CPU_TO_LE16(BT_CHANNEL_SIGNALING);
	
	struct
	{
		BT_Signal_Header_t SignalCommandHeader;
		uint8_t            Data[Length];
	} ATTR_PACKED SignalPacket;

	/*  Fill out the Signal Command header in the response packet */
	SignalPacket.SignalCommandHeader.Code              = SignalCode;
	SignalPacket.SignalCommandHeader.Identifier        = SignalIdentifier;
	SignalPacket.SignalCommandHeader.Length            = cpu_to_le16(Length);
	
	memcpy(SignalPacket.Data, Data, Length);

	return Bluetooth_L2CAP_SendPacket(StackState, &SignalChannel, sizeof(SignalPacket), &SignalPacket);
}

static inline void Bluetooth_L2CAP_Signal_EchoReq(BT_StackConfig_t* const StackState,
                                                  BT_HCI_Connection_t* const HCIConnection,
                                                  BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
	
	if (!(Event))
	  return;

	Event->Event = L2CAP_EVENT_EchoReq;
}

static inline void Bluetooth_L2CAP_Signal_InformationReq(BT_StackConfig_t* const StackState,
                                                         BT_HCI_Connection_t* const HCIConnection,
                                                         BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_InformationReq_t* InformationRequest = (BT_Signal_InformationReq_t*)SignalCommandHeader->Data;

	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
	
	if (!(Event))
	  return;

	Event->Event  = L2CAP_EVENT_InformationReq;
	Event->Result = le16_to_cpu(InformationRequest->InfoType);
}

static inline void Bluetooth_L2CAP_Signal_ConnectReq(BT_StackConfig_t* const StackState,
                                                     BT_HCI_Connection_t* const HCIConnection,
                                                     BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConnectionReq_t* ConnectionRequest = (BT_Signal_ConnectionReq_t*)SignalCommandHeader->Data;
				
	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
	
	if (!(Event))
	  return;

	/* Create a new channel from the given connection request data */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_NewChannel(StackState, HCIConnection->Handle,
	                                                              le16_to_cpu(ConnectionRequest->SourceChannel), le16_to_cpu(ConnectionRequest->PSM));
	
	uint8_t RejectionReason = BT_CONNECTION_REFUSED_RESOURCES;

	/* If there was space in the channel table, request action from the user to accept/reject the connection */
	if (L2CAPChannel)
	  RejectionReason = CALLBACK_Bluetooth_ChannelRequest(StackState, HCIConnection, L2CAPChannel) ? BT_CONNECTION_SUCCESSFUL : BT_CONNECTION_REFUSED_PSM;

	Event->Event        = L2CAP_EVENT_ConnectReq;
	Event->LocalNumber  = L2CAPChannel->LocalNumber;
	Event->RemoteNumber = le16_to_cpu(ConnectionRequest->SourceChannel);
	Event->Result       = RejectionReason;
}

static inline void Bluetooth_L2CAP_Signal_ConnectRsp(BT_StackConfig_t* const StackState,
                                                     BT_HCI_Connection_t* const HCIConnection,
                                                     BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConnectionResp_t* ConnectionResponse = (BT_Signal_ConnectionResp_t*)SignalCommandHeader->Data;
		
	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, le16_to_cpu(ConnectionResponse->SourceChannel), 0);

	if (L2CAPChannel)
	{
		/* Save the now established connection's remote channel number allocated by the remote device */
		L2CAPChannel->RemoteNumber = le16_to_cpu(ConnectionResponse->DestinationChannel);
	
		BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
		
		if (!(Event))
		  return;
		  
		Event->Event        = L2CAP_EVENT_ConnectRsp;
		Event->RemoteNumber = L2CAPChannel->RemoteNumber;
		Event->LocalNumber  = L2CAPChannel->LocalNumber;
	}
}

static inline void Bluetooth_L2CAP_Signal_DisconnectReq(BT_StackConfig_t* const StackState,
                                                        BT_HCI_Connection_t* const HCIConnection,
                                                        BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_DisconnectionReq_t* DisconnectionRequest = (BT_Signal_DisconnectionReq_t*)SignalCommandHeader->Data;

	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
	
	if (!(Event))
	  return;
	  
	Event->Event        = L2CAP_EVENT_DisconnectReq;
	Event->RemoteNumber = le16_to_cpu(DisconnectionRequest->SourceChannel);
	Event->LocalNumber  = le16_to_cpu(DisconnectionRequest->DestinationChannel);
}

static inline void Bluetooth_L2CAP_Signal_DisconnectRsp(BT_StackConfig_t* const StackState,
                                                        BT_HCI_Connection_t* const HCIConnection,
                                                        BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_DisconnectionResp_t* DisconnectionResponse = (BT_Signal_DisconnectionResp_t*)SignalCommandHeader->Data;

	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, 0, le16_to_cpu(DisconnectionResponse->SourceChannel));
	
	/* If a valid connection was found, close it and notify the user */
	if (L2CAPChannel)
	{
		BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
		
		if (!(Event))
		  return;
		  
		Event->Event        = L2CAP_EVENT_DisconnectRsp;
		Event->LocalNumber  = L2CAPChannel->LocalNumber;
		Event->RemoteNumber = L2CAPChannel->RemoteNumber;
	}
}

static inline void Bluetooth_L2CAP_Signal_ConfigReq(BT_StackConfig_t* const StackState,
                                                    BT_HCI_Connection_t* const HCIConnection,
                                                    BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConfigurationReq_t* ConfigurationRequest = (BT_Signal_ConfigurationReq_t*)SignalCommandHeader->Data;

	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
	
	if (!(Event))
	  return;

	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, le16_to_cpu(ConfigurationRequest->DestinationChannel), 0);

	if (!(L2CAPChannel) || (L2CAPChannel->State == L2CAP_CHANSTATE_WaitConnectRsp))
	{
		Event->Event       = L2CAP_EVENT_SendRejectReq;
		Event->LocalNumber = le16_to_cpu(ConfigurationRequest->DestinationChannel);
		Event->Result      = BT_CMDREJ_INVALID_CHANNEL_ID;
		return;
	}
	
	uint16_t OptionsLength     = (le16_to_cpu(SignalCommandHeader->Length) - sizeof(*ConfigurationRequest));
	uint8_t  OptionsPos        = 0;
	bool     OptionsAcceptable = true;
		
	/* Process the channel configuration options, abort if a mandatory unknown option is encountered */
	while ((OptionsPos < OptionsLength) && OptionsAcceptable)
	{
		BT_Config_Option_Header_t* OptionHeader = (BT_Config_Option_Header_t*)&ConfigurationRequest->Options[OptionsPos];
		
		switch (OptionHeader->Type & 0xEF)
		{
			case BT_CONFIG_OPTION_MTU:
				L2CAPChannel->RemoteMTU = (((uint16_t)OptionHeader->Data[1] << 8) | OptionHeader->Data[0]);
				break;
			default:
				if (OptionHeader->Type & (1 << 7))
				  OptionsAcceptable = false;
				break;
		}

		/* Progress to the next option in the packet */
		OptionsPos += (sizeof(*OptionHeader) + OptionHeader->Length);
	}
	  
	Event->Event       = L2CAP_EVENT_ConfigReq;
	Event->LocalNumber = le16_to_cpu(ConfigurationRequest->DestinationChannel);
	Event->Result      = (OptionsAcceptable) ? BT_CONFIGURATION_SUCCESSFUL : BT_CONFIGURATION_UNKNOWNOPTIONS;
}

static inline void Bluetooth_L2CAP_Signal_ConfigRsp(BT_StackConfig_t* const StackState,
                                                    BT_HCI_Connection_t* const HCIConnection,
                                                    BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_ConfigurationResp_t* ConfigurationResponse = (BT_Signal_ConfigurationResp_t*)SignalCommandHeader->Data;

	/* Find the existing channel in the channel table if it exists */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, HCIConnection->Handle, le16_to_cpu(ConfigurationResponse->SourceChannel), 0);

	if (!(L2CAPChannel))
	  return;

	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
	
	if (!(Event))
	  return;

	Event->Event       = L2CAP_EVENT_ConfigRsp;
	Event->LocalNumber = L2CAPChannel->LocalNumber;
	Event->Result      = le16_to_cpu(ConfigurationResponse->Result);
}

static inline void Bluetooth_L2CAP_Signal_CommandRej(BT_StackConfig_t* const StackState,
                                                     BT_HCI_Connection_t* const HCIConnection,
                                                     BT_Signal_Header_t* const SignalCommandHeader)
{
	BT_Signal_CommandRej_t* CommandRejectRequest = (BT_Signal_CommandRej_t*)SignalCommandHeader->Data;

	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, SignalCommandHeader->Identifier);
	
	if (!(Event))
	  return;

	Event->Event        = L2CAP_EVENT_CommandRej;
	Event->RemoteNumber = le16_to_cpu(CommandRejectRequest->Data[1]);
	Event->LocalNumber  = le16_to_cpu(CommandRejectRequest->Data[0]);
	Event->Result       = le16_to_cpu(CommandRejectRequest->Reason);
}

/** Initalizes the Bluetooth L2CAP layer, ready for new channel connections.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_L2CAP_Init(BT_StackConfig_t* const StackState)
{
	for (uint8_t i = 0; i < BT_MAX_LOGICAL_CHANNELS; i++)
	  StackState->State.L2CAP.Channels[i].State = L2CAP_CHANSTATE_Closed;
	  
	StackState->State.L2CAP.PendingEvents        = 0;
	StackState->State.L2CAP.LastAllocatedChannel = BT_CHANNEL_BASEOFFSET;
}

/** Processes a recieved Bluetooth L2CAP packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState     Pointer to a Bluetooth Stack state table.
 *  \param[in, out] HCIConnection  HCI connection handle the packet was received on.
 *  \param[in]      Data           Pointer to the received data packet.
 */
void Bluetooth_L2CAP_ProcessPacket(BT_StackConfig_t* const StackState,
                                   BT_HCI_Connection_t* const HCIConnection,
                                   const void* Data)
{
	BT_DataPacket_Header_t* L2CAPDataHeader = (BT_DataPacket_Header_t*)Data;

	/* Check if signalling (control) packet data, or user application connection data */
	if (L2CAPDataHeader->DestinationChannel == BT_CHANNEL_SIGNALING)
	{
		BT_Signal_Header_t* SignalCommandHeader = (BT_Signal_Header_t*)L2CAPDataHeader->Payload;

		switch (SignalCommandHeader->Code)
		{
			case BT_SIGNAL_CONNECTION_REQUEST:
				Bluetooth_L2CAP_Signal_ConnectReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONNECTION_RESPONSE:
				Bluetooth_L2CAP_Signal_ConnectRsp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONFIGURATION_REQUEST:
				Bluetooth_L2CAP_Signal_ConfigReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_CONFIGURATION_RESPONSE:
				Bluetooth_L2CAP_Signal_ConfigRsp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_DISCONNECTION_REQUEST:
				Bluetooth_L2CAP_Signal_DisconnectReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_DISCONNECTION_RESPONSE:
				Bluetooth_L2CAP_Signal_DisconnectRsp(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_ECHO_REQUEST:
				Bluetooth_L2CAP_Signal_EchoReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			case BT_SIGNAL_INFORMATION_REQUEST:
				Bluetooth_L2CAP_Signal_InformationReq(StackState, HCIConnection, SignalCommandHeader);
				break;
			default:
				Bluetooth_L2CAP_Signal_CommandRej(StackState, HCIConnection, SignalCommandHeader);
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
	/* Check if there are any pending events in the L2CAP event queue */
	if (StackState->State.L2CAP.PendingEvents)
	{
		BT_L2CAP_Event_t* Event        = &StackState->State.L2CAP.Events[0];
		bool              DequeueEvent = true;
	
		/* Look up the event's associated channel (if one exists) by the local channel number */
		BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_FindChannel(StackState, Event->ConnectionHandle, Event->LocalNumber, 0);

		/* Determine and process the next queued L2CAP event in the event queue, sending command packets as needed */
		if (Event->Event == L2CAP_EVENT_EchoReq)
		{		
			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_ECHO_RESPONSE, Event->Identifier, 0, NULL);
		}
		else if (Event->Event == L2CAP_EVENT_InformationReq)
		{
			struct
			{
				BT_Signal_InformationResp_t InformationResponse;
				uint8_t                     Data[4];
			} ATTR_PACKED ResponsePacket;
			uint8_t DataLen = 0;

			/* Retrieve the requested information and store it in the outgoing packet, if found */
			switch (Event->Result)
			{
				case BT_INFOREQ_MTU:
					ResponsePacket.InformationResponse.Result = CPU_TO_LE16(BT_INFORMATION_SUCCESSFUL);

					DataLen = 2;		
					ResponsePacket.Data[0] = (BT_DEFAULT_L2CAP_CHANNEL_MTU & 0xFF);
					ResponsePacket.Data[1] = (BT_DEFAULT_L2CAP_CHANNEL_MTU >> 8);
					break;
				case BT_INFOREQ_EXTENDEDFEATURES:
					ResponsePacket.InformationResponse.Result = CPU_TO_LE16(BT_INFORMATION_SUCCESSFUL);
					
					DataLen = 4;
					ResponsePacket.Data[0] = (0UL & 0xFF);
					ResponsePacket.Data[1] = (0UL >> 8);
					ResponsePacket.Data[2] = (0UL >> 16);
					ResponsePacket.Data[3] = (0UL >> 24);
					break;
				default:
					ResponsePacket.InformationResponse.Result = CPU_TO_LE16(BT_INFORMATION_NOTSUPPORTED);
					break;
			}

			ResponsePacket.InformationResponse.InfoType = cpu_to_le16(Event->Result);
			
			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_INFORMATION_RESPONSE, Event->Identifier,
											                (sizeof(ResponsePacket.InformationResponse) + DataLen), &ResponsePacket);		
		}
		else if (Event->Event == L2CAP_EVENT_SendRejectReq)
		{
			struct
			{
				BT_Signal_CommandRej_t RejectionRequest;
				uint16_t               Data[2];
			} ATTR_PACKED ResponsePacket;
			uint8_t DataLen = 0;
			
			ResponsePacket.RejectionRequest.Reason = cpu_to_le16(Event->Result);
			
			if (Event->Result == BT_CMDREJ_INVALID_CHANNEL_ID)
			{
				ResponsePacket.Data[0] = cpu_to_le16(Event->LocalNumber);
				ResponsePacket.Data[1] = cpu_to_le16(Event->RemoteNumber);
				DataLen = 4;
			}
			
			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_COMMAND_REJECT, Event->Identifier,
											                (sizeof(ResponsePacket.RejectionRequest) + DataLen), &ResponsePacket);			
		}		
		else if (Event->Event == L2CAP_EVENT_OpenChannelReq)
		{
			BT_Signal_ConnectionReq_t ConnectionRequest;
			ConnectionRequest.PSM                  = cpu_to_le16(L2CAPChannel->PSM);
			ConnectionRequest.SourceChannel        = cpu_to_le16(L2CAPChannel->LocalNumber);
				
			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_CONNECTION_REQUEST, Event->Identifier,
			                                                sizeof(ConnectionRequest), &ConnectionRequest);
			if (DequeueEvent)
			  L2CAPChannel->State = L2CAP_CHANSTATE_WaitConnectRsp;
		}
		else if (Event->Event == L2CAP_EVENT_CloseChannelReq)
		{
			BT_Signal_DisconnectionReq_t DisconnectionRequest;
			DisconnectionRequest.DestinationChannel = cpu_to_le16(L2CAPChannel->RemoteNumber);
			DisconnectionRequest.SourceChannel      = cpu_to_le16(L2CAPChannel->LocalNumber);

			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_DISCONNECTION_REQUEST, Event->Identifier,
			                                                sizeof(DisconnectionRequest), &DisconnectionRequest);
			if (DequeueEvent)
			  L2CAPChannel->State = L2CAP_CHANSTATE_WaitDisconnectRsp;
		}
		else if (Event->Event == L2CAP_EVENT_SendConfigReq)
		{
			struct
			{
				BT_Signal_ConfigurationReq_t ConfigurationRequest;
				struct
				{
					BT_Config_Option_Header_t Header;
					uint16_t Value;
				} Option_LocalMTU;
			} ATTR_PACKED PacketData;

			PacketData.ConfigurationRequest.DestinationChannel = cpu_to_le16(L2CAPChannel->RemoteNumber);
			PacketData.ConfigurationRequest.Flags    = 0;
			PacketData.Option_LocalMTU.Header.Type   = CPU_TO_LE16(BT_CONFIG_OPTION_MTU);
			PacketData.Option_LocalMTU.Header.Length = sizeof(PacketData.Option_LocalMTU.Value);
			PacketData.Option_LocalMTU.Value         = cpu_to_le16(L2CAPChannel->LocalMTU);

			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_CONFIGURATION_REQUEST, Event->Identifier,
											                sizeof(PacketData), &PacketData);			
		}
		else if (Event->Event == L2CAP_EVENT_ConnectReq)
		{
			BT_Signal_ConnectionResp_t ConnectionResponse;
			ConnectionResponse.DestinationChannel = (L2CAPChannel) ? cpu_to_le16(L2CAPChannel->LocalNumber) : 0;
			ConnectionResponse.SourceChannel      = cpu_to_le16(Event->RemoteNumber);
			ConnectionResponse.Result             = cpu_to_le16(Event->Result);
			ConnectionResponse.Status             = 0x00;
			
			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_CONNECTION_RESPONSE, Event->Identifier,
			                                                sizeof(ConnectionResponse), &ConnectionResponse);
			if ((DequeueEvent) && (L2CAPChannel))
			  L2CAPChannel->State = (Event->Result == BT_CONNECTION_SUCCESSFUL) ? L2CAP_CHANSTATE_Config_WaitConfig : L2CAP_CHANSTATE_Closed;
		}
		else if (Event->Event == L2CAP_EVENT_ConnectRsp)
		{
			if (L2CAPChannel)
			{
				if (Event->Result == BT_CONNECTION_SUCCESSFUL)
				  L2CAPChannel->State = L2CAP_CHANSTATE_Config_WaitConfig;
				else if (Event->Result != BT_CONNECTION_PENDING)
				  L2CAPChannel->State = L2CAP_CHANSTATE_Closed;
			}
		}
		else if (Event->Event == L2CAP_EVENT_ConfigReq)
		{
			BT_Signal_ConfigurationResp_t ConfigurationResponse;
			ConfigurationResponse.SourceChannel = cpu_to_le16(L2CAPChannel->RemoteNumber);
			ConfigurationResponse.Flags         = 0x00;
			ConfigurationResponse.Result        = cpu_to_le16(Event->Result);

			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_CONFIGURATION_RESPONSE, Event->Identifier,
			                                                sizeof(ConfigurationResponse), &ConfigurationResponse);			
			if ((DequeueEvent) && (L2CAPChannel) && (Event->Result == BT_CONFIGURATION_SUCCESSFUL))
			{
				switch (L2CAPChannel->State)
				{
					case L2CAP_CHANSTATE_Config_WaitReq:
						L2CAPChannel->State = L2CAP_CHANSTATE_Open;
						EVENT_Bluetooth_ChannelStateChange(StackState, L2CAPChannel);
						break;
					case L2CAP_CHANSTATE_Config_WaitConfig:
						L2CAPChannel->State = L2CAP_CHANSTATE_Config_WaitSendConfig;
						break;
					case L2CAP_CHANSTATE_Config_WaitReqResp:
						L2CAPChannel->State = L2CAP_CHANSTATE_Config_WaitResp;
						break;
				}
			}
		}
		else if (Event->Event == L2CAP_EVENT_ConfigRsp)
		{
			if ((L2CAPChannel) && (Event->Result == BT_CONFIGURATION_SUCCESSFUL))
			{
				switch (L2CAPChannel->State)
				{
					case L2CAP_CHANSTATE_Config_WaitReqResp:
						L2CAPChannel->State = L2CAP_CHANSTATE_Config_WaitReq;
						break;
					case L2CAP_CHANSTATE_Config_WaitResp:
						L2CAPChannel->State = L2CAP_CHANSTATE_Open;
						EVENT_Bluetooth_ChannelStateChange(StackState, L2CAPChannel);
						break;
				}
			}
		}
		else if (Event->Event == L2CAP_EVENT_DisconnectReq)
		{
			BT_Signal_DisconnectionResp_t DisconnectionResponse;
			DisconnectionResponse.DestinationChannel = cpu_to_le16(Event->LocalNumber);
			DisconnectionResponse.SourceChannel      = cpu_to_le16(Event->RemoteNumber);

			DequeueEvent = Bluetooth_L2CAP_SendSignalPacket(StackState, Event->ConnectionHandle, BT_SIGNAL_DISCONNECTION_RESPONSE, Event->Identifier,
	                                                        sizeof(DisconnectionResponse), &DisconnectionResponse);
			if ((DequeueEvent) && (L2CAPChannel))
			{
				L2CAPChannel->State = L2CAP_CHANSTATE_Closed;
				EVENT_Bluetooth_ChannelStateChange(StackState, L2CAPChannel);
			}
		}
		else if (Event->Event == L2CAP_EVENT_DisconnectRsp)
		{			
			if ((L2CAPChannel) && (L2CAPChannel->State == L2CAP_CHANSTATE_WaitDisconnectRsp))
			{
				L2CAPChannel->State = L2CAP_CHANSTATE_Closed;
				EVENT_Bluetooth_ChannelStateChange(StackState, L2CAPChannel);
			}
		}
	
		/* If the event was processed, dequeue it from the event queue */
		if (DequeueEvent)
		{
			memmove(&StackState->State.L2CAP.Events[0], &StackState->State.L2CAP.Events[1], sizeof(BT_L2CAP_Event_t) * StackState->State.L2CAP.PendingEvents);
			StackState->State.L2CAP.PendingEvents--;
			return true;
		}
	}
	
	/* Check for any half-open channels, send configuration details to the remote device if found */
	for (uint8_t i = 0; i < BT_MAX_LOGICAL_CHANNELS; i++)
	{
		BT_L2CAP_Channel_t* CurrentChannel = &StackState->State.L2CAP.Channels[i];
		
		if ((CurrentChannel->State != L2CAP_CHANSTATE_Config_WaitConfig) && (CurrentChannel->State != L2CAP_CHANSTATE_Config_WaitSendConfig))
		  continue;

		/* Find the associated HCI connection via the connection handle indicated in the L2CAP packet header */
		BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, CurrentChannel->ConnectionHandle);

		/* If the HCI connection is invalid, abort the configuration */
		if (!(HCIConnection))
		  continue;

		BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, ++HCIConnection->CurrentIdentifier);
	
		if (!(Event))
		  return false;

		Event->Event            = L2CAP_EVENT_SendConfigReq;
		Event->LocalNumber      = CurrentChannel->LocalNumber;
		Event->ConnectionHandle = HCIConnection->Handle;

		if (CurrentChannel->State == L2CAP_CHANSTATE_Config_WaitConfig)
		  CurrentChannel->State = L2CAP_CHANSTATE_Config_WaitReqResp;
		else if (CurrentChannel->State == L2CAP_CHANSTATE_Config_WaitSendConfig)
		  CurrentChannel->State = L2CAP_CHANSTATE_Config_WaitResp;
				  
		return true;
	}
	
	return false;
}

/** Opens a new channel to the given device on the given PSM.
 *
 *  \param[in, out] StackState     Pointer to a Bluetooth Stack state table.
 *  \param[in, out] HCIConnection  Handle to the HCI connection the L2CAP channel is to be opened upon.
 *  \param[in]      PSM            Protocol the opened channel is to be assicated with.
 *
 *  \return Handle to a L2CAP channel object if one was created, \c NULL otherwise.
 */
BT_L2CAP_Channel_t* const Bluetooth_L2CAP_OpenChannel(BT_StackConfig_t* const StackState,
                                                      BT_HCI_Connection_t* const HCIConnection,
                                                      const uint16_t PSM)
{	  
	/* Create a new channel object from the requested parameters if space exists in the connection table */
	BT_L2CAP_Channel_t* L2CAPChannel = Bluetooth_L2CAP_NewChannel(StackState, HCIConnection->Handle, 0, PSM);

	/* If no space left in the connection table, abort */
	if (!(L2CAPChannel))
	  return NULL;

	L2CAPChannel->LocallyInitiated = true;

	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, ++HCIConnection->CurrentIdentifier);
	
	if (!(Event))
	{
		L2CAPChannel->State = L2CAP_CHANSTATE_Closed;
		return NULL;
	}

	Event->Event            = L2CAP_EVENT_OpenChannelReq;
	Event->LocalNumber      = L2CAPChannel->LocalNumber;
	Event->ConnectionHandle = HCIConnection->Handle;

	return L2CAPChannel;
}

/** Closes the given L2CAP channel.
 *
 *  \param[in, out] StackState    Pointer to a Bluetooth Stack state table.
 *  \param[in, out] L2CAPChannel  Handle to the L2CAP channel object which is to be closed.
 */
void Bluetooth_L2CAP_CloseChannel(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const L2CAPChannel)
{
	/* Don't try to close a non-existing or already closed channel */
	if (!(L2CAPChannel) || (L2CAPChannel->State == L2CAP_CHANSTATE_Closed))
	  return;

	/* Find the associated HCI connection via the connection handle indicated in the L2CAP packet header */
	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, L2CAPChannel->ConnectionHandle);

	/* If the HCI connection is invalid, abort the configuration */
	if (!(HCIConnection))
	  return;

	BT_L2CAP_Event_t* Event = Bluetooth_L2CAP_NewEvent(StackState, HCIConnection->Handle, ++HCIConnection->CurrentIdentifier);
	
	if (!(Event))
	  return;
	  
	Event->Event            = L2CAP_EVENT_CloseChannelReq;
	Event->LocalNumber      = L2CAPChannel->LocalNumber;
	Event->ConnectionHandle = HCIConnection->Handle;
}

/** Sends the given data through the given established L2CAP channel to a remote device.
 *
 *  \param[in, out] StackState    Pointer to a Bluetooth Stack state table.
 *  \param[in, out] L2CAPChannel  Handle to the established L2CAP channel object which is to be used.
 *  \param[in]      Length        Length of the data to send to the remote device, in bytes.
 *  \param[in]      Data          Data to send to the remote device.
 *
 *  \return Boolean \c true if the data was sent sucessfully, \c false otherwise.
 */
bool Bluetooth_L2CAP_SendPacket(BT_StackConfig_t* const StackState,
                                BT_L2CAP_Channel_t* const L2CAPChannel,
                                const uint16_t Length,
                                const void* Data)
{
	/* If the destination channel is not the signaling channel and it is not currently fully open, abort */
	if (!(L2CAPChannel) || (L2CAPChannel->State != L2CAP_CHANSTATE_Open))
	  return false;

	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, L2CAPChannel->ConnectionHandle);

	/* If an open device connection with the correct connection handle was not found, abort */
	if (!(HCIConnection))
	  return false;

	/* Check if there is any space in the buffer for the current connection */
	if (HCIConnection->DataPacketsQueued == StackState->State.HCI.ACLDataPackets)
	  return false;
	
	/* Determine the length of the first fragment (includes L2CAP data packet header) */
	uint16_t BytesInPacket  = MIN(Length, (L2CAPChannel->RemoteMTU - sizeof(BT_DataPacket_Header_t)));
	uint16_t BytesRemaining = Length;
	
	struct
	{
		BT_DataPacket_Header_t L2CAPDataHeader;
		uint8_t                Data[BytesInPacket];
	} ATTR_PACKED FirstPacket;

	FirstPacket.L2CAPDataHeader.PayloadLength      = cpu_to_le16(Length);
	FirstPacket.L2CAPDataHeader.DestinationChannel = cpu_to_le16(L2CAPChannel->RemoteNumber);
	memcpy(FirstPacket.Data, Data, BytesInPacket);
	
	if (!(Bluetooth_HCI_SendPacket(StackState, HCIConnection, false, sizeof(FirstPacket), &FirstPacket)))
	  return false;

	BytesRemaining -= BytesInPacket;
	Data           += BytesInPacket;
		
	/* Split the remaining data to send into multiple packets if required to suit the receiver's MTU */
	while (BytesRemaining)
	{
		/* Determine the length of the next fragment */
		BytesInPacket = MIN(BytesRemaining, L2CAPChannel->RemoteMTU);
		
		if (!(Bluetooth_HCI_SendPacket(StackState, HCIConnection, true, BytesInPacket, Data)))
		  return false;

		BytesRemaining -= BytesInPacket;
		Data           += BytesInPacket;
	}
	
	return true;
}

