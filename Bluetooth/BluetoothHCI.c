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

#include "BluetoothHCI.h"

/** Creates a new Bluetooth device connection, if space is available in the given Bluetooth stack state HCI connection table.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *  \param[in]      BDADDR      Bluetooth device address of the remote device.
 *  \param[in]      LinkType    Type of Bluetooth link, a value from \ref BT_LinkTypes_t.
 *
 *  \return Pointer to the new connection handle if one was created, NULL otherwise.
 */
static BT_HCI_Connection_t* Bluetooth_HCI_NewConnection(Bluetooth_Device_t* const StackState, const uint8_t* const BDADDR, const uint8_t LinkType)
{
	for (uint8_t i = 0; i < MAX_DEVICE_CONNECTIONS; i++)
	{
		BT_HCI_Connection_t* ConnectionHandle = &StackState->State.HCI.Connections[i];

		if (ConnectionHandle->State == HCI_CONSTATE_Free)
		{
			memcpy(ConnectionHandle->BDADDR, BDADDR, BT_BDADDR_LEN);
			ConnectionHandle->State    = HCI_CONSTATE_New;
			ConnectionHandle->LinkType = LinkType;
			return ConnectionHandle;
		}
	}
	
	return NULL;
}

/** Finds the corresponding Bluetooth device connection from the remote device's address or link handle.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *  \param[in]      BDADDR      Bluetooth device address of the remote device to search for, \c NULL to search by link handle.
 *  \param[in]      Handle      If \c BDADDR is \c NULL, the link handle to search for.
 *
 *  \return Pointer to the device connection handle if one was found, NULL otherwise.
 */
static BT_HCI_Connection_t* Bluetooth_HCI_FindConnection(Bluetooth_Device_t* const StackState, const uint8_t* const BDADDR, const uint16_t Handle)
{
	for (uint8_t i = 0; i < MAX_DEVICE_CONNECTIONS; i++)
	{
		BT_HCI_Connection_t* ConnectionHandle = &StackState->State.HCI.Connections[i];
		
		if (ConnectionHandle->State == HCI_CONSTATE_Free)
		  continue;

		if (BDADDR != NULL)
		{
			if (memcmp(ConnectionHandle->BDADDR, BDADDR, BT_BDADDR_LEN) == 0)
			  return ConnectionHandle;
		}
		else
		{
			if (ConnectionHandle->Handle == Handle)
			  return ConnectionHandle;		
		}
	}
	
	return NULL;
}

/** Closes an existing Bluetooth device connection, if one exists in the given Bluetooth stack state HCI connection table.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *  \param[in]      Handle      Link handle of the connection to close.
 *
 *  \return Pointer to the new connection handle if one was created, NULL otherwise.
 */
static BT_HCI_Connection_t* Bluetooth_HCI_CloseConnection(Bluetooth_Device_t* const StackState, const uint16_t Handle)
{
	BT_HCI_Connection_t* ConnectionHandle = Bluetooth_HCI_FindConnection(StackState, NULL, Handle);
	
	if (!(ConnectionHandle))
	  return NULL;
	
	ConnectionHandle->State = HCI_CONSTATE_Free;
	return ConnectionHandle;
}

/** Initalizes the Bluetooth HCI layer, ready for new device connections.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_HCI_Init(Bluetooth_Device_t* const StackState)
{
	StackState->State.HCI.State           = HCISTATE_Init_Reset;
	StackState->State.HCI.StateTransition = true;

	for (uint8_t i = 0; i < MAX_DEVICE_CONNECTIONS; i++)
	  StackState->State.HCI.Connections[i].State = HCI_CONSTATE_Free;
}

/** Processes a recieved Bluetooth HCI packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *  \param[in]      Data        Pointer to the start of the HCI data.
 */
