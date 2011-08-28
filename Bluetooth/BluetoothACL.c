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
	
	char LCDBuff[2][16];
	
	sprintf(LCDBuff[0], "H:%04X D:%04X", (ACLPacketHeader->ConnectionHandle & 0x0FFF), ACLPacketHeader->DataLength);
	sprintf(LCDBuff[1], "C:%04X P:%04X", ACLDataHeader->DestinationChannel, ACLDataHeader->PayloadLength);
	
	LCD_Clear();
	LCD_WriteString(LCDBuff[0]);
	LCD_SetCursor(2, 0);
	LCD_WriteString(LCDBuff[1]);
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
