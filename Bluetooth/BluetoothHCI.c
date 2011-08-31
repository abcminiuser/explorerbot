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

#include "BluetoothHCI.h"

/** Creates a new Bluetooth device connection, if space is available in the given Bluetooth stack state HCI connection table.
 *
 *  \param[in, out] StackState    Pointer to a Bluetooth Stack state table.
 *  \param[in]      RemoteBDADDR  Bluetooth device address of the remote device.
 *  \param[in]      LinkType      Type of Bluetooth link, a value from \ref BT_LinkTypes_t.
 *
 *  \return Pointer to the new connection handle if one was created, NULL otherwise.
 */
static BT_HCI_Connection_t* Bluetooth_HCI_NewConnection(BT_StackConfig_t* const StackState,
                                                        const uint8_t* const RemoteBDADDR,
                                                        const uint8_t LinkType)
{
	for (uint8_t i = 0; i < MAX_DEVICE_CONNECTIONS; i++)
	{
		BT_HCI_Connection_t* Connection = &StackState->State.HCI.Connections[i];

		if (Connection->State == HCI_CONSTATE_Free)
		{
			memcpy(Connection->RemoteBDADDR, RemoteBDADDR, BT_BDADDR_LEN);
			Connection->State             = HCI_CONSTATE_New;
			Connection->LinkType          = LinkType;
			Connection->CurrentIdentifier = 0x01;
			return Connection;
		}
	}
	
	return NULL;
}

/** Finds the corresponding Bluetooth device connection from the remote device's address or link handle.
 *
 *  \param[in, out] StackState    Pointer to a Bluetooth Stack state table.
 *  \param[in]      RemoteBDADDR  Bluetooth device address of the remote device to search for, \c NULL to search by link handle.
 *  \param[in]      Handle        If \c RemoteBDADDR is \c NULL, the link handle to search for.
 *
 *  \return Pointer to the device connection handle if one was found, NULL otherwise.
 */
