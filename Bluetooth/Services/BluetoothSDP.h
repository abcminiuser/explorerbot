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

#ifndef _SERVICEDISCOVERYPROTOCOL_H_
#define _SERVICEDISCOVERYPROTOCOL_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>

		#include "../Bluetooth.h"
		#include "BluetoothSDPDefinitions.h"
		#include "BluetoothSDPTableDefinitions.h"

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

	/* Function Prototypes: */
		void Bluetooth_SDP_ProcessPacket(BT_StackConfig_t* const StackState,
                                         BT_HCI_Connection_t* const Connection,
                                         BT_ACL_Channel_t* const Channel,
                                         uint16_t Length,
                                         uint8_t* Data);

#endif

