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

	TCCR1A = (1 << WGM11);
	TCCR1B = ((1 << WGM13) | (1 << CS10));
	
	ICR1 = 0x0FFC;
	
	Motors_SetChannelSpeed(0, 0);
}

/** Sets the current speed of the robot motors.
 *
 *  \param[in] LeftPower   Motor power to set the left channel to, a value between 0 (off) and \ref MAX_MOTOR_POWER.
 *  \param[in] RightPower  Motor power to set the left channel to, a value between 0 (off) and \ref MAX_MOTOR_POWER.
 */
void Motors_SetChannelSpeed(const int16_t LeftPower, const int16_t RightPower)
{
	bool     LeftDirChange  = (LeftPower  <= 0) ? (PORTD & (1 << 4)) : !(PORTD & (1 << 4));
	bool     RightDirChange = (RightPower <= 0) ? (PORTD & (1 << 3)) : !(PORTD & (1 << 3));
	uint16_t LeftPWMValue   = abs(LeftPower);
	uint16_t RightPWMValue  = abs(RightPower);

	/* If motor channel off or direction switching, disable the PWM output completely */
	if (LeftDirChange || !(LeftPower))
	{
		TCCR1A &= ~(1 << COM1A1);
		PORTB  &= ~(1 << 6);	
	}
	
	/* If motor channel off or direction switching, disable the PWM output completely */
	if (RightDirChange || !(RightPower))
	{
		TCCR1A &= ~(1 << COM1B1);
		PORTB  &= ~(1 << 5);	
	}
	
	/* Delay on direction change to wait until slow inverter transistors have finished switching to prevent voltage rail dips */
	if (LeftDirChange || RightDirChange)
	  Delay_MS(10);
	
	/* DANGER: DO NOT REMOVE SPEED LIMITER BELOW - PREVENTS OVERCURRENT/OVERVOLTAGE OF MOTOR */
	if (LeftPWMValue > MAX_MOTOR_POWER)
	  LeftPWMValue = MAX_MOTOR_POWER;

	/* DANGER: DO NOT REMOVE SPEED LIMITER BELOW - PREVENTS OVERCURRENT/OVERVOLTAGE OF MOTOR */
	if (RightPWMValue > MAX_MOTOR_POWER)
	  RightPWMValue = MAX_MOTOR_POWER;

	/* Set direction pin for left motor channel */
	if (LeftPower <= 0)
	  PORTD &= ~(1 << 4);
	else
	  PORTD |=  (1 << 4);
		
	/* If channel power non-zero, re-activate the PWM output */
	if (LeftPower != 0)
	  TCCR1A |= (1 << COM1A1);

	OCR1A = LeftPWMValue;
	
	/* Set direction pin for right motor channel */
	if (RightPower <= 0)
	  PORTD &= ~(1 << 3);
	else
	  PORTD |=  (1 << 3);

	/* If channel power non-zero, re-activate the PWM output */
	if (RightPower != 0)
	  TCCR1A |= (1 << COM1B1);
	
	OCR1B = RightPWMValue;
}
