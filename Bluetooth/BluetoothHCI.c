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
 *  Bluetooth HCI layer management code. This module manages the overall
 *  Bluetooth stack connection state to and from other devices, processes
 *  received events from the Bluetooth controller, and issues commands to
 *  modify the controller's configuration, such as the broadcast name of the
 *  device.
 */

#include "BluetoothHCI.h"

/** Finds the corresponding Bluetooth device connection from the remote device's address or link handle.
 *
 *  \param[in, out] StackState    Pointer to a Bluetooth Stack state table.
 *  \param[in]      RemoteBDADDR  Bluetooth device address of the remote device to search for, \c NULL to search by link handle.
 *  \param[in]      Handle        If \c RemoteBDADDR is \c NULL, the link handle to search for.
 *
 *  \return Pointer to the device connection handle if one was found, NULL otherwise.
 */
BT_HCI_Connection_t* const Bluetooth_HCI_FindConnection(BT_StackConfig_t* const StackState,
                                                        const uint8_t* const RemoteBDADDR,
                                                        const uint16_t Handle)
{
	/* Search through the HCI connection table, looking for a matching connection */
	for (uint8_t i = 0; i < BT_MAX_DEVICE_CONNECTIONS; i++)
	{
		BT_HCI_Connection_t* Connection = &StackState->State.HCI.Connections[i];
		
		/* Free connection entries are inactive must be skipped */
		if (Connection->State == HCI_CONSTATE_Closed)
		  continue;

		/* Check search parameter; if BDADDR specified search by remote device address, otherwise by handle */
		if (RemoteBDADDR != NULL)
		{
			if (memcmp(Connection->RemoteBDADDR, RemoteBDADDR, sizeof(BDADDR_t)) == 0)
			  return Connection;
		}
		else
		{
			if (Connection->Handle == (Handle & BT_HCI_CONNECTION_HANDLE_MASK))
			  return Connection;		
		}
	}
	
	return NULL;
}

/** Creates a new Bluetooth device connection, if space is available in the given Bluetooth stack state HCI connection table.
 *
 *  \param[in, out] StackState    Pointer to a Bluetooth Stack state table.
 *  \param[in]      RemoteBDADDR  Bluetooth device address of the remote device.
 *  \param[in]      LinkType      Type of Bluetooth link, a value from \ref BT_LinkTypes_t.
 *
 *  \return Pointer to the new connection handle if one was created, NULL otherwise.
 */
static BT_HCI_Connection_t* const Bluetooth_HCI_NewConnection(BT_StackConfig_t* const StackState,
                                                              const uint8_t* const RemoteBDADDR,
                                                              const uint8_t LinkType)
{
	for (uint8_t i = 0; i < BT_MAX_DEVICE_CONNECTIONS; i++)
	{
		BT_HCI_Connection_t* Connection = &StackState->State.HCI.Connections[i];

		if (Connection->State == HCI_CONSTATE_Closed)
		{
			memcpy(Connection->RemoteBDADDR, RemoteBDADDR, sizeof(BDADDR_t));
			Connection->State             = HCI_CONSTATE_New;
			Connection->DataPacketsQueued = 0;
			Connection->LinkType          = LinkType;
			Connection->CurrentIdentifier = 0x01;
			Connection->LocallyInitiated  = false;
			return Connection;
		}
	}
	
	return NULL;
}

/** Initalizes the Bluetooth HCI layer, ready for new device connections.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_HCI_Init(BT_StackConfig_t* const StackState)
{
	StackState->State.HCI.CommandPackets  = 1;
	StackState->State.HCI.State           = HCISTATE_Init_Reset;
	StackState->State.HCI.StateTransition = true;

	for (uint8_t i = 0; i < BT_MAX_DEVICE_CONNECTIONS; i++)
	  StackState->State.HCI.Connections[i].State = HCI_CONSTATE_Closed;
}

/** Manages the timeouts within the HCI command portion of the controller, each time one tick period has elapsed.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_HCI_TickElapsed(BT_StackConfig_t* const StackState)
{
	/* If the stack's HCI timeout period has expired, reset it and fire a new state transition */
	if (StackState->State.HCI.TicksElapsed++ == (BT_HCI_COMMAND_TIMEOUT_MS / BT_TICK_MS))
	{
		StackState->State.HCI.TicksElapsed    = 0;
		StackState->State.HCI.StateTransition = true;
	}
}

