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

#include "RFCOMMServiceTable.h"

/** Serial Port Profile attribute, listing the unique service handle of the Serial Port service
 *  within the device. This handle can then be requested by the SDP client in future transactions
 *  in lieu of a search UUID list.
 */
const struct
{
	uint8_t  Header;
	uint32_t Data;
} PROGMEM SerialPort_Attribute_ServiceHandle =
	{
		(SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_32Bit),
		CPU_TO_BE32(0x00010001),
	};

/** Serial Port Profile attribute, listing the implemented Service Class UUIDs of the Serial Port service
 *  within the device. This list indicates all the class UUIDs that apply to the Serial Port service, so that
 *  a SDP client can search by a generalized class rather than a specific UUID to determine supported services.
 */
const struct
{
	uint8_t        Header;
	uint8_t        Size;
	SDP_ItemUUID_t UUIDList[];
} PROGMEM SerialPort_Attribute_ServiceClassIDs =
	{
		(SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit),
		(sizeof(SDP_ItemUUID_t) * 1),
		{
			{(SDP_DATATYPE_UUID | SDP_DATASIZE_128Bit), SP_CLASS_UUID},
		},
	};

/** Serial Port Profile attribute, listing the Protocols (and their attributes) of the Serial Port service
 *  within the device. This list indicates what protocols the service is layered on top of, as well as any
 *  configuration information for each layer.
 */
const struct
{
	uint8_t                      Header;
	uint8_t                      Size;

	SDP_ItemProtocol_t           L2CAP;
	SDP_ItemProtocol_8BitParam_t RFCOMM;
} PROGMEM SerialPort_Attribute_ProtocolDescriptor =
	{
		(SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit),
		(sizeof(SDP_ItemProtocol_t) + sizeof(SDP_ItemProtocol_8BitParam_t)),
		{
			(SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit),
			sizeof(SDP_ItemUUID_t),
			{
				{(SDP_DATATYPE_UUID | SDP_DATASIZE_128Bit), L2CAP_UUID},
			},
		},
		{
			(SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit),
			(sizeof(SDP_ItemUUID_t) + sizeof(SDP_Item8Bit_t)),
			{
				{(SDP_DATATYPE_UUID | SDP_DATASIZE_128Bit), RFCOMM_UUID},
				{(SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_8Bit), 0x01},
			},
		},
	};

/** Serial Port Profile attribute, listing the Browse Group List UUIDs which this service is a member of.
 *  Browse Group UUIDs give a way to group together services within a device in a simple hierarchy, so that
 *  a SDP client can progressively narrow down an general browse to a specific service which it requires.
 */
const struct
{
	uint8_t        Header;
	uint8_t        Size;
	SDP_ItemUUID_t UUIDList[];
} PROGMEM SerialPort_Attribute_BrowseGroupList =
	{
		(SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit),
		(sizeof(SDP_ItemUUID_t) * 1),
		{
			{(SDP_DATATYPE_UUID | SDP_DATASIZE_128Bit), PUBLICBROWSEGROUP_CLASS_UUID},
		},
	};

/** Serial Port Profile attribute, listing the languages (and their encodings) supported
 *  by the Serial Port service in its text string attributes.
 */
const struct
{
	uint8_t                Header;
	uint8_t                Size;
	SDP_ItemLangEncoding_t LanguageEncodings[];
} PROGMEM SerialPort_Attribute_LanguageBaseIDOffset =
	{
		(SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit),
		(sizeof(SDP_ItemLangEncoding_t) * 1),
		{
			{
				{(SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_16Bit), SWAPENDIAN_16(0x454E)},
				{(SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_16Bit), SWAPENDIAN_16(0x006A)},
				{(SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_16Bit), SWAPENDIAN_16(0x0100)},
			},
		},
	};

/** Serial Port Profile attribute, listing a human readable name of the service. */
const struct
{
	uint8_t Header;
	uint8_t Size;
	char    Text[];
} PROGMEM SerialPort_Attribute_ServiceName =
	{
		(SDP_DATATYPE_String | SDP_DATASIZE_Variable8Bit),
		sizeof("Wireless Serial Port") - 1,
		"Wireless Serial Port",
	};

/** Serial Port Profile attribute, listing a human readable description of the service. */
const struct
{
	uint8_t Header;
	uint8_t Size;
	char    Text[];
} PROGMEM SerialPort_Attribute_ServiceDescription =
	{
		(SDP_DATATYPE_String | SDP_DATASIZE_Variable8Bit),
		sizeof("Wireless Serial Port Service") - 1,
		"Wireless Serial Port Service",
	};

/** Service Attribute Table for the Serial Port service, linking each supported attribute ID to its data, so that
 *  the SDP server can retrieve it for transmission back to a SDP client upon request.
 */
const SDP_ServiceAttributeTable_t PROGMEM SerialPort_Attribute_Table[] =
	{
		{SDP_ATTRIBUTE_ID_SERVICERECORDHANDLE,    &SerialPort_Attribute_ServiceHandle       },
		{SDP_ATTRIBUTE_ID_SERVICECLASSIDS,        &SerialPort_Attribute_ServiceClassIDs     },
		{SDP_ATTRIBUTE_ID_PROTOCOLDESCRIPTORLIST, &SerialPort_Attribute_ProtocolDescriptor  },
		{SDP_ATTRIBUTE_ID_BROWSEGROUPLIST,        &SerialPort_Attribute_BrowseGroupList     },
		{SDP_ATTRIBUTE_ID_LANGUAGEBASEATTROFFSET, &SerialPort_Attribute_LanguageBaseIDOffset},
		{SDP_ATTRIBUTE_ID_SERVICENAME,            &SerialPort_Attribute_ServiceName         },
		{SDP_ATTRIBUTE_ID_SERVICEDESCRIPTION,     &SerialPort_Attribute_ServiceDescription  },
	};

/** Service entry node, used to register the attribute table with the SDP service */
SDP_ServiceEntry_t ServiceEntry_RFCOMMSerialPort =
	{
		.Stack                = NULL,
		.TotalTableAttributes = (sizeof(SerialPort_Attribute_Table) / sizeof(SerialPort_Attribute_Table[0])),
		.AttributeTable       = SerialPort_Attribute_Table,
	};
