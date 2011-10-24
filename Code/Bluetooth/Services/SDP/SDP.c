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
 *  Bluetooth Service Discovery Protocol (SDP) service.
 */

#include "SDP.h"

/** Linked list of service attribute tables registered with the SDP service. */
static SDP_ServiceEntry_t* RegisteredServices = NULL;

/** Base UUID value common to all standardized Bluetooth services */
const UUID_t BaseUUID PROGMEM = {0x00000000, BASE_80BIT_UUID};


/** Registers the given service entry node with the SDP server, so that remote devices may query
 *  the added service's attribute table.
 *
 *  \param[in] ServiceEntry  Pointer to the service table node to register with the SDP service
 */
void SDP_RegisterService(SDP_ServiceEntry_t* const ServiceEntry)
{
	SDP_ServiceEntry_t* CurrentService = RegisteredServices;

	/* No service currently registered, save new service as the root node */
	if (!(CurrentService))
	{
		RegisteredServices = ServiceEntry;
		return;
	}

	/* Transverse service linked list to find the tail node */
	while (CurrentService->NextService != NULL)
	  CurrentService = CurrentService->NextService;

	/* Insert new service as the new tail node */
	CurrentService->NextService = ServiceEntry;
	
	/* Make sure new node indicates that it is the tail node */
	ServiceEntry->NextService   = NULL;
}

/** Unregisters the given service entry node with the SDP server, so that remote devices cannot query
 *  the service's attribute table if it was previously registered.
 *
 *  \param[in] ServiceEntry  Pointer to the service table node to register with the SDP service
 */
void SDP_UnregisterService(SDP_ServiceEntry_t* const ServiceEntry)
{
	SDP_ServiceEntry_t* CurrentService = RegisteredServices;

	/* No service currently registered, abort */
	if (!(CurrentService))
	  return;

	/* Root registered service is the service to remove */
	if (CurrentService == ServiceEntry)
	{
		RegisteredServices = CurrentService->NextService;
		return;
	}

	/* Transverse service linked list to find the inserted node */
	while ((CurrentService->NextService != NULL) && (CurrentService->NextService != ServiceEntry))
	  CurrentService = CurrentService->NextService;

	/* Remove service if found */
	if (CurrentService->NextService)
	  CurrentService->NextService = CurrentService->NextService->NextService;
}

static bool SDP_SendFrame(BT_StackConfig_t* const StackState,
                          BT_L2CAP_Channel_t* const Channel,
                          const uint16_t Length,
                          const void* const Data)
{
	return Bluetooth_L2CAP_SendPacket(StackState, Channel, Length, Data);
}

/** Retrieves the size of a Data Element container from the current input buffer, and advances the input buffer
 *  pointer to the start of the Data Element's contents.
 *
 *  \param[in, out] DataElementHeader  Pointer to the start of a Data Element header
 *  \param[out]     ElementHeaderSize  Size in bytes of the header that was skipped
 *
 *  \return Size in bytes of the Data Element container's contents, minus the header
 */
static uint32_t SDP_GetDataElementSize(const void** const DataElementHeader,
                                       uint8_t* const ElementHeaderSize)
{
	/* Fetch the size of the Data Element structure from the header, increment the current buffer pos */
	uint8_t  SizeIndex = (SDP_ReadData8(DataElementHeader) & 0x07);

	uint32_t ElementValueSize;

	/* Convert the Data Element size index into a size in bytes */
	switch (SizeIndex)
	{
		case SDP_DATASIZE_Variable8Bit:
			*ElementHeaderSize  = (1 + sizeof(uint8_t));
			ElementValueSize    = SDP_ReadData8(DataElementHeader);
			break;
		case SDP_DATASIZE_Variable16Bit:
			*ElementHeaderSize  = (1 + sizeof(uint16_t));
			ElementValueSize    = SDP_ReadData16(DataElementHeader);
			break;
		case SDP_DATASIZE_Variable32Bit:
			*ElementHeaderSize  = (1 + sizeof(uint32_t));
			ElementValueSize    = SDP_ReadData32(DataElementHeader);
			break;
		default:
			*ElementHeaderSize  = 1;
			ElementValueSize    = (1 << SizeIndex);
			break;
	}

	return ElementValueSize;
}