/** Processes a recieved Bluetooth HCI Event packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_HCI_ProcessEventPacket(BT_StackConfig_t* const StackState)
{
	BT_HCIEvent_Header_t* HCIEventHeader = (BT_HCIEvent_Header_t*)StackState->Config.PacketBuffer;

	if (HCIEventHeader->EventCode == EVENT_COMMAND_COMPLETE)
	{
		BT_HCIEvent_CommandComplete_t* CommandCompleteHeader = (BT_HCIEvent_CommandComplete_t*)&HCIEventHeader->Parameters;
		
		/* Update number of allowable command packets to the controller */
		StackState->State.HCI.CommandPackets = CommandCompleteHeader->Packets;
		
		uint8_t NextHCIState = StackState->State.HCI.State;
		
		/* Determine the next state to change to according to the current state and which command completed */
		switch (StackState->State.HCI.State)
		{
			case HCISTATE_Init_Reset:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_RESET))
				  NextHCIState = HCISTATE_Init_ReadBufferSize;
				break;
			case HCISTATE_Init_ReadBufferSize:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE))
				{
					NextHCIState = HCISTATE_Init_GetBDADDR;
				  
					/* Store the total number of ACL data packets the controller is able to queue internally */
					StackState->State.HCI.ACLDataPackets = (((uint16_t)CommandCompleteHeader->Parameters[5] << 8) | CommandCompleteHeader->Parameters[4]);
				}
				break;
			case HCISTATE_Init_GetBDADDR:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBDADDR))
				{
					NextHCIState = HCISTATE_Init_SetLocalName;

					/* Copy over the returned local device address to the stack state buffer */
					memcpy(StackState->State.HCI.LocalBDADDR, &CommandCompleteHeader->Parameters[1], sizeof(BDADDR_t));				
				}
				break;
			case HCISTATE_Init_SetLocalName:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME))
				  NextHCIState = HCISTATE_Init_SetDeviceClass;
				break;
			case HCISTATE_Init_SetDeviceClass:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE))
				  NextHCIState = HCISTATE_Init_SetScanEnable;
				break;
			case HCISTATE_Init_SetScanEnable:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE))
				  NextHCIState = HCISTATE_Idle;
				break;
		}
		
		StackState->State.HCI.StateTransition = (StackState->State.HCI.State != NextHCIState);
		StackState->State.HCI.State           = NextHCIState;		
	}
	else if (HCIEventHeader->EventCode == EVENT_COMMAND_STATUS)
	{
		BT_HCIEvent_CommandStatus_t* CommandStatusHeader = (BT_HCIEvent_CommandStatus_t*)&HCIEventHeader->Parameters;
		
		/* Update number of allowable command packets to the controller */
		StackState->State.HCI.CommandPackets = CommandStatusHeader->Packets;
	}
	else if (HCIEventHeader->EventCode == EVENT_NUM_PACKETS_COMPLETE)
	{
		BT_HCIEvent_NumPacketsComplete_t* CurrentPacketInfo = (BT_HCIEvent_NumPacketsComplete_t*)&HCIEventHeader->Parameters;
		
		/* For each connection reported in the event, find the associated connection object and decrease the number of
		 * queued packets by the number of completed packets reported for each connection */
		for (uint8_t i = 0; i < CurrentPacketInfo->Handles; i++)
		{
			BT_HCI_Connection_t* Connection = Bluetooth_HCI_FindConnection(StackState, 0, CurrentPacketInfo->PacketInfo[i].Handle);

			if (Connection)
			  Connection->DataPacketsQueued -= le16_to_cpu(CurrentPacketInfo->PacketInfo[i].PacketsCompleted);
		}
	}
	else if (HCIEventHeader->EventCode == EVENT_CONNECTION_REQUEST)
	{
		BT_HCIEvent_ConnectionRequest_t* ConnectionRequestEventHeader = (BT_HCIEvent_ConnectionRequest_t*)&HCIEventHeader->Parameters;

		/* Try to create a new connection handle for the connection request */
		BT_HCI_Connection_t* Connection      = Bluetooth_HCI_NewConnection(StackState, ConnectionRequestEventHeader->RemoteBDADDR, ConnectionRequestEventHeader->LinkType);		
		uint8_t              RejectionReason = HCI_ERROR_LIMITED_RESOURCES;
		
		/* If a connection handle was created, fire user-application callback to accept or reject the request - otherwise reject with a LIMITED RESOURCES error code */
		if (Connection)
		  RejectionReason = CALLBACK_Bluetooth_ConnectionRequest(StackState, Connection) ? 0 : HCI_ERROR_UNACCEPTABLE_BDADDR;
		
		/* If a rejection reason was given, reject the request, otherwise send a connection request accept */
		if (RejectionReason)
		{
			BT_HCICommand_RejectConnectionReq_t HCIRejectConnectionHeader;

			memcpy(HCIRejectConnectionHeader.RemoteBDADDR, ConnectionRequestEventHeader->RemoteBDADDR, sizeof(BDADDR_t));
			HCIRejectConnectionHeader.Reason = RejectionReason;
			
			/* Mark the connection as free again as it was rejected */
			if (Connection)
			  Connection->State = HCI_CONSTATE_Closed;

			Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_REJECT_CONNECTION_REQUEST), sizeof(HCIRejectConnectionHeader), &HCIRejectConnectionHeader);			
		}
		else
		{
			BT_HCICommand_AcceptConnectionReq_t HCIAcceptConnectionHeader;

			memcpy(HCIAcceptConnectionHeader.RemoteBDADDR, ConnectionRequestEventHeader->RemoteBDADDR, sizeof(BDADDR_t));
			HCIAcceptConnectionHeader.SlaveRole = true;
			
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST), sizeof(HCIAcceptConnectionHeader), &HCIAcceptConnectionHeader);			
		}
	}
	else if (HCIEventHeader->EventCode == EVENT_CONNECTION_COMPLETE)
	{
		BT_HCIEvent_ConnectionComplete_t* ConnectionCompleteEventHeader = (BT_HCIEvent_ConnectionComplete_t*)&HCIEventHeader->Parameters;		

		/* Find the existing connection handle in the HCI device connection table if it exists, from the remote device BDADDR */
		BT_HCI_Connection_t* Connection = Bluetooth_HCI_FindConnection(StackState, ConnectionCompleteEventHeader->RemoteBDADDR, 0);
		
		/* If the connection handle exists, store the created connection handle and change the device connection state to connected */
		if (Connection)
		{
			Connection->Handle = (le16_to_cpu(ConnectionCompleteEventHeader->Handle) & BT_HCI_CONNECTION_HANDLE_MASK);
			Connection->State  = (ConnectionCompleteEventHeader->Status == HCI_ERROR_SUCCESS) ? HCI_CONSTATE_Connected : HCI_CONSTATE_Failed;
			
			/* Fire user application callback to signal the connection completion */
			EVENT_Bluetooth_ConnectionStateChange(StackState, Connection);
			
			/* Once the user application callback has fired with the failed connection state, free up the connection entry */
			if (Connection->State == HCI_CONSTATE_Failed)
			  Connection->State = HCI_CONSTATE_Closed;
		}
	}
	else if (HCIEventHeader->EventCode == EVENT_DISCONNECTION_COMPLETE)
	{
		BT_HCIEvent_DisconnectionComplete_t* DisconnectionCompleteEventHeader = (BT_HCIEvent_DisconnectionComplete_t*)&HCIEventHeader->Parameters;

		/* Find the existing connection handle in the HCI device connection table if it exists, from the remote device BDADDR */
		BT_HCI_Connection_t* Connection = Bluetooth_HCI_FindConnection(StackState, NULL, le16_to_cpu(DisconnectionCompleteEventHeader->Handle));

		/* If an existing connection was found, close it and fire user application callback to signal the disconnection completion */
		if (Connection)
		{
			Connection->State = HCI_CONSTATE_Closed;
			
			Bluetooth_L2CAP_NotifyHCIDisconnection(StackState, Connection->Handle);			
			EVENT_Bluetooth_ConnectionStateChange(StackState, Connection);
		}
	}
	else if (HCIEventHeader->EventCode == EVENT_PIN_CODE_REQUEST)
	{
		BT_HCIEvent_PinCodeReq_t* PINCodeRequestEventHeader = (BT_HCIEvent_PinCodeReq_t*)&HCIEventHeader->Parameters;		

		/* Check if a PIN code has been set of one or more characters */
		if ((StackState->Config.PINCode != NULL) && strlen(StackState->Config.PINCode))
		{
			BT_HCICommand_PinCodeACKResp_t PINKeyACKResponse;
			
			memcpy(PINKeyACKResponse.RemoteBDADDR, PINCodeRequestEventHeader->RemoteBDADDR, sizeof(BDADDR_t));
			PINKeyACKResponse.PINCodeLength = MIN(strlen(StackState->Config.PINCode), sizeof(PINKeyACKResponse.PINCode));
			strlcpy((void*)PINKeyACKResponse.PINCode, StackState->Config.PINCode, sizeof(PINKeyACKResponse.PINCode));
			
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_PIN_CODE_REQUEST_REPLY), sizeof(PINKeyACKResponse), &PINKeyACKResponse);
		}
		else
		{
			BT_HCICommand_PinCodeNAKResp_t PINKeyNAKResponse;

			memcpy(PINKeyNAKResponse.RemoteBDADDR, PINCodeRequestEventHeader->RemoteBDADDR, sizeof(BDADDR_t));			
			
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_PIN_CODE_REQUEST_NEG_REPLY), sizeof(PINKeyNAKResponse), &PINKeyNAKResponse);
		}
	}
	else if (HCIEventHeader->EventCode == EVENT_LINK_KEY_REQUEST)
	{
		BT_HCIEvent_LinkKeyReq_t*      LinkKeyRequestEventHeader = (BT_HCIEvent_LinkKeyReq_t*)&HCIEventHeader->Parameters;
		BT_HCICommand_LinkKeyNAKResp_t LinkKeyRequestNAKHeader;

		memcpy(LinkKeyRequestNAKHeader.RemoteBDADDR, LinkKeyRequestEventHeader->RemoteBDADDR, sizeof(BDADDR_t));

		Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_LINK_KEY_REQUEST_NEG_REPLY), sizeof(LinkKeyRequestNAKHeader), &LinkKeyRequestNAKHeader);
	}	
}

