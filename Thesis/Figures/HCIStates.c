enum BT_HCIStates_t
{
	HCISTATE_Init_Reset          = 0,
	HCISTATE_Init_ReadBufferSize = 1,
	HCISTATE_Init_GetBDADDR      = 2,
	HCISTATE_Init_SetLocalName   = 3,
	HCISTATE_Init_SetDeviceClass = 4,
	HCISTATE_Init_SetScanEnable  = 5,
	HCISTATE_Idle                = 6,
};