/** Retrieves the total size of the given locally stored (in PROGMEM) attribute Data Element container.
 *
 *  \param[in]  AttributeData  Pointer to the start of the Attribute container, located in PROGMEM
 *  \param[out] HeaderSize     Pointer to a location where the header size of the data element is to be stored
 *
 *  \return Size in bytes of the entire attribute container, including the header
 */
static uint32_t SDP_GetLocalAttributeContainerSize(const void* const AttributeData,
                                                   uint8_t* const HeaderSize)
{
	/* Fetch the size of the Data Element structure from the header */
	uint8_t SizeIndex = (pgm_read_byte(AttributeData) & 0x07);

	uint32_t ElementValueSize;

	/* Convert the Data Element size index into a size in bytes */
	switch (SizeIndex)
	{
		case SDP_DATASIZE_Variable8Bit:
			*HeaderSize = (1 + sizeof(uint8_t));
			ElementValueSize = pgm_read_byte(AttributeData + 1);
			break;
		case SDP_DATASIZE_Variable16Bit:
			*HeaderSize = (1 + sizeof(uint16_t));
			ElementValueSize = be16_to_cpu(pgm_read_word(AttributeData + 1));
			break;
		case SDP_DATASIZE_Variable32Bit:
			*HeaderSize = (1 + sizeof(uint32_t));
			ElementValueSize = be32_to_cpu(pgm_read_dword(AttributeData + 1));
			break;
		default:
			*HeaderSize = 1;
			ElementValueSize = (1 << SizeIndex);
			break;
	}

	return ElementValueSize;
}

/** Recursively unwraps the given locally stored attribute (in PROGMEM space), searching for UUIDs to match against
 *  the given UUID list. As matches are found, they are indicated in the UUIDMatch flag list.
 *
 *  \param[in]      UUIDList        List of UUIDs which must be matched within the service attribute table
 *  \param[in]      TotalUUIDs      Total number of UUIDs stored in the UUID list
 *  \param[in, out] UUIDMatchFlags  Array of flags indicating which UUIDs in the list have already been matched
 *  \param[in]      CurrAttribute   Pointer to the current attribute to search through
 *
 *  \return True if all the UUIDs given in the UUID list appear in the given attribute table, false otherwise
 */
static void SDP_CheckUUIDMatch(uint8_t UUIDList[][UUID_SIZE_BYTES],
                               const uint8_t TotalUUIDs,
                               uint16_t* const UUIDMatchFlags,
                               const void* CurrAttribute)
{
	uint8_t CurrAttributeType = (pgm_read_byte(CurrAttribute) & ~0x07);

	/* Check the data type of the current attribute value - if UUID, compare, if Sequence, unwrap and recurse */
	if (CurrAttributeType == SDP_DATATYPE_UUID)
	{
		uint16_t CurrUUIDMatchMask = (1 << 0);

		/* Look for matches in the UUID list against the current attribute UUID value */
		for (uint8_t i = 0; i < TotalUUIDs; i++)
		{
			/* Check if the current unmatched UUID is identical to the search UUID */
			if (!(*UUIDMatchFlags & CurrUUIDMatchMask) && !(memcmp_P(UUIDList[i], (CurrAttribute + 1), UUID_SIZE_BYTES)))
			{
				/* Indicate match found for the current attribute UUID and early-abort */
				*UUIDMatchFlags |= CurrUUIDMatchMask;
				break;
			}

			CurrUUIDMatchMask <<= 1;
		}
	}
	else if (CurrAttributeType == SDP_DATATYPE_Sequence)
	{
		uint8_t  SequenceHeaderSize;
		uint16_t SequenceSize = SDP_GetLocalAttributeContainerSize(CurrAttribute, &SequenceHeaderSize);

		CurrAttribute += SequenceHeaderSize;

		/* Recursively unwrap the sequence container, and re-search its contents for UUIDs */
		while (SequenceSize)
		{
			uint8_t  InnerHeaderSize;
			uint16_t InnerSize = SDP_GetLocalAttributeContainerSize(CurrAttribute, &InnerHeaderSize);

			/* Recursively search of the next element in the sequence, trying to match UUIDs with the UUID list */
			SDP_CheckUUIDMatch(UUIDList, TotalUUIDs, UUIDMatchFlags, CurrAttribute);

			/* Skip to the next element in the sequence */
			SequenceSize  -= InnerHeaderSize + InnerSize;
			CurrAttribute += InnerHeaderSize + InnerSize;
		}
	}
}