/** Processes a recieved Bluetooth HCI Data packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_HCI_ProcessDataPacket(BT_StackConfig_t* const StackState)
{
	BT_HCIData_Header_t* HCIDataHeader = (BT_HCIData_Header_t*)StackState->Config.PacketBuffer;
	
	/* If an open device connection with the correct connection handle was not foumd, abort */	
	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_FindConnection(StackState, NULL, HCIDataHeader->Handle);

	/* If an open device connection with the correct connection handle was not foumd, abort */
	if (!(HCIConnection))
	  return;
	
	Bluetooth_L2CAP_ProcessPacket(StackState, HCIConnection, HCIDataHeader->Data);
}

/** Manages the existing HCI layer connections of a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *
 *  \return Boolean \c true if more HCI management tasks are pending, \c false otherwise.
 */
bool Bluetooth_HCI_Manage(BT_StackConfig_t* const StackState)
{
	/* Only process HCI state transitions (one action per transition) when the controller is willing to accept a new command */
	if (!(StackState->State.HCI.StateTransition) || !(StackState->State.HCI.CommandPackets))
	  return false;
	
	/* Reset HCI layer timeout counter */
	StackState->State.HCI.TicksElapsed = 0;
	
	uint8_t ParamBuffer[3];
	
	/* Determine if a packet needs to be sent based on the current HCI state */
	switch (StackState->State.HCI.State)
	{
		case HCISTATE_Init_Reset:
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_RESET), 0, NULL);
			break;
		case HCISTATE_Init_ReadBufferSize:
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE), 0, NULL);
			break;
		case HCISTATE_Init_GetBDADDR:
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBDADDR), 0, NULL);
			break;
		case HCISTATE_Init_SetLocalName:
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME), 248, StackState->Config.Name);
			break;
		case HCISTATE_Init_SetDeviceClass:
			ParamBuffer[0] = (StackState->Config.Class & 0xFF);
			ParamBuffer[1] = (StackState->Config.Class >> 8);
			ParamBuffer[2] = (StackState->Config.Class >> 16);
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE), 3, ParamBuffer);
			break;
		case HCISTATE_Init_SetScanEnable:
			ParamBuffer[0] = (StackState->Config.Discoverable ? BT_SCANMODE_InquiryAndPageScans : BT_SCANMODE_NoScansEnabled);
			Bluetooth_HCI_SendControlPacket(StackState, (OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE), 1, ParamBuffer);
			break;
		case HCISTATE_Idle:
			EVENT_Bluetooth_InitComplete(StackState);
			break;
	}

	StackState->State.HCI.StateTransition = false;
	return true;
}

