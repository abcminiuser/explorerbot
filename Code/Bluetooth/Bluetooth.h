/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.com
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for Bluetooth.c.
 */

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>

		#include "BluetoothCommon.h"
		#include "BluetoothHCI.h"
		#include "BluetoothL2CAP.h"

	/* Function Prototypes: */
		void Bluetooth_Init(BT_StackConfig_t* const StackState);
		bool Bluetooth_ManageConnections(BT_StackConfig_t* const StackState);
		void Bluetooth_ProcessPacket(BT_StackConfig_t* const StackState,
		                             const uint8_t Type);
		void Bluetooth_TickElapsed(BT_StackConfig_t* const StackState);

#endif
