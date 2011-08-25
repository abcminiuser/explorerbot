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

#ifndef __BLUETOOTH_HCIPACKET_TYPES_H_
#define __BLUETOOTH_HCIPACKET_TYPES_H_

	/* Includes: */
		#include "BluetoothCommon.h"

	/* Enums: */
		enum BT_ScanEnable_Modes_t
		{
			BT_SCANMODE_NoScansEnabled       = 0,
			BT_SCANMODE_InquiryScanOnly      = 1,
			BT_SCANMODE_PageScanOnly         = 2,
			BT_SCANMODE_InquiryAndPageScans  = 3,
		};
		
		enum BT_LinkTypes_t
		{
			BT_LINKTYPE_SCO                  = 0,
			BT_LINKTYPE_ACL                  = 1,
			BT_LINKTYPE_eSCO                 = 2,
		};

	/* Type Defines: */
		typedef struct
		{
			uint16_t OpCode;
			uint8_t  ParameterLength;
			uint8_t  Parameters[];
		} ATTR_PACKED BT_HCICommand_Header_t;

		typedef struct
		{
			uint8_t  EventCode;
			uint8_t  ParameterLength;
			uint8_t  Parameters[];
		} ATTR_PACKED BT_HCIEvent_Header_t;

		typedef struct
		{
			uint8_t  Status;
			uint8_t  Packets;
			uint16_t OpCode;
		} ATTR_PACKED BT_HCIEvent_CommandStatus_t;

		typedef struct
		{
			uint8_t  HCIPacketsAllowable;
			uint16_t Opcode;
			uint8_t  Parameters[];
		} ATTR_PACKED BT_HCIEvent_CommandComplete_t;

		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
			uint8_t  ClassOfDevice_Service;
			uint16_t ClassOfDevice_MajorMinor;
			uint8_t  LinkType;
		} ATTR_PACKED BT_HCIEvent_ConnectionRequest_t;

		typedef struct
		{
			uint8_t  Status;
			uint16_t Handle;
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
			uint8_t  LinkType;
			uint8_t  EncryptionEnabled;
		} ATTR_PACKED BT_HCIEvent_ConnectionComplete_t;

		typedef struct
		{
			uint8_t  Status;
			uint16_t Handle;
			uint8_t  Reason;
		} ATTR_PACKED BT_HCIEvent_DisconnectionComplete_t;

		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
		} ATTR_PACKED BT_HCIEvent_PinCodeReq_t;

		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
		} ATTR_PACKED BT_HCIEvent_LinkKeyReq_t;

		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
		} ATTR_PACKED BT_HCICommand_LinkKeyNAKResp_t;

		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
			uint8_t  PINCodeLength;
			char     PINCode[16];
		} ATTR_PACKED BT_HCICommand_PinCodeResp_t;

		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
			uint8_t  SlaveRole;
		} ATTR_PACKED BT_HCICommand_AcceptConnectionReq_t;

		typedef struct
		{
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
			uint8_t  Reason;
		} ATTR_PACKED BT_HCICommand_RejectConnectionReq_t;
		
#endif
