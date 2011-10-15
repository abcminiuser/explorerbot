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
 *  Bluetooth RFCOMM service SDP table definitions, to register the RFCOMM service with the SDP service.
 */

#include "RFCOMMServiceTable.h"

/** Serial Port Profile attribute, listing the unique service handle of the Serial Port service
 *  within the device. This handle can then be requested by the SDP client in future transactions
 *  in lieu of a search UUID list.
 */
static const SDP_Item32Bit_t PROGMEM SerialPort_Attribute_ServiceHandle = SDP_ITEM32BIT(SDP_DATATYPE_UnsignedInt, CPU_TO_BE32(0x00010000));

/** Serial Port Profile attribute, listing the implemented Service Class UUIDs of the Serial Port service
 *  within the device. This list indicates all the class UUIDs that apply to the Serial Port service, so that
 *  a SDP client can search by a generalized class rather than a specific UUID to determine supported services.
 */
static const struct
{
	SDP_ItemSequence8Bit_t UUIDList_Header;
	struct
	{
		SDP_ItemUUID_t     SerialPortUUID;
	} UUIDList;
} PROGMEM SerialPort_Attribute_ServiceClassIDs =
	{
		SDP_ITEMSEQUENCE8BIT(sizeof(SerialPort_Attribute_ServiceClassIDs.UUIDList)),
		{
			SDP_ITEMUUID(SP_CLASS_UUID),
		}
	};

/** Serial Port Profile attribute, listing the Protocols (and their attributes) of the Serial Port service
 *  within the device. This list indicates what protocols the service is layered on top of, as well as any
 *  configuration information for each layer.
 */
static const struct
{
	SDP_ItemSequence8Bit_t Protocols_Header;
	struct
	{
		SDP_ItemSequence8Bit_t L2CAP_Header;
		struct
		{
			SDP_ItemUUID_t UUID;
		} L2CAP;

		SDP_ItemSequence8Bit_t RFCOMM_Header;
		struct
		{
			SDP_ItemUUID_t UUID;
			SDP_Item8Bit_t ControlChannel;
		} RFCOMM;
	} Protocols;
} PROGMEM SerialPort_Attribute_ProtocolDescriptor =
	{
		SDP_ITEMSEQUENCE8BIT(sizeof(SerialPort_Attribute_ProtocolDescriptor.Protocols)),
		{
			SDP_ITEMSEQUENCE8BIT(sizeof(SerialPort_Attribute_ProtocolDescriptor.Protocols.L2CAP)),
			{
				SDP_ITEMUUID(L2CAP_UUID),
			},

			SDP_ITEMSEQUENCE8BIT((sizeof(SerialPort_Attribute_ProtocolDescriptor.Protocols.RFCOMM))),
			{
				SDP_ITEMUUID(RFCOMM_UUID),
				SDP_ITEM8BIT(SDP_DATATYPE_UnsignedInt, 0x01),
			},
		},
	};

/** Serial Port Profile attribute, listing the Browse Group List UUIDs which this service is a member of.
 *  Browse Group UUIDs give a way to group together services within a device in a simple hierarchy, so that
 *  a SDP client can progressively narrow down an general browse to a specific service which it requires.
 */
static const struct
{
	SDP_ItemSequence8Bit_t UUIDList_Header;
	struct
	{
		SDP_ItemUUID_t     PublicBrowseGroupUUID;
	} UUIDList;
} PROGMEM SerialPort_Attribute_BrowseGroupList =
	{
		SDP_ITEMSEQUENCE8BIT(sizeof(SerialPort_Attribute_BrowseGroupList.UUIDList)),
		{
			SDP_ITEMUUID(PUBLICBROWSEGROUP_CLASS_UUID),
		},
	};

/** Serial Port Profile attribute, listing the languages (and their encodings) supported
 *  by the Serial Port service in its text string attributes.
 */
static const struct
{
	SDP_ItemSequence8Bit_t LanguageEncoding_Header;
	struct
	{
		SDP_Item16Bit_t    LanguageID;
		SDP_Item16Bit_t    EncodingID;
		SDP_Item16Bit_t    OffsetID;
	} LanguageEncoding;
} PROGMEM SerialPort_Attribute_LanguageBaseIDOffset =
	{
		SDP_ITEMSEQUENCE8BIT(sizeof(SerialPort_Attribute_LanguageBaseIDOffset.LanguageEncoding)),
		{
			SDP_ITEM16BIT(SDP_DATATYPE_UnsignedInt, CPU_TO_BE16(0x454E)),
			SDP_ITEM16BIT(SDP_DATATYPE_UnsignedInt, CPU_TO_BE16(0x006A)),
			SDP_ITEM16BIT(SDP_DATATYPE_UnsignedInt, CPU_TO_BE16(0x0100)),
		},
	};

/** Serial Port Profile attribute, listing a human readable name of the service. */
static const struct
{
	SDP_ItemString8Bit_t Text_Header;
	char                 Text[];
} PROGMEM SerialPort_Attribute_ServiceName =
	{
		SDP_ITEMSTRING("Wireless Serial Port"),
	};

/** Serial Port Profile attribute, listing a human readable description of the service. */
static const struct
{
	SDP_ItemString8Bit_t Text_Header;
	char                 Text[];
} PROGMEM SerialPort_Attribute_ServiceDescription =
	{
		SDP_ITEMSTRING("Wireless Serial Port Service"),
	};

/** Service Attribute Table for the Serial Port service, linking each supported attribute ID to its data, so that
 *  the SDP server can retrieve it for transmission back to a SDP client upon request.
 */
static const SDP_ServiceAttributeTable_t PROGMEM SerialPort_Attribute_Table[] =
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
