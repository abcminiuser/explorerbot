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
 *  Header file for SDP.c.
 */

#ifndef _SDP_SERVICE_H_
#define _SDP_SERVICE_H_

	/* Includes: */
		#include <stdbool.h>
		#include <stdint.h>

		#include "../../Bluetooth.h"
		#include "SDPServiceTable.h"

	/* Enums: */
		enum SDP_PDU_t
		{
			SDP_PDU_ERRORRESPONSE                  = 0x01,
			SDP_PDU_SERVICESEARCHREQUEST           = 0x02,
			SDP_PDU_SERVICESEARCHRESPONSE          = 0x03,
			SDP_PDU_SERVICEATTRIBUTEREQUEST        = 0x04,
			SDP_PDU_SERVICEATTRIBUTERESPONSE       = 0x05,
			SDP_PDU_SERVICESEARCHATTRIBUTEREQUEST  = 0x06,
			SDP_PDU_SERVICESEARCHATTRIBUTERESPONSE = 0x07,	
		};

	/* Type Defines: */
		/** Header for all SDP transaction packets. This header is sent at the start of all SDP packets sent to or from a SDP
		 *  server.
		 */
		typedef struct
		{
			uint8_t  PDU; /**< SDP packet type, a SDP_PDU_* mask value. */
			uint16_t TransactionID; /**< Unique transaction ID number to associate requests and responses. */
			uint16_t ParameterLength; /**< Length of the data following the SDP header. */
			uint8_t  Parameters[]; /**< SDP packet parameters contained in the packet. */
		} ATTR_PACKED BT_SDP_PDUHeader_t;

		/** Structure for a SDP service entry node, which links a service attribute table to a particular stack instance. For each service that is to
		 *  be registered to a stack in the SDP service, an instance of this structure must be created and filled out before registering the node in the
		 *  SDP service list via \ref SDP_RegisterService().
		 */
		typedef struct SDP_ServiceEntry_t
		{
			BT_StackConfig_t*                  Stack; /**< Stack the service node should be registered to, or NULL if all stacks. */
			const uint8_t                      TotalTableAttributes; /**< Total number of attributes stored in the service's attribute table. */
			const SDP_ServiceAttributeTable_t* AttributeTable; /**< Attribute table of the service, stored in PROGMEM. */

			struct SDP_ServiceEntry_t*         NextService; /**< Pointer to the next service node in the registration list (for internal use only). */
		} SDP_ServiceEntry_t;

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
		static inline uint16_t* SDP_WriteSequenceHeader16(void** BufferPos)
		{
			SDP_WriteData8(BufferPos, (SDP_DATASIZE_Variable16Bit | SDP_DATATYPE_Sequence));

			uint16_t* SizePos = *BufferPos;
			SDP_WriteData16(BufferPos, 0);

			return SizePos;
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
		
	/* Function Prototypes: */
		void SDP_RegisterService(SDP_ServiceEntry_t* const ServiceEntry);
		void SDP_UnregisterService(SDP_ServiceEntry_t* const ServiceEntry);

		void SDP_Init(BT_StackConfig_t* const StackState);
		void SDP_Manage(BT_StackConfig_t* const StackState);
		void SDP_ChannelOpened(BT_StackConfig_t* const StackState,
		                       BT_L2CAP_Channel_t* const Channel);
		void SDP_ChannelClosed(BT_StackConfig_t* const StackState,
		                       BT_L2CAP_Channel_t* const Channel);
		void SDP_ProcessPacket(BT_StackConfig_t* const StackState,
                               BT_L2CAP_Channel_t* const Channel,
                               const uint16_t Length,
                               uint8_t* Data);

#endif

