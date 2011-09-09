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

#include "SDP.h"

static ServiceEntry_t* RegisteredServices;

void SDP_RegisterService(ServiceEntry_t* const ServiceEntry)
{
	ServiceEntry_t* CurrentService = RegisteredServices;

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
}

void SDP_UnregisterService(ServiceEntry_t* const ServiceEntry)
{
	ServiceEntry_t* CurrentService = RegisteredServices;

	/* No service currently registered, abort */
	if (CurrentService == NULL)
	  return;

	/* Root registered service is the service to remove */
	if (CurrentService == ServiceEntry)
	{
		RegisteredServices = CurrentService->NextService;
		return;
	}

	/* Transverse service linked list to find the inserted node */
	while ((CurrentService->NextService != ServiceEntry) && CurrentService->NextService)
	  CurrentService = CurrentService->NextService;

	/* Remove service if found */
	if (CurrentService->NextService)
	  CurrentService->NextService = CurrentService->NextService->NextService;
}

void SDP_Init(BT_StackConfig_t* const StackState)
{
	RegisteredServices = NULL;
}

void SDP_Manage(BT_StackConfig_t* const StackState)
{

}

static void SDP_ServiceSearch(BT_StackConfig_t* const StackState,
                              BT_L2CAP_Channel_t* const Channel,
                              BT_SDP_PDUHeader_t* const SDPHeader)
{
	const void* CurrParameterPos = SDPHeader->Parameters;

	/* Read search UUID list sequence header and obtain its data size and a pointer to start of the data */
	uint16_t    ServiceUUIDListSize;
	const void* ServiceUUIDList     = SDP_ReadSequenceHeader(&CurrParameterPos, &ServiceUUIDListSize);
	
	/* Read maximum service count for the response */
	uint16_t    MaximumServiceCount = SDP_ReadData16(&CurrParameterPos);

	/* Read continuation state value from the client */
	uint8_t     ContinuationState   = SDP_ReadData8(&CurrParameterPos);

	struct
	{
		BT_SDP_PDUHeader_t SDPHeader;
		uint16_t           TotalServiceRecordCount;
		uint16_t           CurrentServiceRecordCount;
		uint8_t            ResponseData[100];
	} ResponsePacket;

	uint16_t ResponseSize    = 0;
	void*    CurrResponsePos = ResponsePacket.ResponseData;
	
	/* Write out response sequence header */
	uint16_t* RespServiceListSize = SDP_WriteSequenceHeader16(&CurrResponsePos);
	ResponseSize += 2;
	
	uint8_t MatchingServiceCount = 0;
	// TODO - PROCESS AND ADD RESPONSE DATA
	
	ResponseSize += be16_to_cpu(*RespServiceListSize);
	
	/* Write continuation state - always zero */
	SDP_WriteData8(&CurrResponsePos, 0);
	ResponseSize += 1;

	/* Set the total number of matching services and response services to the number of services added to the list */
	ResponsePacket.TotalServiceRecordCount   = cpu_to_be16(MatchingServiceCount);
	ResponsePacket.CurrentServiceRecordCount = cpu_to_be16(MatchingServiceCount);

	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICEATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(sizeof(ResponsePacket.TotalServiceRecordCount) +
	                                                       sizeof(ResponsePacket.CurrentServiceRecordCount) + ResponseSize);

	/* Send the completed response packet to the sender */
	Bluetooth_L2CAP_SendPacket(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + be16_to_cpu(ResponsePacket.SDPHeader.ParameterLength)), &ResponsePacket);
}

