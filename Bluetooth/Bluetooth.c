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
 *  Main module for the Bluetooth stack. This module contains the main Bluetooth stack management functions.
 */

#include "Bluetooth.h"

/** Initializes the Bluetooth stack ready for use. This must be called once at application startup to reset the internal
 *  stack state.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 */
void Bluetooth_Init(BT_StackConfig_t* const StackState)
{
	memset(&StackState->State, 0x00, sizeof(StackState->State));
	
	Bluetooth_HCI_Init(StackState);
	Bluetooth_L2CAP_Init(StackState);
	
	EVENT_Bluetooth_InitServices(StackState);
}

/** Manages the Bluetooth connections between the local and remote Bluetooth devices. This must be called periodically to
 *  maintain connections.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *
 *  \return Boolean \c true if more management is required, \c false if all pending management tasks have completed.
 */
bool Bluetooth_ManageConnections(BT_StackConfig_t* const StackState)
{
	if (Bluetooth_HCI_Manage(StackState) || Bluetooth_L2CAP_Manage(StackState))
	  return true;
	  
	EVENT_Bluetooth_ManageServices(StackState);
	return false;
}

/** Processes the Bluetooth packet of the specified type currently stored in the input packet buffer of the given stack instance.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *  \param[in]      Type        Packet type, a value from the \ref BT_PacketType_t enum.
 */
void Bluetooth_ProcessPacket(BT_StackConfig_t* const StackState,
                             const uint8_t Type)
{
	switch (Type)
	{
		case BLUETOOTH_PACKET_HCIEvent:
			Bluetooth_HCI_ProcessEventPacket(StackState);
			return;
		case BLUETOOTH_PACKET_Data:
			Bluetooth_HCI_ProcessDataPacket(StackState);
			return;
	}
}
