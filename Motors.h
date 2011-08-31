/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.com
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _MOTORS_H_
#define _MOTORS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		#include <stdlib.h>
		
		#include <LUFA/Common/Common.h>
		
	/* Macros: */
		#define MAX_MOTOR_POWER   0x0200
	
	/* Enums: */
		typedef enum
		{
			MOTOR_CHANNEL_Left  = (1 << 0),
			MOTOR_CHANNEL_Right = (1 << 1),
			MOTOR_CHANNEL_All   = (MOTOR_CHANNEL_Left | MOTOR_CHANNEL_Right),
		} Motor_Channel_t;
	
	/* Function Prototypes: */
		void Motors_Init(void);
		void Motors_SetChannelSpeed(const uint8_t Channel,
		                            const int16_t Power);

#endif

