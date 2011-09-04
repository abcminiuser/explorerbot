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

#ifndef __BLUETOOTH_L2CAPPACKET_SIGNALCODES_H_
#define __BLUETOOTH_L2CAPPACKET_SIGNALCODES_H_

	/* Includes: */
		#include "BluetoothCommon.h"
		
	/* Macros: */
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

		#define BT_INFOREQ_MTU                    0x0001
		#define BT_INFOREQ_EXTENDEDFEATURES       0x0002

		#define BT_INFORMATION_SUCCESSFUL         0x0000
		#define BT_INFORMATION_NOTSUPPORTED       0x0001

		#define BT_CONNECTION_SUCCESSFUL          0x0000
		#define BT_CONNECTION_REFUSED_PSM         0x0002
		#define BT_CONNECTION_REFUSED_RESOURCES   0x0004

		#define BT_CONFIGURATION_SUCCESSFUL       0x0000
		#define BT_CONFIGURATION_REJECTED         0x0002
		#define BT_CONFIGURATION_UNKNOWNOPTIONS   0x0003

		#define BT_CONFIG_OPTION_MTU              1
		
#endif