/** Reads in the collection of Attribute ranges from the input buffer's Data Element Sequence container, into the given
 *  Attribute list for later use. Once complete, the input buffer pointer is advanced to the end of the Attribute container.
 *
 *  \param[out] AttributeList     Pointer to a buffer where the list of Attribute ranges are to be stored
 *  \param[in]  CurrentParameter  Pointer to a Buffer containing a Data Element Sequence of Attribute and Attribute Range elements
 *
 *  \return Total number of Attribute ranges stored in the Data Element Sequence
 */
static uint8_t SDP_GetAttributeList(uint16_t AttributeList[][2],
                                    const void** const CurrentParameter)
{
	uint8_t ElementHeaderSize;
	uint8_t TotalAttributes = 0;

	/* Retrieve the total size of the Attribute container, and unwrap the outer Data Element Sequence container */
	uint16_t AttributeIDListLength = SDP_GetDataElementSize(CurrentParameter, &ElementHeaderSize);

	while (AttributeIDListLength)
	{
		/* Retrieve the size of the next Attribute in the container and get a pointer to the next free Attribute element in the list */
		uint16_t* CurrentAttributeRange = AttributeList[TotalAttributes++];
		uint8_t   AttributeLength       = SDP_GetDataElementSize(CurrentParameter, &ElementHeaderSize);

		/* Copy over the starting Attribute ID and (if it the current element is a range) the ending Attribute ID */
		memcpy(&CurrentAttributeRange[0], *CurrentParameter, AttributeLength);

		/* If the element is not an Attribute Range, copy over the starting ID to the ending ID to make a range of 1 */
		if (AttributeLength == 2)
		  CurrentAttributeRange[1] = CurrentAttributeRange[0];

		/* Swap the endianness of the attribute range values */
		CurrentAttributeRange[0] = be16_to_cpu(CurrentAttributeRange[0]);
		CurrentAttributeRange[1] = be16_to_cpu(CurrentAttributeRange[1]);

		AttributeIDListLength -= (AttributeLength + ElementHeaderSize);
		*CurrentParameter     += AttributeLength;
	}

	return TotalAttributes;
}

/** Reads in the collection of UUIDs from the input buffer's Data Element Sequence container, into the given
 *  UUID list for later use. Once complete, the input buffer pointer is advanced to the end of the UUID container.
 *
 *  \param[out] UUIDList          Pointer to a buffer where the list of UUIDs are to be stored
 *  \param[in]  CurrentParameter  Pointer to a Buffer containing a Data Element Sequence of UUID elements
 *
 *  \return Total number of UUIDs stored in the Data Element Sequence
 */
