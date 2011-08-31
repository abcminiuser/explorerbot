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

#ifndef _BLUETOOTH_COMMON_H_
#define _BLUETOOTH_COMMON_H_

	/* Includes: */
		#include <LUFA/Common/Common.h>

	/* Defines: */
		/** Length of a bluetooth device address in bytes. */
		#define BT_BDADDR_LEN              6
		
		/** Maximum number of simultaneous device connections. */
		#define MAX_DEVICE_CONNECTIONS     2
		
		/** Maximum number of simultaneous ACL logical channels. */
		#define MAX_LOGICAL_CHANNELS       5

		/** Default maximum transmission unit size for a ACL channel packet. */
		#define DEFAULT_ACL_CHANNEL_MTU    1024

	/* Enums: */
		/** Enum for the possible device connection states. */
		enum BT_HCI_ConnectionStates_t
		{
			HCI_CONSTATE_Free                   = 0,
			HCI_CONSTATE_New                    = 1,
			HCI_CONSTATE_Connected              = 2,
		};

		/** Enum for the possible states for a Bluetooth ACL channel. */
		enum BT_ACL_ChannelStates_t
		{
			ACL_CHANSTATE_Closed                = 0, /**< Channel is closed and inactive. No data may be sent or received. */
			ACL_CHANSTATE_New                   = 1, 
			ACL_CHANSTATE_WaitConnect           = 2, /**< A connection request has been received, but a response has not been sent. */
			ACL_CHANSTATE_WaitConnectRsp        = 3, /**< A connection request has been sent, but a response has not been received. */
			ACL_CHANSTATE_Config_WaitConfig     = 4, /**< Channel has been connected, but not yet configured on either end. */
			ACL_CHANSTATE_Config_WaitSendConfig = 5, /**< Channel configuration has been received and accepted, but not yet sent. */
			ACL_CHANSTATE_Config_WaitReqResp    = 6, /**< Channel configuration has been sent but not responded to, and a configuration
			                                          *   request from the remote end has not yet been received.
			                                          */
			ACL_CHANSTATE_Config_WaitResp       = 7, /**< Channel configuration has been sent but not accepted, but a configuration request
			                                          *   from the remote end has been accepted.
			                                          */
			ACL_CHANSTATE_Config_WaitReq        = 8, /**< Channel configuration has been sent and accepted, but a configuration request
			                                          *   from the remote end has not yet been accepted.
			                                          */
			ACL_CHANSTATE_Open                  = 9, /**< Channel is open and ready to send or receive data */
			ACL_CHANSTATE_WaitDisconnect        = 10, /**< A disconnection request has been sent, but not yet acknowledged. */
		};

		/** Enum for the possible Bluetooth packet types. */
		enum BT_PacketType_t
		{
			BLUETOOTH_PACKET_HCICommand         = 0x01,
			BLUETOOTH_PACKET_ACLData            = 0x02,
			BLUETOOTH_PACKET_SynchData          = 0x03,
			BLUETOOTH_PACKET_HCIEvent           = 0x04,
		};

	/* Type Defines: */
		typedef struct
		{
			uint8_t  State;
			uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
			uint16_t Handle;
			uint8_t  LinkType;
			uint8_t  CurrentIdentifier;
		} BT_HCI_Connection_t;
	
		/** Type define for a Bluetooth ACL channel information structure. This structure contains all the relevant
		 *  information on an ACL channel for data transmission and reception by the stack.
		 */
		typedef struct
		{
			uint16_t ConnectionHandle; /** HCI connection handle for the channel. */
			uint8_t  State; /**< Current channel state, a value from the \ref BT_ChannelStates_t enum. */
			uint16_t LocalNumber; /**< Local channel number on the device. */
			uint16_t RemoteNumber; /**< Remote channel number on the connected device. */
			uint16_t PSM; /**< Protocol used on the channel. */
			uint16_t LocalMTU; /**< MTU of data sent from the connected device to the local device. */
			uint16_t RemoteMTU; /**< MTU of data sent from the local device to the connected device. */
		} BT_ACL_Channel_t;

		typedef struct
		{
			struct
			{
				uint32_t Class; /**< Class of the local device, a mask of DEVICE_CLASS_* masks. */
				char     PINCode[16]; /**< Pin code required to send or receive in order to authenticate with a remote device. */
				char*    Name; /**< Name of the local Bluetooth device, up to 248 characters. */
				bool     Discoverable;
				void*    PacketBuffer;
			} Config;
			
			struct
			{
				struct
				{
					uint8_t             State;
					bool                StateTransition;
					uint8_t             LocalBDADDR[BT_BDADDR_LEN];
					BT_HCI_Connection_t Connections[MAX_DEVICE_CONNECTIONS];
				} HCI;
				
				struct
				{
					BT_ACL_Channel_t    Channels[MAX_LOGICAL_CHANNELS];
				} ACL;
			} State;
		} BT_StackConfig_t;

	/* User Implemented Callback Functions: */
		/** User application callback, fired when the Bluetooth stack has prepared a packet ready for transport to the Bluetooth adapter.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in]      Type        Packet type, a value from the \ref BT_PacketType_t enum.
		 *  \param[in]      Length      Length of the packet to send, in bytes.
		 *  \param[in]      Data        Pointer to the start of the packet to send.
		 */
		void CALLBACK_Bluetooth_SendPacket(BT_StackConfig_t* const StackState,
		                                   const uint8_t Type,
		                                   const uint16_t Length);

		/** User application callback, fired when the Bluetooth stack has received a connection request from a remote device, and requires
		 *  a decision to be made to accept or reject the request.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Connection  Pointer to the incoming connection information.
		 *
		 *  \return Boolean \c true to accept the request, \c false to reject it.
		 */
		bool CALLBACK_Bluetooth_ConnectionRequest(BT_StackConfig_t* const StackState,
		                                          BT_HCI_Connection_t* const Connection);

		/** User application callback, fired when the Bluetooth stack has received a channel creation request from a remote device, and requires
		 *  a decision to be made to accept or reject the request.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Connection  Pointer to the device connection information.
		 *  \param[in, out] Channel     Pointer to the incoming channel information.
		 *
		 *  \return Boolean \c true to accept the channel request, \c false to reject it.
		 */
		bool CALLBACK_Bluetooth_ChannelRequest(BT_StackConfig_t* const StackState,
		                                       BT_HCI_Connection_t* const Connection,
		                                       BT_ACL_Channel_t* const Channel);
	
	/* User Implemented Callback Event Functions: */
		/** User application event callback, fired when the Bluetooth stack has completed a connection to a remote device.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Connection  Pointer to the established device connection information.
		 */
		void EVENT_Bluetooth_ConnectionComplete(BT_StackConfig_t* const StackState,
		                                        BT_HCI_Connection_t* const Connection);

		/** User application event callback, fired when the Bluetooth stack has completed a disconnection from a remote device.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Connection  Pointer to the closed device connection information.
		 */
		void EVENT_Bluetooth_DisconnectionComplete(BT_StackConfig_t* const StackState,
		                                           BT_HCI_Connection_t* const Connection);

		/** User application event callback, fired when the Bluetooth stack has sucessfully opened a logical ACL connection to
		 *  a remote device.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Channel     Pointer to the opened device connection information.
		 */
		void EVENT_Bluetooth_ChannelOpened(BT_StackConfig_t* const StackState,
		                                   BT_ACL_Channel_t* const Channel);

		/** User application event callback, fired when the Bluetooth stack has closed a logical ACL connection to a remote device.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Channel     Pointer to the closed device connection information.
		 */
		void EVENT_Bluetooth_ChannelClosed(BT_StackConfig_t* const StackState,
		                                   BT_ACL_Channel_t* const Channel);

		/** User application callback, fired when the Bluetooth stack has received data on an open connection.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Connection  Pointer to the HCI connection information.
		 *  \param[in, out] Channel     Pointer to the ACL channel information.
		 *  \param[in]      Length      Length of the ACL packet data.
		 *  \param[in]      Data        Pointer to the start of the ACL packet data.
		 */
		void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
		                                  BT_HCI_Connection_t* const Connection,
		                                  BT_ACL_Channel_t* const Channel,
		                                  uint16_t Length,
		                                  uint8_t* Data);
		
#endif

