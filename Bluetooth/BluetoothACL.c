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
void Bluetooth_ACL_Init(Bluetooth_Device_t* const StackState)
{
	
}

/** Processes a recieved Bluetooth ACL packet from a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *  \param[in]      Data        Pointer to the start of the ACL data.
 */
void Bluetooth_ACL_ProcessPacket(Bluetooth_Device_t* const StackState, uint8_t* Data)
{
	
}

/** Manages the existing ACL layer connections of a Bluetooth adapter.
 *
 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
 *
 *  \return Boolean \c true if more ACL management tasks are pending, \c false otherwise.
 */
bool Bluetooth_ACL_Manage(Bluetooth_Device_t* const StackState)
{
	return false;
}
