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
 *  Bluetooth HCI layer packet definitions.
 */

#ifndef _BLUETOOTH_HCIPACKET_TYPES_H_
#define _BLUETOOTH_HCIPACKET_TYPES_H_

	/* Includes: */
		#include "BluetoothCommon.h"

	/* Enums: */
		/** Enum for the possible Bluetooth Scan modes that can be set via the Bluetooth adapter. */
		enum BT_ScanEnable_Modes_t
		{
			BT_SCANMODE_NoScansEnabled       = 0, /**< Disable all scan responses. */
			BT_SCANMODE_InquiryScanOnly      = 1, /**< Respond to Inquiry scans only. */
			BT_SCANMODE_PageScanOnly         = 2, /**< Respond to Page Scan scans only. */
			BT_SCANMODE_InquiryAndPageScans  = 3, /**< Respond to both Inquiry and Page Scan scans. */
		};

	/* Type Defines: */
		/** Bluetooth HCI Command packet header structure, common to all HCI Command packets. */
		typedef struct
		{
			uint16_t OpCode; /**< Operation code for the HCI command, a combination of OGF and OCF masks. */
			uint8_t  ParameterLength; /**< Length of the command parameters following the header. */
			uint8_t  Parameters[]; /**< Parameters for the HCI command. */
		} ATTR_PACKED BT_HCICommand_Header_t;

		/** Bluetooth HCI Event packet header structure, common to all HCI Event packets. */
		typedef struct
		{
			uint8_t  EventCode; /**< Event code for the received HCI event. */
			uint8_t  ParameterLength; /**< Length of the event parameters following the header. */
			uint8_t  Parameters[]; /**< Parameters for the HCI event. */
		} ATTR_PACKED BT_HCIEvent_Header_t;

		/** Bluetooth HCI Data packet header structure, common to all HCI Data packets. */
		typedef struct
		{
			uint16_t Handle; /**< HCI connection handle for the established HCI connection. */
			uint16_t DataLength; /**< Length of the HCI packet data following the header. */
			uint8_t  Data[]; /**< Data payload of the HCI data packet. */
		} ATTR_PACKED BT_HCIData_Header_t;
		
		/** HCI Command Status Event structure. */
		typedef struct
		{
			uint8_t  Status; /**< Status of the command being processed. */
			uint8_t  Packets; /**< Number of HCI packets which may be sent to the controller from the host. */
			uint16_t OpCode; /**< OpCode of the operation being processed. */
		} ATTR_PACKED BT_HCIEvent_CommandStatus_t;
		
		/** HCI Command Complete Event structure. */
		typedef struct
		{
			uint8_t  Packets; /**< Number of HCI packets which may be sent to the controller from the host. */
			uint16_t Opcode; /**< OpCode of the operation that has completed. */
			uint8_t  Parameters[]; /**< Optional parameters associated with the completed command. */
		} ATTR_PACKED BT_HCIEvent_CommandComplete_t;

		/** HCI Number of Packets Complete Event structure. */
		typedef struct
		{
			uint8_t  Handles; /**< Number of connection handles reported in the event. */
			struct
			{
				uint16_t Handle; /**< HCI connection handle being reported. */
				uint16_t PacketsCompleted; /**< Number of HCI data packets which have been completed  by the controller. */
			} PacketInfo[]; /**< List of HCI connection handles and completed HCI data packets. */
		} ATTR_PACKED BT_HCIEvent_NumPacketsComplete_t;
		
		/** HCI Connection Request Event structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
			uint8_t  ServiceClass; /**< Service class of the remote device, a \c DEVICE_CLASS_SERVICE_* value. */
			uint16_t MajorMinorClass; /**< Major and Minor classes of the remote device, one or more \c DEVICE_CLASS_* masks. */
			uint8_t  LinkType; /**< Link type to be established, a value from \ref BT_LinkTypes_t. */
		} ATTR_PACKED BT_HCIEvent_ConnectionRequest_t;

		/** HCI Connection Complete Command structure. */
		typedef struct
		{
			uint8_t  Status; /**< Status of the HCI connection. */
			uint16_t Handle; /**< HCI connection handle for the completed HCI connection. */
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
			uint8_t  LinkType; /**< Link type for the HCI connection, a value from \ref BT_LinkTypes_t. */
			uint8_t  EncryptionEnabled; /**< Indicates if encryption is enabled for the established connection. */
		} ATTR_PACKED BT_HCIEvent_ConnectionComplete_t;

		/** HCI Disconnection Complete Command structure. */
		typedef struct
		{
			uint8_t  Status; /**< Status of the HCI connection. */
			uint16_t Handle; /**< HCI connection handle for the disconnected HCI connection. */
			uint8_t  Reason; /**< Reason for the disconnection. */
		} ATTR_PACKED BT_HCIEvent_DisconnectionComplete_t;

		/** HCI Link Key Request Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
		} ATTR_PACKED BT_HCIEvent_LinkKeyReq_t;

		/** HCI Link Key Negative Acknowledge Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
		} ATTR_PACKED BT_HCICommand_LinkKeyNAKResp_t;

		/** HCI PIN Code Request Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
		} ATTR_PACKED BT_HCIEvent_PinCodeReq_t;

		/** HCI PIN Code Acknowledge Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
			uint8_t  PINCodeLength; /**< Length of the device's authentication PIN code, in octets. */
			char     PINCode[16]; /**< PIN authentication code for the local device. */
		} ATTR_PACKED BT_HCICommand_PinCodeACKResp_t;

		/** HCI PIN Code Negative Acknowledge Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
		} ATTR_PACKED BT_HCICommand_PinCodeNAKResp_t;

		/** HCI Connection Accept Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
			uint8_t  SlaveRole; /**< Role for the local device in the connection establishment. */
		} ATTR_PACKED BT_HCICommand_AcceptConnectionReq_t;

		/** HCI Connection Rejection Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
			uint8_t  Reason; /**< Connection rejection reason value, a \c HCI_ERROR_* value. */
		} ATTR_PACKED BT_HCICommand_RejectConnectionReq_t;

		/** HCI Create Connection Command structure. */
		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN]; /**< Bluetooth BDADDR of the remote device. */
			uint16_t PacketType; /**< Bluetooth low level packet type to use for the connection (see Bluetooth specification). */
			uint8_t  PageScanMode; /**< Page scan repition mode. */
			uint8_t  Reserved; /**< Must be set to zero to maintain compatibility. */
			uint16_t ClockOffset; /**< Clock offset between the local and remote device, if known. */
			uint8_t  AllowRoleSwitch; /**< Indicates if the remote device may request a role switch in the connection. */
		} ATTR_PACKED BT_HCICommand_CreateConnection_t;
		
#endif