static uint8_t SDP_GetUUIDList(uint8_t UUIDList[][UUID_SIZE_BYTES],
                               const void** const CurrentParameter)
{
	uint8_t ElementHeaderSize;
	uint8_t TotalUUIDs = 0;

	/* Retrieve the total size of the UUID container, and unwrap the outer Data Element Sequence container */
	uint16_t ServicePatternLength = SDP_GetDataElementSize(CurrentParameter, &ElementHeaderSize);

	while (ServicePatternLength)
	{
		/* Retrieve the size of the next UUID in the container and get a pointer to the next free UUID element in the list */
		uint8_t* CurrentUUID = UUIDList[TotalUUIDs++];
		uint8_t  UUIDLength  = SDP_GetDataElementSize(CurrentParameter, &ElementHeaderSize);

		/* Copy over UUID from the container to the free slot */
		if (UUIDLength <= 4)
		{
			/* Copy over the base UUID value to the free UUID slot in the list */
			memcpy_P(CurrentUUID, &BaseUUID, sizeof(BaseUUID));

			/* Copy over short UUID */
			memcpy(CurrentUUID + (4 - UUIDLength), *CurrentParameter, UUIDLength);
		}
		else
		{
			/* Copy over full UUID */
			memcpy(CurrentUUID, *CurrentParameter, UUIDLength);
		}

		ServicePatternLength -= (UUIDLength + ElementHeaderSize);
		*CurrentParameter    += UUIDLength;
	}

	return TotalUUIDs;
}

/** Retrieves the Attribute table for the given UUID list if it exists.
 *
 *  \param[in] ServiceEntry  Pointer to the service entry whose attribute table is to be examined
 *  \param[in] UUIDList      List of UUIDs which must be matched within the service attribute table
 *  \param[in] TotalUUIDs    Total number of UUIDs stored in the UUID list
 *
 *  \return True if all the UUIDs given in the UUID list appear in the given attribute table, false otherwise
 */
static bool SDP_SearchServiceTable(const SDP_ServiceEntry_t* const ServiceEntry,
								   uint8_t UUIDList[][UUID_SIZE_BYTES],
                                   const uint8_t TotalUUIDs)
{
	uint16_t UUIDMatchFlags = 0;
	
	/* Search through the current attribute table, checking each attribute value for UUID matches */\
	for (uint8_t i = 0; i < ServiceEntry->TotalTableAttributes; i++)
	{
		const void* CurrAttribute = pgm_read_ptr(&ServiceEntry->AttributeTable[i].Data);
		SDP_CheckUUIDMatch(UUIDList, TotalUUIDs, &UUIDMatchFlags, CurrAttribute);
	}

	/* Determine how many UUID matches in the list we have found */
	uint8_t UUIDMatches;
	for (UUIDMatches = 0; UUIDMatchFlags; UUIDMatches++)
	  UUIDMatchFlags &= (UUIDMatchFlags - 1);

	/* If all UUIDs have been matched to the current service, return true */
	return (UUIDMatches == TotalUUIDs);
}

/** Adds the given attribute ID and value to the response buffer, and advances the response buffer pointer past the added data.
 *
 *  \param[in] AttributeID          Attribute ID to add to the response buffer
 *  \param[in] AttributeValue       Pointer to the start of the Attribute's value, located in PROGMEM
 *  \param[in, out] ResponseBuffer  Pointer to a buffer where the Attribute and Attribute Value is to be added
 *
 *  \return Number of bytes added to the response buffer
 */
static uint16_t SDP_AddAttributeToResponse(const uint16_t AttributeID,
                                           const void* AttributeValue,
                                           void** ResponseBuffer)
{
	/* Retrieve the size of the attribute value from its container header */
	uint8_t  AttributeHeaderLength;
	uint16_t AttributeValueLength = SDP_GetLocalAttributeContainerSize(AttributeValue, &AttributeHeaderLength);

	/* Add a Data Element header to the response for the Attribute ID */
	SDP_WriteData8(ResponseBuffer, (SDP_DATATYPE_UnsignedInt | SDP_DATASIZE_16Bit));

	/* Add the Attribute ID to the created Data Element */
	SDP_WriteData16(ResponseBuffer, AttributeID);

	/* Copy over the Attribute value Data Element container to the response */
	memcpy_P(*ResponseBuffer, AttributeValue, AttributeHeaderLength + AttributeValueLength);
	*ResponseBuffer += AttributeHeaderLength + AttributeValueLength;

	return (sizeof(uint8_t) + sizeof(uint16_t) + AttributeHeaderLength + AttributeValueLength);
}