static void SDP_ServiceAttribute(BT_StackConfig_t* const StackState,
                                 BT_L2CAP_Channel_t* const Channel,
                                 BT_SDP_PDUHeader_t* const SDPHeader)
{
	const void* CurrParameterPos = SDPHeader->Parameters;

	/* Read service record handle */
	uint32_t    RecordHandle         = SDP_ReadData32(&CurrParameterPos);

	/* Read maximum attribute size byte count for the response */
	uint16_t    MaximumAttributeSize = SDP_ReadData16(&CurrParameterPos);
	
	/* Read search Attribute list sequence header and obtain its data size and a pointer to start of the data */
	uint16_t    AttributeListSize;
	const void* AttributeList        = SDP_ReadSequenceHeader(&CurrParameterPos, &AttributeListSize);
	
	/* Read continuation state value from the client */
	uint8_t     ContinuationState    = SDP_ReadData8(&CurrParameterPos);

	struct
	{
		BT_SDP_PDUHeader_t SDPHeader;
		uint16_t           AttributeListByteCount;
		uint8_t            ResponseData[100];
	} ResponsePacket;

	uint16_t ResponseSize    = 0;
	void*    CurrResponsePos = ResponsePacket.ResponseData;
	
	/* Write out response sequence header */
	uint16_t* RespAttributeListSize = SDP_WriteSequenceHeader16(&CurrResponsePos);
	ResponseSize += 2;
	
	// TODO - PROCESS AND ADD RESPONSE DATA
	
	ResponseSize += be16_to_cpu(*RespAttributeListSize);
	
	/* Write continuation state - always zero */
	SDP_WriteData8(&CurrResponsePos, 0);
	ResponseSize += 1;

	/* Set the total response list size to the size of the outer container plus its header size and continuation state */
	ResponsePacket.AttributeListByteCount    = cpu_to_be16(ResponseSize);

	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICEATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(sizeof(ResponsePacket.AttributeListByteCount) + ResponseSize);

	/* Send the completed response packet to the sender */
	Bluetooth_L2CAP_SendPacket(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + be16_to_cpu(ResponsePacket.SDPHeader.ParameterLength)), &ResponsePacket);
}

static void SDP_ServiceSearchAttribute(BT_StackConfig_t* const StackState,
                                       BT_L2CAP_Channel_t* const Channel,
                                       BT_SDP_PDUHeader_t* const SDPHeader)
{
	const void* CurrParameterPos = SDPHeader->Parameters;

	/* Read search UUID list sequence header and obtain its data size and a pointer to start of the data */
	uint16_t    ServiceUUIDListSize;
	const void* ServiceUUIDList      = SDP_ReadSequenceHeader(&CurrParameterPos, &ServiceUUIDListSize);

	/* Read maximum attribute size byte count for the response */
	uint16_t    MaximumAttributeSize = SDP_ReadData16(&CurrParameterPos);
	
	/* Read search Attribute list sequence header and obtain its data size and a pointer to start of the data */
	uint16_t    AttributeListSize;
	const void* AttributeList        = SDP_ReadSequenceHeader(&CurrParameterPos, &AttributeListSize);
	
	/* Read continuation state value from the client */
	uint8_t     ContinuationState    = SDP_ReadData8(&CurrParameterPos);

	struct
	{
		BT_SDP_PDUHeader_t SDPHeader;
		uint16_t           AttributeListByteCount;
		uint8_t            ResponseData[100];
	} ResponsePacket;

	uint16_t ResponseSize    = 0;
	void*    CurrResponsePos = ResponsePacket.ResponseData;
	
	/* Write out response sequence header */
	uint16_t* RespAttributeListSize = SDP_WriteSequenceHeader16(&CurrResponsePos);
	ResponseSize += 2;
	
	// TODO - PROCESS AND ADD RESPONSE DATA
	
	ResponseSize += be16_to_cpu(*RespAttributeListSize);
	
	/* Write continuation state - always zero */
	SDP_WriteData8(&CurrResponsePos, 0);
	ResponseSize += 1;

	/* Set the total response list size to the size of the outer container plus its header size and continuation state */
	ResponsePacket.AttributeListByteCount    = cpu_to_be16(ResponseSize);

	/* Fill in the response packet's header */
	ResponsePacket.SDPHeader.PDU             = SDP_PDU_SERVICESEARCHATTRIBUTERESPONSE;
	ResponsePacket.SDPHeader.TransactionID   = SDPHeader->TransactionID;
	ResponsePacket.SDPHeader.ParameterLength = cpu_to_be16(sizeof(ResponsePacket.AttributeListByteCount) + ResponseSize);

	/* Send the completed response packet to the sender */
	Bluetooth_L2CAP_SendPacket(StackState, Channel, (sizeof(ResponsePacket.SDPHeader) + be16_to_cpu(ResponsePacket.SDPHeader.ParameterLength)), &ResponsePacket);
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
                       uint16_t Length,
                       uint8_t* Data)
{
	BT_SDP_PDUHeader_t* SDPHeader = (BT_SDP_PDUHeader_t*)Data;
	
	/* Ensure correct channel PSM before processing the data */
	if (Channel->PSM != CHANNEL_PSM_SDP)
	  return;
	
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

