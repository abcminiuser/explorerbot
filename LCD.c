/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.org
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

#include "LCD.h"

void LCD_Init(void)
{
	DDRE |= LCD_E | LCD_RS | LCD_RW;
	DDRF |= ((1 << 3) | (1 << 2) | (1 << 1) | (1 << 0));
	DDRB |= (1 << 4);
	
	TCCR2A = ((1 << COM2A1) | (1 << COM2A0) | (1 << WGM21) | (1 << WGM20));

	LCD_SetBacklight(0);
	
	// TODO
}

void LCD_Clear(void)
{
	// TODO
}

void LCD_SetBacklight(const uint8_t Intensity)
{
	OCR2A = Intensity;
	
	if (!(Intensity))
	{
		TCCR2B = 0;
		PORTB &= ~(1 << 4);
	}
	else
	{
		TCCR2B = (1 << CS20);	
	}
}

void LCD_SetCursor(const uint8_t Y,
                   const uint8_t X)
{
	// TODO
}

void LCD_WriteChar(const char Character)
{
	// TODO
}

void LCD_WriteString(const char* String)
{
	while (*String != 0x00)
	{
		LCD_WriteChar(*String);
		String++;
	}
}

void LCD_WriteString_P(const char* String)
{
	uint8_t CurrByte;

	while ((CurrByte = pgm_read_byte(String)) != 0x00)
	{
		LCD_WriteChar(CurrByte);
		String++;
	}
}
