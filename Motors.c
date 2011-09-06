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
	
	Motors_SetChannelSpeed(0, 0);
}

/** Sets the current speed of the robot motors.
 *
 *  \param[in] LeftPower   Motor power to set the left channel to, a value between 0 (off) and \ref MAX_MOTOR_POWER.
 *  \param[in] RightPower  Motor power to set the left channel to, a value between 0 (off) and \ref MAX_MOTOR_POWER.
 */
void Motors_SetChannelSpeed(const int16_t LeftPower, const int16_t RightPower)
{
	uint16_t LeftPWMValue  = abs(LeftPower);
	uint16_t RightPWMValue = abs(RightPower);
	
	/* DANGER: DO NOT REMOVE SPEED LIMITERS BELOW - PREVENTS OVERCURRENT/OVERVOLTAGE OF MOTORS */
	if (LeftPWMValue > MAX_MOTOR_POWER)
	  LeftPWMValue = MAX_MOTOR_POWER;

	if (RightPWMValue > MAX_MOTOR_POWER)
	  RightPWMValue = MAX_MOTOR_POWER;

	if (LeftPower <= 0)
	  PORTD &= ~(1 << 3);
	else
	  PORTD |=  (1 << 3);
		
	if (LeftPower == 0)
	{
		TCCR1A &= ~(1 << COM1B1);
		PORTB  &= ~(1 << 5);
	}
	else
	{
		TCCR1A |=  (1 << COM1B1);
	}

	OCR1B = LeftPWMValue;
	
	if (RightPower <= 0)
	  PORTD &= ~(1 << 4);
	else
	  PORTD |=  (1 << 4);

	if (RightPower == 0)
	{
		TCCR1A &= ~(1 << COM1A1);
		PORTB  &= ~(1 << 6);
	}
	else
	{
		TCCR1A |=  (1 << COM1A1);
	}
	
	OCR1A = RightPWMValue;
}
