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

#ifndef _BLUETOOTH_SERVICE_SDP_DEFS_H_
#define _BLUETOOTH_SERVICE_SDP_DEFS_H_

	/* Enums: */
		enum ServiceDiscovery_PDU_t
		{
			SDP_PDU_ERRORRESPONSE                  = 0x01,
			SDP_PDU_SERVICESEARCHREQUEST           = 0x02,
			SDP_PDU_SERVICESEARCHRESPONSE          = 0x03,
			SDP_PDU_SERVICEATTRIBUTEREQUEST        = 0x04,
			SDP_PDU_SERVICEATTRIBUTERESPONSE       = 0x05,
			SDP_PDU_SERVICESEARCHATTRIBUTEREQUEST  = 0x06,
			SDP_PDU_SERVICESEARCHATTRIBUTERESPONSE = 0x07,	
		};
	
		/** Data sizes for SDP Data Element headers, to indicate the size of the data contained in the element. When creating
		 *  a Data Element, a value from this enum should be ORed with a value from the \ref ServiceDiscovery_DataTypes_t enum.
		 */
		enum ServiceDiscovery_DataSizes_t
		{
			SDP_DATASIZE_8Bit                      = 0, /**< Contained data is 8 bits in length. */
			SDP_DATASIZE_16Bit                     = 1, /**< Contained data is 16 bits in length. */
			SDP_DATASIZE_32Bit                     = 2, /**< Contained data is 32 bits in length. */
			SDP_DATASIZE_64Bit                     = 3, /**< Contained data is 64 bits in length. */
			SDP_DATASIZE_128Bit                    = 4, /**< Contained data is 128 bits in length. */
			SDP_DATASIZE_Variable8Bit              = 5, /**< Contained data is encoded in an 8 bit size integer following the header. */
			SDP_DATASIZE_Variable16Bit             = 6, /**< Contained data is encoded in an 16 bit size integer following the header. */
			SDP_DATASIZE_Variable32Bit             = 7, /**< Contained data is encoded in an 32 bit size integer following the header. */
		};

		/** Data types for SDP Data Element headers, to indicate the type of data contained in the element. When creating
		 *  a Data Element, a value from this enum should be ORed with a value from the \ref ServiceDiscovery_DataSizes_t enum.
		 */
		enum ServiceDiscovery_DataTypes_t
		{
			SDP_DATATYPE_Nill                     = (0 << 3), /**< Indicates the container data is a Nill (null) type. */
			SDP_DATATYPE_UnsignedInt              = (1 << 3), /**< Indicates the container data is an unsigned integer. */
			SDP_DATATYPE_SignedInt                = (2 << 3), /**< Indicates the container data is a signed integer. */
			SDP_DATATYPE_UUID                     = (3 << 3), /**< Indicates the container data is a UUID. */
			SDP_DATATYPE_String                   = (4 << 3), /**< Indicates the container data is an ASCII string. */
			SDP_DATATYPE_Boolean                  = (5 << 3), /**< Indicates the container data is a logical boolean. */
			SDP_DATATYPE_Sequence                 = (6 << 3), /**< Indicates the container data is a sequence of containers. */
			SDP_DATATYPE_Alternative              = (7 << 3), /**< Indicates the container data is a sequence of alternative containers. */
			SDP_DATATYPE_URL                      = (8 << 3), /**< Indicates the container data is a URL. */
		};

	/* Type Defines: */
		/** Header for all SDP transaction packets. This header is sent at the start of all SDP packets sent to or from a SDP
		 *  server.
		 */
		typedef struct
		{
			uint8_t  PDU; /**< SDP packet type, a SDP_PDU_* mask value */
			uint16_t TransactionID; /**< Unique transaction ID number to associate requests and responses */
			uint16_t ParameterLength; /**< Length of the data following the SDP header */
			uint8_t  Parameters[];
		} BT_SDP_PDUHeader_t;
		
#endif
