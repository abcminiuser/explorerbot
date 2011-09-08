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

	/* Inline Functions: */
		/** Writes 8 bits of raw data to the given buffer, incrementing the buffer position afterwards.
		 *
		 *  \param[in, out] BufferPos  Current position in the buffer where the data is to be written to
		 *  \param[in]      Data       Data to write to the buffer
		 */
		static inline void SDP_WriteData8(void** BufferPos,
		                                  const uint8_t Data)
		{
			*((uint8_t*)*BufferPos) = Data;
			*BufferPos += sizeof(uint8_t);
		}

		/** Writes 16 bits of raw data to the given buffer, incrementing the buffer position afterwards.
		 *
		 *  \param[in, out] BufferPos  Current position in the buffer where the data is to be written to
		 *  \param[in]      Data       Data to write to the buffer
		 */
		static inline void SDP_WriteData16(void** BufferPos,
		                                   const uint16_t Data)
		{
			*((uint16_t*)*BufferPos) = cpu_to_be16(Data);
			*BufferPos += sizeof(uint16_t);
		}

		/** Writes 32 bits of raw data to the given buffer, incrementing the buffer position afterwards.
		 *
		 *  \param[in, out] BufferPos  Current position in the buffer where the data is to be written to
		 *  \param[in]      Data       Data to write to the buffer
		 */
		static inline void SDP_WriteData32(void** BufferPos,
		                                   const uint32_t Data)
		{
			*((uint32_t*)*BufferPos) = cpu_to_be32(Data);
			*BufferPos += sizeof(uint32_t);
		}

		/** Reads 8 bits of raw data from the given buffer, incrementing the buffer position afterwards.
		 *
		 *  \param[in, out] BufferPos  Current position in the buffer where the data is to be read from
		 *
		 *  \return Data read from the buffer
		 */
		static inline uint8_t SDP_ReadData8(const void** BufferPos)
		{
			uint8_t Data = *((const uint8_t*)*BufferPos);
			*BufferPos += sizeof(uint8_t);

			return Data;
		}

		/** Reads 16 bits of raw data from the given buffer, incrementing the buffer position afterwards.
		 *
		 *  \param[in, out] BufferPos  Current position in the buffer where the data is to be read from
		 *
		 *  \return Data read from the buffer
		 */
		static inline uint16_t SDP_ReadData16(const void** BufferPos)
		{
			uint16_t Data = be16_to_cpu(*((const uint16_t*)*BufferPos));
			*BufferPos += sizeof(uint16_t);

			return Data;
		}

		/** Reads 32 bits of raw data from the given buffer, incrementing the buffer position afterwards.
		 *
		 *  \param[in, out] BufferPos  Current position in the buffer where the data is to be read from
		 *
		 *  \return Data read from the buffer
		 */
		static inline uint32_t SDP_ReadData32(const void** BufferPos)
		{
			uint32_t Data = be32_to_cpu(*((const uint32_t*)*BufferPos));
			*BufferPos += sizeof(uint32_t);

			return Data;
		}

		/** Adds a new Data Element Sequence container with a 16-bit size header to the buffer. The buffer
		 *  pointer's position is advanced past the added header once the element has been added. The returned
		 *  size header value is pre-zeroed out so that it can be incremented as data is placed into the Data
		 *  Element Sequence container.
		 *
		 *  The total added size of the container header is three bytes, regardless of the size of its contents
		 *  as long as the contents' size in bytes fits into a 16-bit integer.
		 *
		 *  \param[in, out] BufferPos  Pointer to a buffer where the container header is to be placed
		 *
		 *  \return Pointer to the 16-bit size value of the container header, which has been pre-zeroed
		 */
		static inline uint16_t* SDP_AddSequence16(void** BufferPos)
		{
			SDP_WriteData8(BufferPos, (SDP_DATASIZE_Variable16Bit | SDP_DATATYPE_Sequence));

			uint16_t* SizePos = *BufferPos;
			SDP_WriteData16(BufferPos, 0);

			return SizePos;
		}

#endif