/** Retrieves a pointer to the value of the given Attribute ID from the given Attribute table.
 *
 *  \param[in] ServiceEntry    Pointer to the service entry whose attribute table is to be examined
 *  \param[in] AttributeID     Attribute ID to search for within the table
 *
 *  \return Pointer to the start of the Attribute's value if found within the table, NULL otherwise
 */
static void* SDP_GetAttributeValue(const SDP_ServiceEntry_t* const ServiceEntry,
                                   const uint16_t AttributeID)
{
	/* Search through the current Attribute table, abort when the terminator item has been reached */
	for (uint8_t i = 0; i < ServiceEntry->TotalTableAttributes; i++)
	{
		/* Check if the current Attribute ID matches the search ID - if so return a pointer to it */
		if (pgm_read_word(&ServiceEntry->AttributeTable[i].AttributeID) == AttributeID)
		  return pgm_read_ptr(&ServiceEntry->AttributeTable[i].Data);
	}

	return NULL;
}

/** Adds all the Attributes in the given service table to the response that appear in the Attribute table.
 *
 *  \param[in]  ServiceEntry     Pointer to the service entry whose attribute table is to be examined
 *  \param[in]  AttributeList    Pointer to a list of Attribute ranges
 *  \param[in]  TotalAttributes  Number of Attributes stored in the Attribute list
 *  \param[out] BufferPos        Pointer to the output buffer position where the retrieved attributes are to be stored
 *
 *  \return Number of bytes added to the output buffer
 */
static uint16_t SDP_AddListedAttributesToResponse(const SDP_ServiceEntry_t* const ServiceEntry,
                                                  uint16_t AttributeList[][2],
                                                  const uint8_t TotalAttributes,
                                                  void** const BufferPos)
{
	uint16_t TotalResponseSize;

	/* Add an inner Data Element Sequence header for the current services's found Attributes */
	uint16_t* AttributeListSize = SDP_WriteSequenceHeader16(BufferPos);

	/* Search through the list of Attributes one at a time looking for values in the current UUID's Attribute table */
	for (uint8_t CurrAttribute = 0; CurrAttribute < TotalAttributes; CurrAttribute++)
	{
		uint16_t* AttributeIDRange = AttributeList[CurrAttribute];

		/* Look through the current service's attribute list, examining all the attributes */
		for (uint8_t i = 0; i < ServiceEntry->TotalTableAttributes; i++)
		{
			/* Get the current Attribute's ID from the current attribute table entry */
			uint16_t CurrAttributeID = pgm_read_word(&ServiceEntry->AttributeTable[i].AttributeID);

			/* Check if the current Attribute's ID is within the current Attribute range */
			if ((CurrAttributeID >= AttributeIDRange[0]) && (CurrAttributeID <= AttributeIDRange[1]))
			{
				void* CurrAttributeValue = pgm_read_ptr(&ServiceEntry->AttributeTable[i].Data);

				/* Increment the current UUID's returned Attribute container size by the number of added bytes */
				*AttributeListSize += SDP_AddAttributeToResponse(CurrAttributeID, CurrAttributeValue, BufferPos);
			}
		}
	}

	/* Record the total number of added bytes to the buffer */
	TotalResponseSize = 3 + *AttributeListSize;

	/* Fix endianness of the added attribute data element sequence */
	*AttributeListSize = cpu_to_be16(*AttributeListSize);

	return TotalResponseSize;
}

