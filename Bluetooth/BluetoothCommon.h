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
 *  Common definitions for the Bluetooth stack.
 */

#ifndef _BLUETOOTH_COMMON_H_
#define _BLUETOOTH_COMMON_H_

	/* Includes: */
		#include <LUFA/Common/Common.h>
		
		#include "Drivers/LCD.h"

	/* Defines: */
		/** Maximum number of simultaneous device connections (user-configurable). */
		#define BT_MAX_DEVICE_CONNECTIONS         3
		
		/** Maximum number of simultaneous L2CAP logical channels (user-configurable). */
		#define BT_MAX_LOGICAL_CHANNELS           (BT_MAX_DEVICE_CONNECTIONS * 8)
		
		/** Maximum number of queued L2CAP signalling commands (user-configurable). */
		#define BT_MAX_QUEUED_L2CAP_EVENTS        10

		/** Default maximum transmission unit size for a L2CAP channel packet (user-configurable). */
		#define BT_DEFAULT_L2CAP_CHANNEL_MTU      1024
		
		/** Tick length interval that \ref Bluetooth_TickElapsed() is called at (user-configurable). */
		#define BT_TICK_MS                        SYSTEM_TICK_MS

	/* Enums: */
		/** Enum for the possible L2CAP channel Protocol Service Multiplexer (PSM) values. */
		enum BT_ChannelPSM_t
		{
			CHANNEL_PSM_SDP                       = 0x0001, /**< Bluetooth channel PSM for Service Discovery Protocol. */
			CHANNEL_PSM_UDP                       = 0x0002, /**< Bluetooth channel PSM for UDP. */
			CHANNEL_PSM_RFCOMM                    = 0x0003, /**< Bluetooth channel PSM for RFCOMM. */
			CHANNEL_PSM_TCP                       = 0x0004, /**< Bluetooth channel PSM for TCP. */
			CHANNEL_PSM_IP                        = 0x0009, /**< Bluetooth channel PSM for IP. */
			CHANNEL_PSM_FTP                       = 0x000A, /**< Bluetooth channel PSM for FTP. */
			CHANNEL_PSM_HTTP                      = 0x000C, /**< Bluetooth channel PSM for HTTP. */
			CHANNEL_PSM_UPNP                      = 0x0010, /**< Bluetooth channel PSM for UPNP. */
			CHANNEL_PSM_HIDCTL                    = 0x0011, /**< Bluetooth channel PSM for HID Control. */
			CHANNEL_PSM_HIDINT                    = 0x0013, /**< Bluetooth channel PSM for HID Data. */
		};

		/** Enum for the possible Bluetooth packet types. */
		enum BT_PacketType_t
		{
			BLUETOOTH_PACKET_HCICommand           = 0x01, /**< Bluetooth packet is a HCI Command Packet. */
			BLUETOOTH_PACKET_HCIData              = 0x02, /**< Bluetooth packet is a HCI Data Packet. */
			BLUETOOTH_PACKET_HCISynchData         = 0x03, /**< Bluetooth packet is a HCI Synchronous Data Packet. */
			BLUETOOTH_PACKET_HCIEvent             = 0x04, /**< Bluetooth packet is a HCI Event Packet. */
		};
		
		/** Enum for the possible Bluetooth link types. */
		enum BT_LinkTypes_t
		{
			LINK_TYPE_SCO                         = 0x00, /**< Bluetooth link type for a SCO link. */
			LINK_TYPE_ACL                         = 0x01, /**< Bluetooth link type for an ACL link. */
			LINK_TYPE_eSCO                        = 0x02, /**< Bluetooth link type for an Extended SCO link. */
		};

		/** Enum for the possible internal L2CAP Bluetooth event codes. */
		enum BT_L2CAP_Events_t
		{
			L2CAP_EVENT_OpenChannelReq            = 0,  /**< User-initiated channel open event. */
			L2CAP_EVENT_CloseChannelReq           = 1,  /**< User-initiated channel close event. */
			L2CAP_EVENT_SendConfigReq             = 2,  /**< Send local channel configuration event. */
			L2CAP_EVENT_ConnectReq                = 3,  /**< Remote channel connection event. */
			L2CAP_EVENT_ConnectRsp                = 4,  /**< Remote channel connection response event. */
			L2CAP_EVENT_ConfigReq                 = 5,  /**< Remote channel configuration event. */
			L2CAP_EVENT_ConfigRsp                 = 6,  /**< Remote channel configuration response event. */
			L2CAP_EVENT_DisconnectReq             = 7,  /**< Remote channel disconnection event. */
			L2CAP_EVENT_DisconnectRsp             = 8,  /**< Remote channel disconnection response event. */
			L2CAP_EVENT_CommandRej                = 9,  /**< L2CAP signalling command reject event. */
			L2CAP_EVENT_EchoReq                   = 10, /**< L2CAP echo event. */
			L2CAP_EVENT_InformationReq            = 11, /**< L2CAP information request event. */
		};

	/* Type Defines: */
		/** Type define for a Bluetooth Device Address.
		 *
		 *  \note This is an array type, and thus is not passed by value between functions.
		 */
		typedef uint8_t BDADDR_t[6];
		
		/** Type define for a queued L2CAP event. As the L2AP layer receives commands from remote devices and from the 
		 *  user application, events are generated and queued for later processing. This queue allows for events to be
		 *  deferred until such time that there is adequate space in the data packet buffers, to prevent buffer overruns
		 *  or lost command packets.
		 */
		typedef struct
		{
			uint8_t  Event; /**< Event code for the given L2AP event. */
			uint16_t ConnectionHandle; /**< HCI Connection handle for the given event's response (if any). */
			uint8_t  Identifier; /**< L2CAP signalling identifier for the given event's response (if any). */
			uint16_t SourceChannel; /**< Source channel of the event's initiator (if any). */
			uint16_t DestinationChannel; /**< Destination channel of the event's initiator (if any). */
			uint8_t  Result; /**< Result of the event (if any). */
		} BT_L2CAP_Event_t;

		/** Type define for a Bluetooth HCI connection information structure.  This structure contains all the relevant
		 *  information on a HCI channel for data transmission and reception by the stack.
		 */
		typedef struct
		{
			uint8_t  State; /**< Current connection state, a value from the \ref BT_HCIStates_t enum. */
			BDADDR_t RemoteBDADDR; /**< Remote Bluetooth device's BDADDR for the connection. */
			uint16_t DataPacketsQueued; /**< Number of data packets queued by the controller for this connection. */
			uint16_t Handle; /**< Bluetooth HCI layer connection handle in the adapter for the connection. */
			uint8_t  LinkType; /**< Link type of the connection, a value from the \ref BT_LinkTypes_t enum. */
			uint8_t  CurrentIdentifier; /**< Current Bluetooth HCI signal layer identifier index. */
			bool     LocallyInitiated; /**< Boolean true if the connection was initiated by the local device, false if remotely initiated. */
		} BT_HCI_Connection_t;
	
		/** Type define for a Bluetooth L2CAP channel information structure. This structure contains all the relevant
		 *  information on an L2CAP channel for data transmission and reception by the stack.
		 */
		typedef struct
		{
			uint16_t ConnectionHandle; /**< HCI connection handle for the channel. */
			uint8_t  State; /**< Current channel state, a value from the \ref BT_L2CAP_ChannelStates_t enum. */
			uint16_t LocalNumber; /**< Local channel number on the device. */
			uint16_t RemoteNumber; /**< Remote channel number on the connected device. */
			uint16_t PSM; /**< Protocol used on the channel. */
			uint16_t LocalMTU; /**< MTU of data sent from the connected device to the local device. */
			uint16_t RemoteMTU; /**< MTU of data sent from the local device to the connected device. */
			bool     LocallyInitiated; /**< Boolean true if the channel was initiated by the local device, false if remotely initiated. */
		} BT_L2CAP_Channel_t;

		/** Type define for a Bluetooth Stack instance, used to contain the complete stack state for a Bluetooth adapter. */
		typedef struct
		{
			struct
			{
				uint32_t Class; /**< Class of the local device, a mask of DEVICE_CLASS_* masks. */
				char     PINCode[16]; /**< Pin code required to send or receive in order to authenticate with a remote device. */
				char*    Name; /**< Name of the local Bluetooth device, up to 248 characters. */
				bool     Discoverable; /**< Indicates if the stack should be discoverable to other devices during an Inquiry. */
				void*    PacketBuffer; /**< Pointer to a location where packets are to be stored and retrieved from the physical adapter. */
			} Config; /**< Stack configuration. This must be set by the user application before the stack is initialized via \ref Bluetooth_Init(). */
			
			struct
			{
				struct
				{
					uint8_t             State; /**< Current HCI state machine state, a value from the \ref BT_HCIStates_t enum. */
					bool                StateTransition; /**< Indicates if the state machine has undergone a state transition that needs to be processed. */
					uint8_t             CommandPackets; /**< Number of allowable HCI command packets which can be sent to the controller. */
					uint16_t            ACLDataPackets; /**< Number of allowable ACL data packets which can be sent to the controller. */
					uint16_t            TicksElapsed; /**< Number of ticks that has elapsed since the last command was sent. */
					BDADDR_t            LocalBDADDR; /**< Address of the local Bluetooth adapter attached to this stack instance. */
					BT_HCI_Connection_t Connections[BT_MAX_DEVICE_CONNECTIONS]; /**< HCI connection state information list. */
				} HCI; /**< HCI layer connection state information. */
				
				struct
				{
					uint16_t            LastAllocatedChannel; /**< Index of the last allocated L2CAP channel. */
					BT_L2CAP_Channel_t  Channels[BT_MAX_LOGICAL_CHANNELS]; /**< L2CAP channel state information list. */
					uint8_t             PendingEvents;
					BT_L2CAP_Event_t    Events[BT_MAX_QUEUED_L2CAP_EVENTS];
				} L2CAP; /**< L2CAP layer channel state information. */
			} State; /**< Stack state information, managed by the Bluetooth stack internally. */
		} BT_StackConfig_t;

	/* User Implemented Callback Functions: */
		/** User application callback, fired when the Bluetooth stack has prepared a packet ready for transport to the Bluetooth adapter.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in]      Type        Packet type, a value from the \ref BT_PacketType_t enum.
		 *  \param[in]      Length      Length of the packet to send, in bytes.
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
		                                       BT_L2CAP_Channel_t* const Channel);
	
	/* User Implemented Callback Event Functions: */
		/** User application event callback, fired when the Bluetooth stack initialization has completed.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 */
		void EVENT_Bluetooth_InitComplete(BT_StackConfig_t* const StackState);

		/** User application event callback, fired when the Bluetooth stack initialization process has started, so that the user services may be initialized.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 */
		void EVENT_Bluetooth_InitServices(BT_StackConfig_t* const StackState);

		/** User application event callback, fired when the Bluetooth stack management routine has been run, and the user services may be managed.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 */
		void EVENT_Bluetooth_ManageServices(BT_StackConfig_t* const StackState);

		/** User application event callback, fired when the Bluetooth stack has changed the state of a connection to a remote device.
		 *
		 *  \note This event fires only when a connection has opened, closed, or failed to open.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Connection  Pointer to the established device connection information.
		 */
		void EVENT_Bluetooth_ConnectionStateChange(BT_StackConfig_t* const StackState,
                                                   BT_HCI_Connection_t* const Connection);

		/** User application event callback, fired when the Bluetooth stack has change the state of a logical L2CAP connection to
		 *  a remote device.
		 *
		 *  \note This event fires only when a channel has opened, closed, or failed to open.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Channel     Pointer to the opened device connection information.
		 */
		void EVENT_Bluetooth_ChannelStateChange(BT_StackConfig_t* const StackState,
                                                BT_L2CAP_Channel_t* const Channel);

		/** User application callback, fired when the Bluetooth stack has received data on an open connection.
		 *
		 *  \param[in, out] StackState  Pointer to a Bluetooth Stack state table.
		 *  \param[in, out] Channel     Pointer to the L2CAP channel information.
		 *  \param[in]      Length      Length of the L2CAP packet data.
		 *  \param[in]      Data        Pointer to the start of the L2CAP packet data.
		 */
		void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
		                                  BT_L2CAP_Channel_t* const Channel,
		                                  uint16_t Length,
		                                  uint8_t* Data);
		
#endif