BT_HCI_Connection_t* Bluetooth_HCI_FindConnection(BT_StackConfig_t* const StackState,
                                                  const uint8_t* const RemoteBDADDR,
                                                  const uint16_t Handle)
{
	for (uint8_t i = 0; i < MAX_DEVICE_CONNECTIONS; i++)
	{
		BT_HCI_Connection_t* Connection = &StackState->State.HCI.Connections[i];
		
		if (Connection->State == HCI_CONSTATE_Free)
		  continue;

		if (RemoteBDADDR != NULL)
		{
			if (memcmp(Connection->RemoteBDADDR, RemoteBDADDR, BT_BDADDR_LEN) == 0)
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

/** Initalizes the Bluetooth HCI layer, ready for new device connections.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_HCI_Init(BT_StackConfig_t* const StackState)
{
	StackState->State.HCI.State           = HCISTATE_Init_Reset;
	StackState->State.HCI.StateTransition = true;

	for (uint8_t i = 0; i < MAX_DEVICE_CONNECTIONS; i++)
	  StackState->State.HCI.Connections[i].State = HCI_CONSTATE_Free;
}

/** Processes a recieved Bluetooth HCI packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_HCI_ProcessPacket(BT_StackConfig_t* const StackState)
{
	BT_HCIEvent_Header_t*   HCIEventHeader = (BT_HCIEvent_Header_t*)StackState->Config.PacketBuffer;
	uint8_t                 NextHCIState   = StackState->State.HCI.State;

	if (HCIEventHeader->EventCode == EVENT_COMMAND_COMPLETE)
	{
		BT_HCIEvent_CommandComplete_t* CommandCompleteHeader = (BT_HCIEvent_CommandComplete_t*)&HCIEventHeader->Parameters;
		
		/* Determine the next state to change to according to the current state and which command completed */
		switch (StackState->State.HCI.State)
		{
			case HCISTATE_Init_Reset:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_RESET))
				  NextHCIState = HCISTATE_Init_ReadBufferSize;
				break;
			case HCISTATE_Init_ReadBufferSize:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE))
				  NextHCIState = HCISTATE_Init_GetBDADDR;
				break;
			case HCISTATE_Init_GetBDADDR:
				if (CommandCompleteHeader->Opcode == CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBDADDR))
				{
					memcpy(StackState->State.HCI.LocalBDADDR, &CommandCompleteHeader->Parameters[1], BT_BDADDR_LEN);				
					NextHCIState = HCISTATE_Init_SetLocalName;
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
	}
	else if (HCIEventHeader->EventCode == EVENT_CONNECTION_REQUEST)
	{
		BT_HCIEvent_ConnectionRequest_t* ConnectionRequestEventHeader = (BT_HCIEvent_ConnectionRequest_t*)&HCIEventHeader->Parameters;
				
		/* Must first copy the Remote BDADDR to a temporary buffer so we can overwrite it in the packet buffer */
		uint8_t RemoteBDADDR[BT_BDADDR_LEN];
		memcpy(RemoteBDADDR, &ConnectionRequestEventHeader->RemoteBDADDR, BT_BDADDR_LEN);

		/* Try to create a new connection handle for the connection request */
		BT_HCI_Connection_t* Connection      = Bluetooth_HCI_NewConnection(StackState, ConnectionRequestEventHeader->RemoteBDADDR, ConnectionRequestEventHeader->LinkType);		
		uint8_t              RejectionReason = ERROR_LIMITED_RESOURCES;
		
		/* If a connection handle was created, fire user-application callback to accept or reject the request - otherwise reject with a LIMITED RESOURCES error code */
		if (Connection)
		  RejectionReason = CALLBACK_Bluetooth_ConnectionRequest(StackState, Connection) ? 0 : ERROR_UNACCEPTABLE_BDADDR;
		
		BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.PacketBuffer;

		/* If a rejection reason was given, reject the request, otherwise send a connection request accept */
		if (RejectionReason)
		{
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_LINK_CONTROL | OCF_LINK_CONTROL_REJECT_CONNECTION_REQUEST),
			HCICommandHeader->ParameterLength = sizeof(BT_HCICommand_RejectConnectionReq_t);
			
			BT_HCICommand_RejectConnectionReq_t* HCIRejectConnectionHeader = (BT_HCICommand_RejectConnectionReq_t*)&HCICommandHeader->Parameters;
			memcpy(HCIRejectConnectionHeader->RemoteBDADDR, RemoteBDADDR, BT_BDADDR_LEN);
			HCIRejectConnectionHeader->Reason = RejectionReason;
			
			// Mark the connection as free again as it was rejected
			if (Connection)
			  Connection->State = HCI_CONSTATE_Free;
		}
		else
		{
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_LINK_CONTROL | OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST),
			HCICommandHeader->ParameterLength = sizeof(BT_HCICommand_AcceptConnectionReq_t);
			
			BT_HCICommand_AcceptConnectionReq_t* HCIAcceptConnectionHeader = (BT_HCICommand_AcceptConnectionReq_t*)&HCICommandHeader->Parameters;
			memcpy(HCIAcceptConnectionHeader->RemoteBDADDR, RemoteBDADDR, BT_BDADDR_LEN);
			HCIAcceptConnectionHeader->SlaveRole = true;		
		}

		CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + HCICommandHeader->ParameterLength));
	}
	else if (HCIEventHeader->EventCode == EVENT_CONNECTION_COMPLETE)
	{
		BT_HCIEvent_ConnectionComplete_t* ConnectionCompleteEventHeader = (BT_HCIEvent_ConnectionComplete_t*)&HCIEventHeader->Parameters;		

		/* Find the existing connection handle in the HCI device connection table if it exists, from the remote device BDADDR */
		BT_HCI_Connection_t* Connection = Bluetooth_HCI_FindConnection(StackState, ConnectionCompleteEventHeader->RemoteBDADDR, 0);
		
		/* If the connection handle exists, store the created connection handle and change the device connection state to connected */
		if (Connection)
		{
			Connection->Handle = le16_to_cpu(ConnectionCompleteEventHeader->Handle);
			Connection->State  = HCI_CONSTATE_Connected;
			
			/* Fire user application callback to signal the connection completion */
			EVENT_Bluetooth_ConnectionComplete(StackState, Connection);
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
			Connection->State = HCI_CONSTATE_Free;
			
			Bluetooth_ACL_NotifyHCIDisconnection(StackState, Connection->Handle);			
			EVENT_Bluetooth_DisconnectionComplete(StackState, Connection);
		}
	}
	else if (HCIEventHeader->EventCode == EVENT_PIN_CODE_REQUEST)
	{
		BT_HCIEvent_PinCodeReq_t* PINCodeRequestEventHeader = (BT_HCIEvent_PinCodeReq_t*)&HCIEventHeader->Parameters;		

		/* Must first copy the Remote BDADDR to a temporary buffer so we can overwrite it in the packet buffer */
		uint8_t RemoteBDADDR[BT_BDADDR_LEN];
		memcpy(RemoteBDADDR, &PINCodeRequestEventHeader->RemoteBDADDR, BT_BDADDR_LEN);

		BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.PacketBuffer;
		
		// Check if a PIN code has been set of one or more characters
		if ((StackState->Config.PINCode != NULL) && strlen(StackState->Config.PINCode))
		{
			BT_HCICommand_PinCodeACKResp_t* PINKeyACKResponse = (BT_HCICommand_PinCodeACKResp_t*)&HCIEventHeader->Parameters;
			uint8_t KeyLength = MIN(strlen(StackState->Config.PINCode), 16);

			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_LINK_CONTROL | OCF_LINK_CONTROL_PIN_CODE_REQUEST_REPLY);
			HCICommandHeader->ParameterLength = sizeof(BT_HCICommand_PinCodeACKResp_t);
			
			memcpy(&PINKeyACKResponse->RemoteBDADDR, RemoteBDADDR, BT_BDADDR_LEN);
			PINKeyACKResponse->PINCodeLength = KeyLength;
			memcpy(&PINKeyACKResponse->PINCode, StackState->Config.PINCode, KeyLength);
		}
		else
		{
			BT_HCICommand_PinCodeNAKResp_t* PINKeyNAKResponse = (BT_HCICommand_PinCodeNAKResp_t*)&HCIEventHeader->Parameters;

			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_LINK_CONTROL | OCF_LINK_CONTROL_PIN_CODE_REQUEST_NEG_REPLY);
			HCICommandHeader->ParameterLength = sizeof(BT_HCICommand_PinCodeNAKResp_t);
			
			memcpy(&PINKeyNAKResponse->RemoteBDADDR, RemoteBDADDR, BT_BDADDR_LEN);
		}

		CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + HCICommandHeader->ParameterLength));
	}
	else if (HCIEventHeader->EventCode == EVENT_LINK_KEY_REQUEST)
	{
		BT_HCIEvent_LinkKeyReq_t* LinkKeyRequestEventHeader = (BT_HCIEvent_LinkKeyReq_t*)&HCIEventHeader->Parameters;

		/* Must first copy the Remote BDADDR to a temporary buffer so we can overwrite it in the packet buffer */
		uint8_t RemoteBDADDR[BT_BDADDR_LEN];
		memcpy(RemoteBDADDR, &LinkKeyRequestEventHeader->RemoteBDADDR, BT_BDADDR_LEN);

		BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.PacketBuffer;
		HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_LINK_CONTROL | OCF_LINK_CONTROL_LINK_KEY_REQUEST_NEG_REPLY),
		HCICommandHeader->ParameterLength = sizeof(BT_HCICommand_LinkKeyNAKResp_t);
		
		BT_HCICommand_LinkKeyNAKResp_t* LinkKeyRequestNAKHeader = (BT_HCICommand_LinkKeyNAKResp_t*)&HCIEventHeader->Parameters;		
		memcpy(&LinkKeyRequestNAKHeader->RemoteBDADDR, RemoteBDADDR, BT_BDADDR_LEN);

		CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + HCICommandHeader->ParameterLength));
	}
	
	StackState->State.HCI.StateTransition = (StackState->State.HCI.State != NextHCIState);
	StackState->State.HCI.State           = NextHCIState;
}

