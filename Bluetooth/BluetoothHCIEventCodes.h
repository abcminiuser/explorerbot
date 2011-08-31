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
 *  Bluetooth HCI event codes, used to indicate the class type and specific
 *  event from a HCI controller.
 */

#ifndef _HCI_EVENT_CODES_H_
#define _HCI_EVENT_CODES_H_

	/* Macros: */
		#define OGF_LINK_CONTROL                              (0x01 << 10)
		#define OGF_CTRLR_BASEBAND                            (0x03 << 10)
		#define OGF_CTRLR_INFORMATIONAL                       (0x04 << 10)

		#define OCF_LINK_CONTROL_INQUIRY                       0x0001
		#define OCF_LINK_CONTROL_INQUIRY_CANCEL                0x0002
		#define OCF_LINK_CONTROL_PERIODIC_INQUIRY              0x0003
		#define OCF_LINK_CONTROL_EXIT_PERIODIC_INQUIRY         0x0004
		#define OCF_LINK_CONTROL_CREATE_CONNECTION             0x0005
		#define OCF_LINK_CONTROL_DISCONNECT                    0x0006
		#define OCF_LINK_CONTROL_CREATE_CONNECTION_CANCEL      0x0008
		#define OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST     0x0009
		#define OCF_LINK_CONTROL_REJECT_CONNECTION_REQUEST     0x000A
		#define OCF_LINK_CONTROL_LINK_KEY_REQUEST_REPLY        0x000B
		#define OCF_LINK_CONTROL_LINK_KEY_REQUEST_NEG_REPLY    0x000C
		#define OCF_LINK_CONTROL_PIN_CODE_REQUEST_REPLY        0x000D
		#define OCF_LINK_CONTROL_PIN_CODE_REQUEST_NEG_REPLY    0x000E
		#define OCF_LINK_CONTROL_CHANGE_CONNECTION_PACKET_TYPE 0x000F
		#define OCF_LINK_CONTROL_REMOTE_NAME_REQUEST           0x0019
		#define OCF_CTRLR_BASEBAND_SET_EVENT_MASK              0x0001
		#define OCF_CTRLR_BASEBAND_RESET                       0x0003
		#define OCF_CTRLR_BASEBAND_WRITE_PIN_TYPE              0x000A
		#define OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME            0x0013
		#define OCF_CTRLR_BASEBAND_READ_LOCAL_NAME             0x0014
		#define OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE           0x001A
		#define OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE       0x0024
		#define OCF_CTRLR_BASEBAND_WRITE_SIMPLE_PAIRING_MODE   0x0056
		#define OCF_CTRLR_BASEBAND_WRITE_AUTHENTICATION_ENABLE 0x0020
		#define OCF_CTRLR_INFORMATIONAL_READBUFFERSIZE         0x0005
		#define OCF_CTRLR_INFORMATIONAL_READBDADDR             0x0009

		#define EVENT_COMMAND_STATUS                           0x0F
		#define EVENT_COMMAND_COMPLETE                         0x0E
		#define EVENT_CONNECTION_COMPLETE                      0x03
		#define EVENT_CONNECTION_REQUEST                       0x04
		#define EVENT_DISCONNECTION_COMPLETE                   0x05
		#define EVENT_REMOTE_NAME_REQUEST_COMPLETE             0x07
		#define EVENT_PIN_CODE_REQUEST                         0x16
		#define EVENT_LINK_KEY_REQUEST                         0x17

		#define ERROR_LIMITED_RESOURCES                        0x0D
		#define ERROR_UNACCEPTABLE_BDADDR                      0x0F
		
#endif