void Bluetooth_HCI_ProcessPacket(Bluetooth_Device_t* const StackState, uint8_t* Data)
{
	BT_HCIEvent_Header_t*   HCIEventHeader = (BT_HCIEvent_Header_t*)Data;
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
					memcpy(StackState->State.HCI.LocalBDADDR, &CommandCompleteHeader->Parameters[1], sizeof(StackState->State.HCI.LocalBDADDR));				
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
		
		/* Must copy the Remote BDADDR to a temporary buffer, as the output packet buffer could potentially overlap the input buffer */
		uint8_t RemoteBDADDR[BT_BDADDR_LEN];
		memcpy(RemoteBDADDR, &ConnectionRequestEventHeader->RemoteBDADDR, sizeof(RemoteBDADDR));
		
		/* Try to create a new connection handle for the connection request */
		BT_HCI_Connection_t* ConnectionHandle = Bluetooth_HCI_NewConnection(StackState, ConnectionRequestEventHeader->RemoteBDADDR, ConnectionRequestEventHeader->LinkType);		
		uint8_t           RejectionReason  = ERROR_LIMITED_RESOURCES;
		
		/* If a connection handle was created, fire user-application callback to accept or reject the request - otherwise reject with a LIMITED RESOURCES error code */
		if (ConnectionHandle)
		  RejectionReason = CALLBACK_Bluetooth_ConnectionRequest(StackState, ConnectionHandle) ? 0 : ERROR_UNACCEPTABLE_BDADDR;
		
		BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.OutputPacketBuffer;

		/* If a rejection reason was given, reject the request, otherwise send a connection request accept */
		if (RejectionReason)
		{
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_LINK_CONTROL | OCF_LINK_CONTROL_REJECT_CONNECTION_REQUEST),
			HCICommandHeader->ParameterLength = sizeof(BT_HCICommand_RejectConnectionReq_t);
			
			BT_HCICommand_RejectConnectionReq_t* HCIRejectConnectionHeader = (BT_HCICommand_RejectConnectionReq_t*)&HCICommandHeader->Parameters;
			memcpy(HCIRejectConnectionHeader->RemoteBDADDR, RemoteBDADDR, sizeof(RemoteBDADDR));
			HCIRejectConnectionHeader->Reason = RejectionReason;
		}
		else
		{
			HCICommandHeader->OpCode          = CPU_TO_LE16(OGF_LINK_CONTROL | OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST),
			HCICommandHeader->ParameterLength = sizeof(BT_HCICommand_AcceptConnectionReq_t);
			
			BT_HCICommand_AcceptConnectionReq_t* HCIAcceptConnectionHeader = (BT_HCICommand_AcceptConnectionReq_t*)&HCICommandHeader->Parameters;
			memcpy(HCIAcceptConnectionHeader->RemoteBDADDR, RemoteBDADDR, sizeof(RemoteBDADDR));
			HCIAcceptConnectionHeader->SlaveRole = true;		
		}

		CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + HCICommandHeader->ParameterLength),
		                              StackState->Config.OutputPacketBuffer);
	}
	else if (HCIEventHeader->EventCode == EVENT_CONNECTION_COMPLETE)
	{
		BT_HCIEvent_ConnectionComplete_t* ConnectionCompleteEventHeader = (BT_HCIEvent_ConnectionComplete_t*)&HCIEventHeader->Parameters;		

		/* Find the existing connection handle in the HCI device connection table if it exists, from the remote device BDADDR */
		BT_HCI_Connection_t* ConnectionHandle = Bluetooth_HCI_FindConnection(StackState, ConnectionCompleteEventHeader->RemoteBDADDR, 0);
		
		/* If the connection handle exists, store the created connection handle and change the device connection state to connected */
		if (ConnectionHandle)
		{
			ConnectionHandle->Handle = ConnectionCompleteEventHeader->Handle;
			ConnectionHandle->State  = HCI_CONSTATE_Connected;
			
			/* Fire user application callback to signal the connection completion */
			EVENT_Bluetooth_ConnectionComplete(StackState, ConnectionHandle);
		}
	}
	else if (HCIEventHeader->EventCode == EVENT_DISCONNECTION_COMPLETE)
	{
		BT_HCIEvent_DisconnectionComplete_t* DisconnectionCompleteEventHeader = (BT_HCIEvent_DisconnectionComplete_t*)&HCIEventHeader->Parameters;

		/* Find and close the existing connection handle in the HCI device connection table if it exists, from the remote device BDADDR */
		BT_HCI_Connection_t* ConnectionHandle = Bluetooth_HCI_CloseConnection(StackState, DisconnectionCompleteEventHeader->Handle);

		/* If an existing connection was found, fire user application callback to signal the disconnection completion */
		if (ConnectionHandle)
		  EVENT_Bluetooth_DisconnectionComplete(StackState, ConnectionHandle);
	}
	#if 0
	else if ((HCIEventHeader->EventCode != 0x1B) && (HCIEventHeader->EventCode != 0x0F)) // TODO: Remove
	{
		char Buffer[17];
		
		sprintf(Buffer, "UNK EVENT: %02X", HCIEventHeader->EventCode);
	
		LCD_Clear();
		LCD_WriteString(Buffer);
		Delay_MS(200);
		Delay_MS(200);
		Delay_MS(200);
	}
	#endif
	
	StackState->State.HCI.StateTransition = (StackState->State.HCI.State != NextHCIState);
	StackState->State.HCI.State           = NextHCIState;
}

/** Manages the existing HCI layer connections of a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *
 *  \return Boolean \c true if more HCI management tasks are pending, \c false otherwise.
 */
bool Bluetooth_HCI_Manage(Bluetooth_Device_t* const StackState)
{
	BT_HCICommand_Header_t* HCICommandHeader = (BT_HCICommand_Header_t*)StackState->Config.OutputPacketBuffer;

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
	{
		CALLBACK_Bluetooth_SendPacket(StackState, BLUETOOTH_PACKET_HCICommand, (sizeof(BT_HCICommand_Header_t) + HCICommandHeader->ParameterLength),
		                              StackState->Config.OutputPacketBuffer);
	}
	
	StackState->State.HCI.StateTransition = false;
	return true;
}