/** Manages the existing HCI layer connections of a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *
 *  \return Boolean \c true if more HCI management tasks are pending, \c false otherwise.
 */
bool Bluetooth_HCI_Manage(BT_StackConfig_t* const StackState)
{
	BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.PacketBuffer;

	/* Only process HCI state transitions (one action per transition) */
	if (!(StackState->State.HCI.StateTransition))
	  return false;
	
	HCICommandHeader->OpCode = 0;
	
	switch (StackState->State.HCI.State)
	{
		case HCISTATE_Init_Reset:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_RESET);
			HCICommandHeader->ParameterLength = 0;
			break;
		case HCISTATE_Init_ReadBufferSize:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE),
			HCICommandHeader->ParameterLength = 0;
			break;
		case HCISTATE_Init_GetBDADDR:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_INFORMATIONAL | OCF_CTRLR_INFORMATIONAL_READBDADDR),
			HCICommandHeader->ParameterLength = 0;
			break;
		case HCISTATE_Init_SetLocalName:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME),
			HCICommandHeader->ParameterLength = 248;	
			strlcpy((void*)HCICommandHeader->Parameters, StackState->Config.Name, HCICommandHeader->ParameterLength);
			break;
		case HCISTATE_Init_SetDeviceClass:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE),
			HCICommandHeader->ParameterLength = 3;
			HCICommandHeader->Parameters[0]   = (StackState->Config.Class & 0xFF);
			HCICommandHeader->Parameters[1]   = (StackState->Config.Class >> 8);
			HCICommandHeader->Parameters[2]   = (StackState->Config.Class >> 16);
			break;
		case HCISTATE_Init_SetScanEnable:
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_CTRLR_BASEBAND | OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE),
			HCICommandHeader->ParameterLength = 1;
			HCICommandHeader->Parameters[0]   = (StackState->Config.Discoverable ? BT_SCANMODE_InquiryAndPageScans : BT_SCANMODE_NoScansEnabled);
			break;
	}
	
	if (HCICommandHeader->OpCode)
	  CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + HCICommandHeader->ParameterLength));
	
	StackState->State.HCI.StateTransition = false;
	return true;
}
