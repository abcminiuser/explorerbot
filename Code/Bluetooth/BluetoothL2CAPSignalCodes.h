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
 *  Bluetooth L2CAP signalling command definitions.
 */

#ifndef _BLUETOOTH_L2CAPPACKET_SIGNALCODES_H_
#define _BLUETOOTH_L2CAPPACKET_SIGNALCODES_H_

	/* Includes: */
		#include "BluetoothCommon.h"

	/* Macros: */
		/** \name Bluetooth L2CAP Signalling Channel commands. */
		//@{
		#define BT_SIGNAL_COMMAND_REJECT          0x01
		#define BT_SIGNAL_CONNECTION_REQUEST      0x02
		#define BT_SIGNAL_CONNECTION_RESPONSE     0x03
		#define BT_SIGNAL_CONFIGURATION_REQUEST   0x04
		#define BT_SIGNAL_CONFIGURATION_RESPONSE  0x05
		#define BT_SIGNAL_DISCONNECTION_REQUEST   0x06
		#define BT_SIGNAL_DISCONNECTION_RESPONSE  0x07
		#define BT_SIGNAL_ECHO_REQUEST            0x08
		#define BT_SIGNAL_ECHO_RESPONSE           0x09
		#define BT_SIGNAL_INFORMATION_REQUEST     0x0A
		#define BT_SIGNAL_INFORMATION_RESPONSE    0x0B
		//@}

		/** \name Bluetooth L2CAP Information Request item parameter indexes. */
		//@{
		#define BT_INFOREQ_MTU                    0x0001
		#define BT_INFOREQ_EXTENDEDFEATURES       0x0002
		//@}

		/** \name Bluetooth L2CAP Information Request error codes. */
		//@{
		#define BT_INFORMATION_SUCCESSFUL         0x0000
		#define BT_INFORMATION_NOTSUPPORTED       0x0001
		//@}

		/** \name Bluetooth L2CAP Connection error codes. */
		//@{
		#define BT_CONNECTION_SUCCESSFUL          0x0000
		#define BT_CONNECTION_PENDING             0x0001
		#define BT_CONNECTION_REFUSED_PSM         0x0002
		#define BT_CONNECTION_REFUSED_SECURITY    0x0003
		#define BT_CONNECTION_REFUSED_RESOURCES   0x0004
		//@}

		/** \name Bluetooth L2CAP Channel Configuration error codes. */
		//@{
		#define BT_CONFIGURATION_SUCCESSFUL       0x0000
		#define BT_CONFIGURATION_REJECTED_PARAMS  0x0002
		#define BT_CONFIGURATION_REJECTED         0x0002
		#define BT_CONFIGURATION_UNKNOWNOPTIONS   0x0003
		//@}

		/** \name Bluetooth L2CAP Command Rejection reason codes. */
		//@{
		#define BT_CMDREJ_COMMAND_UNKNOWN         0x0000
		#define BT_CMDREJ_SIGNAL_MTU_EXCEEDED     0x0001
		#define BT_CMDREJ_INVALID_CHANNEL_ID      0x0002
		//@}

		/** \name Bluetooth L2CAP Channel Configuration option parameter indexes. */
		//@{
		#define BT_CONFIG_OPTION_MTU              0x01
		//@}

#endif
