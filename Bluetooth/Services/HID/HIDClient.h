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

#ifndef _HID_CLIENT_SERVICE_H_
#define _HID_CLIENT_SERVICE_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>

		#include "../../Bluetooth.h"
		#include "HIDService.h"
	
	/* Macros: */
		#define MAX_HID_CONNECTIONS     MIN(MAX_DEVICE_CONNECTIONS, 1)

	/* Enums: */
		enum HID_Service_States_t
		{
			HID_SSTATE_Free             = 0,
			HID_SSTATE_New              = 1,
			HID_SSTATE_ReportRequested  = 2,
			HID_SSTATE_Connected        = 3,
		};

	/* Type Defines: */
		typedef struct
		{
			BT_StackConfig_t*   Stack;
			BT_L2CAP_Channel_t* ControlChannel;
			BT_L2CAP_Channel_t* InterruptChannel;
			uint8_t State;
		} HID_Service_t;

	/* Function Prototypes: */
		void HID_Client_Init(BT_StackConfig_t* const StackState);
		void HID_Client_Manage(BT_StackConfig_t* const StackState);
		void HID_Client_ChannelOpened(BT_StackConfig_t* const StackState,
		                              BT_L2CAP_Channel_t* const Channel);
		void HID_Client_ChannelClosed(BT_StackConfig_t* const StackState,
		                              BT_L2CAP_Channel_t* const Channel);
		void HID_Client_ProcessPacket(BT_StackConfig_t* const StackState,
                                      BT_L2CAP_Channel_t* const Channel,
                                      uint16_t Length,
                                      uint8_t* Data);

		void CALLBACK_HID_Client_ReportReceived(BT_StackConfig_t* const StackState,
                                                BT_L2CAP_Channel_t* const Channel,
		                                        uint8_t ReportType,
		                                        uint16_t Length,
		                                        uint8_t* Data);

#endif
