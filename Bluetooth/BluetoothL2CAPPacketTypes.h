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
 *  Bluetooth L2CAP layer packet definitions.
 */

#ifndef _BLUETOOTH_L2CAPPACKET_TYPES_H_
#define _BLUETOOTH_L2CAPPACKET_TYPES_H_

	/* Includes: */
		#include "BluetoothCommon.h"

	/* Type Defines: */	
		/** Bluetooth L2CAP data packet header structure, for L2CAP packets containing L2CAP data. */
		typedef struct
		{
			uint16_t PayloadLength; /**< Size of the data payload, in bytes. */
			uint16_t DestinationChannel; /**< Destination channel in the device the data is directed to. */
			uint8_t  Payload[]; /**< Trailing packet data. */
		} ATTR_PACKED BT_DataPacket_Header_t;

		/** Bluetooth signaling command header structure, for all L2CAP packets containing a signaling command. */
		typedef struct
		{
			uint8_t  Code; /**< Signal code, a \c BT_SIGNAL_* mask value. */
			uint8_t  Identifier; /**< Unique signal command identifier to link requests and responses. */
			uint16_t Length; /**< Length of the signaling command data, in bytes. */
			uint8_t  Data[]; /**< Trailing packet data. */
		} ATTR_PACKED BT_Signal_Header_t;

		/** Connection Request signaling command structure, for channel connection requests. */
		typedef struct
		{
			uint16_t PSM; /**< Type of data the channel will carry, a value from the \ref BT_ChannelPSM_t enum. */
			uint16_t SourceChannel; /**< Channel source on the sending device this channel will link to. */
		} ATTR_PACKED BT_Signal_ConnectionReq_t;

		/** Connection response signaling command structure, for responses to channel connection requests. */
		typedef struct
		{
			uint16_t DestinationChannel; /**< Destination device channel that the connection request was processed on. */
			uint16_t SourceChannel; /**< Source device channel address that the connection request came from. */
			uint16_t Result; /**< Connection result, a \c BT_CONNECTION_* mask value. */
			uint16_t Status; /**< Status of the request if the result was set to the pending value. */
		} ATTR_PACKED BT_Signal_ConnectionResp_t;

		/** Disconnection request signaling command structure, for channel disconnection requests. */
		typedef struct
		{
			uint16_t DestinationChannel; /**< Destination channel address which is to be disconnected. */
			uint16_t SourceChannel; /**< Source channel address which is to be disconnected. */
		} ATTR_PACKED BT_Signal_DisconnectionReq_t;

		/** Disconnection response signaling command structure, for responses to channel disconnection requests. */
		typedef struct
		{
			uint16_t DestinationChannel; /**< Destination channel address which was disconnected. */
			uint16_t SourceChannel; /**< Source channel address which was disconnected. */
		} ATTR_PACKED BT_Signal_DisconnectionResp_t;

		/** Configuration Request signaling command structure, for channel configuration requests. */
		typedef struct
		{
			uint16_t DestinationChannel; /**< Destination channel address which is to be configured. */
			uint16_t Flags; /**< Configuration flags for the request, including command continuation. */
			uint8_t  Options[]; /**< Option values for the channel configuration. */
		} ATTR_PACKED BT_Signal_ConfigurationReq_t;

		/** Configuration Response signaling command structure, for responses to channel configuration requests. */
		typedef struct
		{
			uint16_t SourceChannel; /**< Source channel that the configuration request was directed at. */
			uint16_t Flags; /**< Configuration flags for the response, including response continuation. */
			uint16_t Result; /**< Configuration result, a \c BT_CONFIGURATION_* mask value. */
		} ATTR_PACKED BT_Signal_ConfigurationResp_t;

		/** Information Request signaling command structure, for device information requests. */
		typedef struct
		{
			uint16_t InfoType; /**< Data type that is being requested, a \c BT_INFOREQ_* mask value. */
		} ATTR_PACKED BT_Signal_InformationReq_t;

		/** Information Response signaling command structure, for responses to information requests. */
		typedef struct
		{
			uint16_t InfoType; /**< Data type that was requested, a \c BT_INFOREQ_* mask value. */
			uint16_t Result; /**< Result of the request, a \c BT_INFORMATION_* mask value */
		} ATTR_PACKED BT_Signal_InformationResp_t;

		/** Configuration Option header structure, placed at the start of each option in a Channel Configuration
		 *  request's options list.
		 */
		typedef struct
		{
			uint8_t Type; /**< Option type, a \c BT_CONFIG_OPTION_* mask value. */
			uint8_t Length; /**< Length of the option's value, in bytes. */
			uint8_t Data[]; /**< Trailing option data. */
		} ATTR_PACKED BT_Config_Option_Header_t;
		
#endif
