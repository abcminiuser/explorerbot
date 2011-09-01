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

#include "Motors.h"

/** Initializes the Motors hardware driver ready for use. This must be called before any other
 *  functions in the Motors hardware driver.
 */
void Motors_Init(void)
{
	DDRD  |= ((1 << 3) | (1 << 4));
	DDRB  |= ((1 << 5) | (1 << 6));

	TCCR1A = ((1 << WGM11) | (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1));
	TCCR1B = (1 << CS11);
	
	Motors_SetChannelSpeed(MOTOR_CHANNEL_All, 0);
}

/** Sets the current speed of the robot motors.
 *
 *  \param[in] Channel  Motor channel whose speed is to be altered, a value from the \ref Motor_Channel_t enum.
 *  \param[in] Power    Motor power to set the given channel(s) to, a value between 0 (off) and \ref MAX_MOTOR_POWER.
 */
void Motors_SetChannelSpeed(const uint8_t Channel, const int16_t Power)
{
	uint16_t MotorPWMValue = abs(Power);
	
	/* DANGER: DO NOT REMOVE SPEED LIMITER BELOW - PREVENTS OVERCURRENT/OVERVOLTAGE OF MOTORS */
	if (MotorPWMValue > MAX_MOTOR_POWER)
	  MotorPWMValue = MAX_MOTOR_POWER;

	if (Channel & MOTOR_CHANNEL_Left)
	{
		if (Power <= 0)
		  PORTD &= ~(1 << 3);
		else
		  PORTD |=  (1 << 3);
		
		if (Power == 0)
		{
			TCCR1A &= ~(1 << COM1B1);
			PORTB  &= ~(1 << 5);
		}
		else
		{
			TCCR1A |=  (1 << COM1B1);
		}

		OCR1B = MotorPWMValue;
	}
	
	if (Channel & MOTOR_CHANNEL_Right)
	{
		if (Power <= 0)
		  PORTD &= ~(1 << 4);
		else
		  PORTD |=  (1 << 4);

		if (Power == 0)
		{
			TCCR1A &= ~(1 << COM1A1);
			PORTB  &= ~(1 << 6);
		}
		else
		{
			TCCR1A |=  (1 << COM1A1);
		}
	
		OCR1A = MotorPWMValue;
	}
}