/** Initiates a connection to a remote Bluetooth device, referenced by its Bluetooth BDADDR.
 *
 *  \param[in, out] StackState    Pointer to a Bluetooth Stack state table.
 *  \param[in]      RemoteBDADDR  Address of the remote device to connect to.
 *  \param[in]      LinkType      Type of link to establish, a value from \ref BT_LinkTypes_t.
 *
 *  \return Handle to the created HCI connection if one was made, \c NULL otherwise.
 */
BT_HCI_Connection_t* Bluetooth_HCI_Connect(BT_StackConfig_t* const StackState,
                                           const uint8_t* const RemoteBDADDR,
                                           const uint8_t LinkType)
{
	/* Disallow connections until the stack is ready */
	if (StackState->State.HCI.State != HCISTATE_Idle)
	  return NULL;

	/* Abort if not enough space in the controller for the command */
	if (!(StackState->State.HCI.CommandPackets))
	  return NULL;

	/* Only ACL connections are implemented at present, reject other types */
	if (LinkType != LINK_TYPE_ACL)
	  return NULL;
	
	/* Create a new HCI connection entry in the stack's HCI connection table */
	BT_HCI_Connection_t* HCIConnection = Bluetooth_HCI_NewConnection(StackState, RemoteBDADDR, LinkType);

	/* Abort if no space was found in the HCI connection table */
	if (!(HCIConnection))
	  return NULL;
	
	/* Indicate that the connection is currently attempting a connection to the remote device */
	HCIConnection->State                 = HCI_CONSTATE_Connecting;
	HCIConnection->LocallyInitiated      = true;
	
	/* Fill out HCI connection parameters - assume role switch allowed for now, and all packet types supported */
	BT_HCICommand_CreateConnection_t CreateConnectHeader;
	memcpy(CreateConnectHeader.RemoteBDADDR, RemoteBDADDR, sizeof(BDADDR_t));
	CreateConnectHeader.PacketType      = CPU_TO_LE16(0xCC18);
	CreateConnectHeader.PageScanMode    = 1;
	CreateConnectHeader.Reserved        = 0;
	CreateConnectHeader.ClockOffset     = CPU_TO_LE16(0);
	CreateConnectHeader.AllowRoleSwitch = true;
	
	/* If the command packet failed to send, kill the created channel object */
	if (!(Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_CREATE_CONNECTION), sizeof(CreateConnectHeader), &CreateConnectHeader)))
	{
		HCIConnection->State = HCI_CONSTATE_Closed;
		return NULL;
	}
	
	return HCIConnection;
}

