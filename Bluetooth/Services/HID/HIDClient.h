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
		#include "HIDClient.h"
	
	/* Macros: */
		#define MAX_HID_CONNECTIONS     MIN(MAX_DEVICE_CONNECTIONS, 1)

		#define HID_TRANSTYPE_MASK         0xF0

	/* Enums: */
		enum HID_TransactionTypes_t
		{
			HID_TRANS_HANDSHAKE            = 0x00,
			HID_TRANS_CONTROL              = 0x10,
			HID_TRANS_GET_REPORT           = 0x40,
			HID_TRANS_SET_REPORT           = 0x50,
			HID_TRANS_GET_PROTOCOL         = 0x60,
			HID_TRANS_SET_PROTOCOL         = 0x70,
			HID_TRANS_GET_IDLE             = 0x80,
			HID_TRANS_SET_IDLE             = 0x90,
			HID_TRANS_DATA                 = 0xA0,
			HID_TRANS_DATAC                = 0xB0,
		};

		enum HID_HandshakeTypes_t
		{
			HID_HS_SUCCESSFUL              = 0x00,
			HID_HS_NOT_READY               = 0x01,
			HID_HS_ERR_INVALID_REPORT_ID   = 0x02,
			HID_HS_ERR_UNSUPPORTED_REQUEST = 0x03,
			HID_HS_ERR_INVALID_PARAMETER   = 0x04,
			HID_HS_ERR_UNKNOWN             = 0x0E,
			HID_HS_ERR_FATAL               = 0x0F,
		};

		enum HID_ControlTypes_t
		{
			HID_CTL_NOP                    = 0x00,
			HID_CTL_HARD_RESET             = 0x01,
			HID_CTL_SOFT_RESET             = 0x02,
			HID_CTL_SUSPEND                = 0x03,
			HID_CTL_EXIT_SUSPEND           = 0x04,
			HID_CTL_VIRTUAL_CABLE_UNPLUG   = 0x05,
		};
		
		enum HID_DataTypes_t
		{
			HID_DATAT_Other                = 0x00,
			HID_DATAT_Input                = 0x01,
			HID_DATAT_Output               = 0x02,
			HID_DATAT_Feature              = 0x03,
		};

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