static void SDP_ServiceSearch(BT_StackConfig_t* const StackState,
                              BT_L2CAP_Channel_t* const Channel,
                              BT_SDP_PDUHeader_t* const SDPHeader)
{
	const void* CurrParameterPos = SDPHeader->Parameters;

	/* Read search UUID list sequence header and obtain its data size and a pointer to start of the data */
	uint8_t  UUIDList[12][UUID_SIZE_BYTES];
	uint8_t  TotalUUIDs          = SDP_GetUUIDList(UUIDList, &CurrParameterPos);
		
	/* Read maximum service count for the response */
	uint16_t MaximumServiceCount = SDP_ReadData16(&CurrParameterPos);

	/* Read continuation state value from the client */
	uint8_t  ContinuationState   = SDP_ReadData8(&CurrParameterPos);

	struct
	{
		BT_SDP_PDUHeader_t SDPHeader;
		uint16_t           TotalServiceRecordCount;
		uint16_t           CurrentServiceRecordCount;
		uint8_t            ResponseData[200];
	} ATTR_PACKED ResponsePacket;

	uint8_t AddedServiceHandles = 0;

	/* Create a pointer to the buffer to indicate the current location for response data to be added */
	void* CurrResponsePos = ResponsePacket.ResponseData;

	/* Search through all registered services, looking for UUID matches */
	SDP_ServiceEntry_t* CurrentService = RegisteredServices;
	while (CurrentService != NULL)
	{
		/* Ignore service nodes registered to other stacks */
		if (CurrentService->Stack && (CurrentService->Stack != StackState))
		  continue;
	
		/* Search the current SDP service attribute table for the given UUIDs */
		if (SDP_SearchServiceTable(CurrentService, UUIDList, TotalUUIDs))
		{
			/* Retrieve a PROGMEM pointer to the value of the service's record handle */
			const void* AttributeValue = SDP_GetAttributeValue(CurrentService, SDP_ATTRIBUTE_ID_SERVICERECORDHANDLE);

			/* Copy over the service record handle to the response list */
			uint8_t AttrHeaderSize;
			uint8_t AttrSize = SDP_GetLocalAttributeContainerSize(AttributeValue, &AttrHeaderSize);
			memcpy_P(CurrResponsePos, AttributeValue + AttrHeaderSize, AttrSize);
			CurrResponsePos += AttrHeaderSize + AttrSize;

			AddedServiceHandles++;

			if (AddedServiceHandles == MaximumServiceCount)
			  break;
		}
		
		/* Select next registered service in the service table list */	
		CurrentService = CurrentService->NextService;
	}

	/* Continuation state - always zero */
	SDP_WriteData8(&CurrResponsePos, 0);

	/* Set the total number of matching services and response services to the number of services added to the list */
	ResponsePacket.TotalServiceRecordCount   = cpu_to_be16(AddedServiceHandles);
	ResponsePacket.CurrentServiceRecordCount = ResponsePacket.TotalServiceRecordCount;

	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICEATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(sizeof(ResponsePacket.TotalServiceRecordCount) +
	                                                       sizeof(ResponsePacket.CurrentServiceRecordCount) +
	                                                       ((uintptr_t)CurrResponsePos - (uintptr_t)ResponsePacket.ResponseData));

	/* Send the completed response packet to the sender */
	SDP_SendFrame(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + be16_to_cpu(ResponsePacket.SDPHeader.ParameterLength)), &ResponsePacket);
}

