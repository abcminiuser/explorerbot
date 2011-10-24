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

/** \file
 *
 *  Header file for Motors.c.
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
		/** Sets the maximum motor PWM value which may be set. Motor power values above this absolute value will
		 *  be clamped to this value instead.
		 */
		#define MAX_MOTOR_POWER   0x0EEE
	
	/* Function Prototypes: */
		void Motors_Init(void);
		void Motors_SetChannelSpeed(const int16_t LeftPower, const int16_t RightPower);

#endif

