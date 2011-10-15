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
 *  Bluetooth SDP service table definitions.
 */

#ifndef _BLUETOOTH_SERVICE_SDP_TABLEDEFS_H_
#define _BLUETOOTH_SERVICE_SDP_TABLEDEFS_H_

	/* Includes: */
		#include "SDP.h"

	/* Macros: */
		/** Size of a full 128 bit UUID, in bytes. */
		#define UUID_SIZE_BYTES                         16

		/** First 80 bits common to all standardized Bluetooth services. */
		#define BASE_80BIT_UUID                         CPU_TO_BE16(0x0000), CPU_TO_BE16(0x1000), CPU_TO_BE16(0x8000), {0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB}

		/** \name UUID values for common Bluetooth service and classes. */
		//@{
		#define RFCOMM_UUID                             {CPU_TO_BE32(0x00000003), BASE_80BIT_UUID}
		#define L2CAP_UUID                              {CPU_TO_BE32(0x00000100), BASE_80BIT_UUID}
		#define SP_CLASS_UUID                           {CPU_TO_BE32(0x00001101), BASE_80BIT_UUID}
		#define PUBLICBROWSEGROUP_CLASS_UUID            {CPU_TO_BE32(0x00001002), BASE_80BIT_UUID}
		//@}

		/** \name Bluetooth service attribute indexes for common service attributes. */
		//@{
		#define SDP_ATTRIBUTE_ID_SERVICERECORDHANDLE    0x0000
		#define SDP_ATTRIBUTE_ID_SERVICECLASSIDS        0x0001
		#define SDP_ATTRIBUTE_ID_PROTOCOLDESCRIPTORLIST 0x0004
		#define SDP_ATTRIBUTE_ID_BROWSEGROUPLIST        0x0005
		#define SDP_ATTRIBUTE_ID_LANGUAGEBASEATTROFFSET 0x0006
		#define SDP_ATTRIBUTE_ID_SERVICENAME            0x0100
		#define SDP_ATTRIBUTE_ID_SERVICEDESCRIPTION     0x0101
		//@}
		
		/** \name Convenience macros to encode SDP service attribute values for a given type and/or size. */
		//@{
		#define SDP_ITEM8BIT(Type, Value)               {(SDP_DATASIZE_8Bit   | (Type)), Value}
		#define SDP_ITEM16BIT(Type, Value)              {(SDP_DATASIZE_16Bit  | (Type)), Value}
		#define SDP_ITEM32BIT(Type, Value)              {(SDP_DATASIZE_32Bit  | (Type)), Value}
		#define SDP_ITEM64BIT(Type, Value)              {(SDP_DATASIZE_64Bit  | (Type)), Value}
		#define SDP_ITEMUUID(UUID)                      {(SDP_DATASIZE_128Bit | SDP_DATATYPE_UUID), UUID}
		#define SDP_ITEMSEQUENCE8BIT(Size)              {(SDP_DATASIZE_Variable8Bit | SDP_DATATYPE_Sequence), Size}
		#define SDP_ITEMSEQUENCE16BIT(Size)             {(SDP_DATASIZE_Variable16Bit | SDP_DATATYPE_Sequence), Size}
		#define SDP_ITEMSTRING(String)                  {(SDP_DATASIZE_Variable8Bit | SDP_DATATYPE_String), (sizeof(String) - 1)}, String
		//@}

	/* Enums: */
		/** Data sizes for SDP Data Element headers, to indicate the size of the data contained in the element. When creating
		 *  a Data Element, a value from this enum should be ORed with a value from the \ref SDP_Element_DataTypes_t enum.
		 */
		enum SDP_Element_DataSizes_t
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
		 *  a Data Element, a value from this enum should be ORed with a value from the \ref SDP_Element_DataSizes_t enum.
		 */
		enum SDP_Element_DataTypes_t
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
		/** Type define for a UUID value structure. This struct can be used to hold full 128-bit UUIDs. */
		typedef struct
		{
			uint32_t A; /**< Bits 0-31 of the UUID. */
			uint16_t B; /**< Bits 32-47 of the UUID. */
			uint16_t C; /**< Bits 48-63 of the UUID. */
			uint16_t D; /**< Bits 64-79 of the UUID. */
			uint8_t  E[6]; /**< Bits 80-127 of the UUID. */
		} UUID_t;
		
		/** Structure for the association of attribute ID values to an attribute value in FLASH. A table of these
		 *  structures can then be built up for each supported UUID service within the device.
		 */
		typedef struct
		{
			uint16_t    AttributeID; /**< Attribute ID of the table element which the UUID service supports. */
			const void* Data; /**< Pointer to the attribute data, located in PROGMEM memory space. */
		} SDP_ServiceAttributeTable_t;

		/** Structure for a list of Data Elements containing 8-bit integers, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t Header; /**< Data Element header, should be (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_8Bit). */
			uint8_t Value; /**< Value to store in the list Data Element. */
		} SDP_Item8Bit_t;

		/** Structure for a list of Data Elements containing 16-bit integers, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t  Header; /**< Data Element header, should be (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_16Bit). */
			uint16_t Value; /**< Value to store in the list Data Element. */
		} SDP_Item16Bit_t;

		/** Structure for a list of Data Elements containing 32-bit integers, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t  Header; /**< Data Element header, should be (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_32Bit). */
			uint32_t Value; /**< Value to store in the list Data Element. */
		} SDP_Item32Bit_t;

		/** Structure for a list of Data Elements containing 64-bit integers, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t  Header; /**< Data Element header, should be (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_64Bit). */
			uint64_t Value; /**< Value to store in the list Data Element. */
		} SDP_Item64Bit_t;

		/** Structure for a list of Data Elements containing 128-bit UUIDs, for service attributes requiring UUID lists. */
		typedef struct
		{
			uint8_t Header; /**< Data Element header, should be (SDP_DATATYPE_UUID | SDP_DATASIZE_128Bit). */
			UUID_t  UUID; /**< UUID to store in the list Data Element. */
		} SDP_ItemUUID_t;

		/** Structure for a list of Data Elements containing a string of maximum length 255, for service attributes requiring strings.
		 *
		 *  \note This structure should be immediately followed by the string buffer.
		 */
		typedef struct
		{
			uint8_t Header; /**< Data Element header, should be (SDP_DATATYPE_String | SDP_DATASIZE_Variable8Bit). */
			uint8_t Length; /**< Length of the string, in bytes. */
		} SDP_ItemString8Bit_t;

		/** Structure for a list of Data Elements containing child elements, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t Header; /**< Data Element header, should be (SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit). */
			uint8_t Size; /**< Size of the inner Data Element sequence. */
		} SDP_ItemSequence8Bit_t;

		/** Structure for a list of Data Elements containing child elements, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t  Header; /**< Data Element header, should be (SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable16Bit). */
			uint16_t Size; /**< Size of the inner Data Element sequence. */
		} SDP_ItemSequence16Bit_t;
		
#endif