static void SDP_ServiceAttribute(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel,
                                 BT_SDP_PDUHeader_t* const SDPHeader)
{
	const void* CurrParameterPos = SDPHeader->Parameters;

	/* Read service record handle */
	uint32_t RecordHandle         = SDP_ReadData32(&CurrParameterPos);

	/* Read maximum attribute size byte count for the response */
	uint16_t MaximumAttributeSize = SDP_ReadData16(&CurrParameterPos);
	
	/* Read search Attribute list sequence header and obtain its data size and a pointer to start of the data */
	uint16_t AttributeList[8][2];
	uint8_t  TotalAttributes      = SDP_GetAttributeList(AttributeList, &CurrParameterPos);
	
	/* Read continuation state value from the client */
	uint8_t  ContinuationState    = SDP_ReadData8(&CurrParameterPos);

	struct
	{
		BT_SDP_PDUHeader_t SDPHeader;
		uint16_t           AttributeListByteCount;
		uint8_t            ResponseData[200];
	} ATTR_PACKED ResponsePacket;

	/* Create a pointer to the buffer to indicate the current location for response data to be added */
	void* CurrResponsePos = ResponsePacket.ResponseData;

	/* Clamp the maximum attribute size to the size of the allocated buffer */
	if (MaximumAttributeSize > sizeof(ResponsePacket.ResponseData))
	  MaximumAttributeSize = sizeof(ResponsePacket.ResponseData);

	/* Search through all registered services, looking for UUID matches */
	SDP_ServiceEntry_t* CurrentService = RegisteredServices;
	while (CurrentService != NULL)
	{
		/* Retrieve a PROGMEM pointer to the value of the Service Record Handle */
		const void* ServiceRecord = SDP_GetAttributeValue(CurrentService, SDP_ATTRIBUTE_ID_SERVICERECORDHANDLE);

		/* Check if matching service record found in the table */
		if (ServiceRecord)
		{
			/* Get the size of the header for the Service Record Handle */
			uint8_t AttrHeaderSize;
			SDP_GetLocalAttributeContainerSize(ServiceRecord, &AttrHeaderSize);

			/* Retrieve the endian-swapped service handle of the current service being examined */
			uint32_t CurrServiceHandle = be32_to_cpu(pgm_read_dword(ServiceRecord + AttrHeaderSize));

			/* Check if the current service in the service table has the requested service handle */
			if (RecordHandle == CurrServiceHandle)
			{
				/* Add the listed attributes for the found UUID to the response */
				SDP_AddListedAttributesToResponse(CurrentService, AttributeList, TotalAttributes, &CurrResponsePos);

				/* Requested service found, abort the search through the service table */
				break;
			}
		}
		
		/* Select next registered service in the service table list */	
		CurrentService = CurrentService->NextService;
	}

	/* Continuation state - always zero */
	SDP_WriteData8(&CurrResponsePos, 0);

	/* Set the total response list size to the size of the outer container plus its header size and continuation state */
	ResponsePacket.AttributeListByteCount    = cpu_to_be16((uintptr_t)CurrResponsePos - (uintptr_t)ResponsePacket.ResponseData - sizeof(uint8_t));

	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICEATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(sizeof(ResponsePacket.AttributeListByteCount) + be16_to_cpu(ResponsePacket.AttributeListByteCount) + sizeof(uint8_t));

	/* Send the completed response packet to the sender */
	SDP_SendFrame(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + be16_to_cpu(ResponsePacket.SDPHeader.ParameterLength)), &ResponsePacket);
}

static void SDP_ServiceSearchAttribute(BT_StackConfig_t* const StackState,
                                       BT_L2CAP_Channel_t* const Channel,
                                       BT_SDP_PDUHeader_t* const SDPHeader)
{
	const void* CurrParameterPos = SDPHeader->Parameters;

	/* Read search UUID list sequence header and obtain its data size and a pointer to start of the data */
	uint8_t  UUIDList[12][UUID_SIZE_BYTES];
	uint8_t  TotalUUIDs           = SDP_GetUUIDList(UUIDList, &CurrParameterPos);

	/* Read maximum attribute size byte count for the response */
	uint16_t MaximumAttributeSize = SDP_ReadData16(&CurrParameterPos);
	
	/* Read search Attribute list sequence header and obtain its data size and a pointer to start of the data */
	uint16_t AttributeList[8][2];
	uint8_t  TotalAttributes      = SDP_GetAttributeList(AttributeList, &CurrParameterPos);
	
	/* Read continuation state value from the client */
	uint8_t  ContinuationState    = SDP_ReadData8(&CurrParameterPos);

	struct
	{
		BT_SDP_PDUHeader_t SDPHeader;
		uint16_t           AttributeListByteCount;
		uint8_t            ResponseData[200];
	} ATTR_PACKED ResponsePacket;

	void* CurrResponsePos = ResponsePacket.ResponseData;
	
	/* Clamp the maximum attribute size to the size of the allocated buffer */
	if (MaximumAttributeSize > sizeof(ResponsePacket.ResponseData))
	  MaximumAttributeSize = sizeof(ResponsePacket.ResponseData);

	/* Write out response sequence header */
	uint16_t* RespAttributeListSize = SDP_WriteSequenceHeader16(&CurrResponsePos);

	/* Search through all registered services, looking for UUID matches */
	SDP_ServiceEntry_t* CurrentService = RegisteredServices;
	while (CurrentService != NULL)
	{
		/* Search the current SDP service attribute table for the given UUIDs */
		if (SDP_SearchServiceTable(CurrentService, UUIDList, TotalUUIDs))
		{
			/* Add the listed attributes for the found UUID to the response */
			*RespAttributeListSize += SDP_AddListedAttributesToResponse(CurrentService, AttributeList, TotalAttributes, &CurrResponsePos);
		}
		
		/* Select next registered service in the service table list */	
		CurrentService = CurrentService->NextService;
	}

	*RespAttributeListSize = cpu_to_be16(*RespAttributeListSize);
	
	/* Write continuation state - always zero */
	SDP_WriteData8(&CurrResponsePos, 0);
	
	/* Set the total response list size to the size of the outer container plus its header size and continuation state */
	ResponsePacket.AttributeListByteCount    = cpu_to_be16((uintptr_t)CurrResponsePos - (uintptr_t)ResponsePacket.ResponseData - sizeof(uint8_t));

	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICESEARCHATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(sizeof(ResponsePacket.AttributeListByteCount) + be16_to_cpu(ResponsePacket.AttributeListByteCount) + sizeof(uint8_t));

	/* Send the completed response packet to the sender */
	SDP_SendFrame(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + be16_to_cpu(ResponsePacket.SDPHeader.ParameterLength)), &ResponsePacket);
}

