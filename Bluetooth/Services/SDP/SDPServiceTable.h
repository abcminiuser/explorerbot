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

#ifndef _BLUETOOTH_SERVICE_SDP_TABLEDEFS_H_
#define _BLUETOOTH_SERVICE_SDP_TABLEDEFS_H_

	/* Macros: */
		/** Size of a full 128 bit UUID, in bytes. */
		#define UUID_SIZE_BYTES                         16

		/** First 80 bits common to all standardized Bluetooth services. */
		#define BASE_80BIT_UUID                         CPU_TO_BE16(0x0000), CPU_TO_BE16(0x0010), CPU_TO_BE16(0x0080), {0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB}

		#define RFCOMM_UUID                             {CPU_TO_BE32(0x00000003), BASE_80BIT_UUID}
		#define L2CAP_UUID                              {CPU_TO_BE32(0x00000100), BASE_80BIT_UUID}
		#define SP_CLASS_UUID                           {CPU_TO_BE32(0x00001101), BASE_80BIT_UUID}
		#define PUBLICBROWSEGROUP_CLASS_UUID            {CPU_TO_BE32(0x00001002), BASE_80BIT_UUID}

		#define SDP_ATTRIBUTE_ID_SERVICERECORDHANDLE    0x0000
		#define SDP_ATTRIBUTE_ID_SERVICECLASSIDS        0x0001
		#define SDP_ATTRIBUTE_ID_PROTOCOLDESCRIPTORLIST 0x0004
		#define SDP_ATTRIBUTE_ID_BROWSEGROUPLIST        0x0005
		#define SDP_ATTRIBUTE_ID_LANGUAGEBASEATTROFFSET 0x0006
		#define SDP_ATTRIBUTE_ID_SERVICENAME            0x0100
		#define SDP_ATTRIBUTE_ID_SERVICEDESCRIPTION     0x0101

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
			uint16_t    AttributeID; /**< Attribute ID of the table element which the UUID service supports */
			const void* Data; /**< Pointer to the attribute data, located in PROGMEM memory space */
		} ServiceAttributeTable_t;

		typedef struct ServiceEntry_t
		{
			const uint8_t                  TotalTableAttributes;
			const ServiceAttributeTable_t* AttributeTable;
			struct ServiceEntry_t*         NextService;
		} ServiceEntry_t;

		/** Structure for a list of Data Elements containing 8-bit integers, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t Header; /**< Data Element header, should be (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_8Bit) */
			uint8_t Value; /**< Value to store in the list Data Element */
		} Item8Bit_t;

		/** Structure for a list of Data Elements containing 16-bit integers, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t  Header; /**< Data Element header, should be (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_16Bit) */
			uint16_t Value; /**< Value to store in the list Data Element */
		} Item16Bit_t;

		/** Structure for a list of Data Elements containing 32-bit integers, for service attributes requiring such lists. */
		typedef struct
		{
			uint8_t  Header; /**< Data Element header, should be (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_32Bit) */
			uint32_t Value; /**< Value to store in the list Data Element */
		} Item32Bit_t;

		/** Structure for a list of Data Elements containing UUIDs, for service attributes requiring UUID lists. */
		typedef struct
		{
			uint8_t Header; /**< Data Element header, should be (SDP_DATATYPE_UUID | SDP_DATASIZE_128Bit) */
			UUID_t  UUID; /**< UUID to store in the list Data Element */
		} ItemUUID_t;

		/** Structure for a list of Data Elements Sequences containing UUID Data Elements, for service attributes requiring
		 *  protocol lists.
		 */
		typedef struct
		{
			uint8_t        Header; /**< Data Element header, should be (SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit) */
			uint8_t        Size; /**< Size of the inner Data Element sequence */

			struct
			{
				ItemUUID_t UUID; /**< UUID to store in the protocol list Data Element sequence */
			} Protocol;
		} ItemProtocol_t;

		/** Structure for a list of Data Elements Sequences containing UUID Data Elements and an 8-bit param value, for service
		 *  attributes requiring extended protocol lists containing an 8-bit value.
		 */
		typedef struct
		{
			uint8_t        Header; /**< Data Element header, should be (SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit) */
			uint8_t        Size; /**< Size of the inner Data Element sequence */

			struct
			{
				ItemUUID_t UUID; /**< UUID to store in the protocol list Data Element sequence */
				Item8Bit_t Param; /**< 8-Bit Parameter associated with the service */
			} Protocol;
		} ItemProtocol_8BitParam_t;

		/** Structure for a list of Data Elements Sequences containing UUID Data Elements and an 16-bit param value, for service
		 *  attributes requiring extended protocol lists containing an 16-bit value.
		 */
		typedef struct
		{
			uint8_t        Header; /**< Data Element header, should be (SDP_DATATYPE_Sequence | SDP_DATASIZE_Variable8Bit) */
			uint8_t        Size; /**< Size of the inner Data Element sequence */

			struct
			{
				ItemUUID_t UUID; /**< UUID to store in the protocol list Data Element sequence */
				Item16Bit_t Channel; /**< 16-Bit Parameter associated with the service */
			} Protocol;
		} ItemProtocol_16BitParam_t;

		/** Structure for a list of Data Elements containing language encodings, including the language ID, Encoding ID and
		 *  Attribute base offset.
		 */
		typedef struct
		{
			Item16Bit_t LanguageID; /**< Language ID for the current language */
			Item16Bit_t EncodingID; /**< Encoding used for the current language */
			Item16Bit_t OffsetID; /**< Attribute offset added to all strings using this language within the service */
		} ItemLangEncoding_t;
		
#endif