/** Disconnects from an established HCI connection to a remote device.
 *
 *  \param[in, out] StackState     Pointer to a Bluetooth Stack state table.
 *  \param[in, out] HCIConnection  Handle of the HCI connection to close.
 *
 *  \return Boolean \c true if the connection was disconnected, \c false otherwise.
 */
bool Bluetooth_HCI_Disconnect(BT_StackConfig_t* const StackState,
                              BT_HCI_Connection_t* const HCIConnection)
{
	/* Disallow connections until the stack is ready */
	if (StackState->State.HCI.State != HCISTATE_Idle)
	  return false;
	  
	/* Abort if invalid connection handle was given */
	if (!(HCIConnection))
	  return false;

	struct 
	{
		uint16_t ConnectionHandle;
		uint8_t  Reason;
	} ATTR_PACKED DisconnectParams;
	
	DisconnectParams.ConnectionHandle = cpu_to_le16(HCIConnection->Handle);
	DisconnectParams.Reason           = HCI_ERROR_REMOTE_USER_TERMINATED_CONN;

	switch (HCIConnection->State)
	{
		case HCI_CONSTATE_Connecting:
			/* Can only terminate connections initiated from the local device (not remote devices) */
			if (!(HCIConnection->LocallyInitiated))
			  return false;
		
			return Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_CREATE_CONNECTION_CANCEL), sizeof(BDADDR_t), HCIConnection->RemoteBDADDR);
		case HCI_CONSTATE_Connected:
			return Bluetooth_HCI_SendControlPacket(StackState, (OGF_LINK_CONTROL | OCF_LINK_CONTROL_DISCONNECT), sizeof(DisconnectParams), &DisconnectParams);
	}

	return true;
}