void SDP_Init(BT_StackConfig_t* const StackState)
{
	/* Reset list of registered service attribute tables for the current stack */
	if (RegisteredServices != NULL)
	{
		SDP_ServiceEntry_t* PreviousService = NULL;
		SDP_ServiceEntry_t* CurrentService  = RegisteredServices;
		
		/* Search through the registered service list nodes */
		while (CurrentService != NULL)
		{
			/* Check if the current regisered service is registered to the stack being re-initialized */
			if (CurrentService->Stack == StackState)
			{
				/* Check if we are altering the root service node, special-case the removal */
				if (!(PreviousService))
				  RegisteredServices = CurrentService->NextService;
				else
				  PreviousService->NextService = CurrentService->NextService;
			}
			
			/* Save reference to previous node for node removal, transverse linked list to next registered service */
			PreviousService = CurrentService;
			CurrentService  = CurrentService->NextService;
		}
	}
}

void SDP_Manage(BT_StackConfig_t* const StackState)
{

}

void SDP_ChannelOpened(BT_StackConfig_t* const StackState,
                       BT_L2CAP_Channel_t* const Channel)
{
	
}

void SDP_ChannelClosed(BT_StackConfig_t* const StackState,
                       BT_L2CAP_Channel_t* const Channel)
{
	
}

void SDP_ProcessPacket(BT_StackConfig_t* const StackState,
                       BT_L2CAP_Channel_t* const Channel,
                       const uint16_t Length,
                       uint8_t* Data)
{
	/* Ensure correct channel PSM before processing the data */
	if (Channel->PSM != CHANNEL_PSM_SDP)
	  return;
	
	BT_SDP_PDUHeader_t* SDPHeader = (BT_SDP_PDUHeader_t*)Data;
	
	/* Dispatch to the correct processing routine for the given SDP packet type */
	switch (SDPHeader->PDU)
	{
		case SDP_PDU_SERVICESEARCHREQUEST:
			SDP_ServiceSearch(StackState, Channel, SDPHeader);
			break;
		case SDP_PDU_SERVICEATTRIBUTEREQUEST:
			SDP_ServiceAttribute(StackState, Channel, SDPHeader);
			break;
		case SDP_PDU_SERVICESEARCHATTRIBUTEREQUEST:
			SDP_ServiceSearchAttribute(StackState, Channel, SDPHeader);
			break;
	}
}

