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

#ifndef _RFCOMM_SERVICE_H_
#define _RFCOMM_SERVICE_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>

		#include "../../Bluetooth.h"
		#include "../SDP/SDP.h"
		#include "RFCOMMServiceTable.h"

	/* Macros: */
		#define FRAME_POLL_FINAL               (1 << 4)

		#define RFCOMM_CONTROL_DLCI            0
		#define RFCOMM_MAX_OPEN_CHANNELS       5	
		
		#define RFCOMM_CONFIG_REMOTESIGNALS    (1 << 0)
		#define RFCOMM_CONFIG_LOCALSIGNALS     (1 << 1)
		#define RFCOMM_CONFIG_LOCALSIGNALSSENT (1 << 2)
		#define RFCOMM_CONFIG_ABMMODESET       (1 << 3)

	/* Enums: */
		/** Enum for the types of RFCOMM frames which can be exchanged on a Bluetooth channel. */
		enum RFCOMM_Frame_Types_t
		{
			RFCOMM_Frame_DM            = 0x0F, /**< Disconnected Mode Field */
			RFCOMM_Frame_DISC          = 0x43, /**< Disconnect Field */
			RFCOMM_Frame_SABM          = 0x2F, /**< Set Asynchronous Balance Mode Field */
			RFCOMM_Frame_UA            = 0x63, /**< Unnumbered Acknowledgement Field */
			RFCOMM_Frame_UIH           = 0xEF, /**< Unnumbered Information with Header check Field */
		};

		enum RFCOMM_Channel_States_t
		{
			RFCOMM_Channel_Closed      = 0,
			RFCOMM_Channel_Configure   = 1,
			RFCOMM_Channel_Open        = 2,
		};

	/* Type Defines: */
		typedef struct
		{
			unsigned EA   : 1;
			unsigned CR   : 1;
			unsigned DLCI : 6;
		} RFCOMM_Address_t;

		typedef struct
		{
			RFCOMM_Address_t Address;
			uint8_t          Control;
		} RFCOMM_Header_t;

		typedef struct
		{
			unsigned EA      : 1;
			unsigned CR      : 1;
			unsigned Command : 6;
		} RFCOMM_Command_t;

		typedef struct
		{
			BT_L2CAP_Channel_t* ACLChannel;
		
			uint8_t  DLCI;
			uint8_t  State;
			uint8_t  Priority;
			uint16_t MTU;
			uint8_t  ConfigFlags;
			struct
			{
				uint8_t Signals;
				uint8_t BreakSignal;
			} Remote;
			struct
			{
				uint8_t Signals;
				uint8_t BreakSignal;
			} Local;
		} RFCOMM_Channel_t;

	/* Function Prototypes: */
		void RFCOMM_Init(BT_StackConfig_t* const StackState);
		void RFCOMM_Manage(BT_StackConfig_t* const StackState);
		void RFCOMM_ChannelOpened(BT_StackConfig_t* const StackState,
		                          BT_L2CAP_Channel_t* const Channel);
		void RFCOMM_ChannelClosed(BT_StackConfig_t* const StackState,
		                          BT_L2CAP_Channel_t* const Channel);
		void RFCOMM_ProcessPacket(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data);

#endif