/** Sends a HCI command packet to the Bluetooth adapter for transmittion to a remote device.
 *
 *  \param[in, out] StackState Pointer to a Bluetooth Stack state table.
 *  \param[in]      OpCode     HCI command operation to send to the controller.
 *  \param[in]      Length     Length of the command parameters to send in bytes.
 *  \param[in]      Data       Command data to send to the Bluetooth controller.
 *
 *  \return Boolean \c true if the data was sent, \c false otherwise.
 */ 
bool Bluetooth_HCI_SendControlPacket(BT_StackConfig_t* const StackState,
                                     const uint16_t OpCode,
		                             const uint8_t Length,
		                             const void* Data)
{
	/* Abort if not enough space in the controller for the command */
	if (!(StackState->State.HCI.CommandPackets))
	  return false;

	BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.PacketBuffer;
	HCICommandHeader->OpCode          = cpu_to_le16(OpCode),
	HCICommandHeader->ParameterLength = Length;
	memcpy(&HCICommandHeader->Parameters[0], Data, Length);

	/* Keep track of how many command packets can be queued into the controller */
	StackState->State.HCI.CommandPackets--;

	CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + Length));
	return true;
}

/** Sends a HCI data packet to the Bluetooth adapter for transmittion to a remote device.
 *
 *  \param[in, out] StackState     Pointer to a Bluetooth Stack state table.
 *  \param[in, out] HCIConnection  Handle of the HCI connection to send the data on.
 *  \param[in]      Length         Length of the data to send in bytes.
 *  \param[in]      Data           Data to send through the nominated HCI connection.
 *
 *  \return Boolean \c true if the data was sent, \c false otherwise.
 */ 
bool Bluetooth_HCI_SendPacket(BT_StackConfig_t* const StackState,
                              BT_HCI_Connection_t* const HCIConnection,
		                      const uint16_t Length,
		                      const void* Data)
{
	if (StackState->State.HCI.State != HCISTATE_Idle)
	  return false;

	if (!(HCIConnection) || (HCIConnection->State != HCI_CONSTATE_Connected))
	  return false;
	  
	if (HCIConnection->DataPacketsQueued == StackState->State.HCI.ACLDataPackets)
	  return false;
	
	/* Keep track of how many packets have been queued into the controller for the connection to prevent buffer overrun */
	HCIConnection->DataPacketsQueued++;

	BT_HCIData_Header_t* HCIDataHeader = (BT_HCIData_Header_t*)StackState->Config.PacketBuffer;
	HCIDataHeader->Handle     = cpu_to_le16(HCIConnection->Handle | BT_L2CAP_FIRST_AUTOFLUSH);
	HCIDataHeader->DataLength = cpu_to_le16(Length);
	memcpy(HCIDataHeader->Data, Data, Length);

	CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCIData, (sizeof(BT_HCIData_Header_t) + Length));
	return true;
}

