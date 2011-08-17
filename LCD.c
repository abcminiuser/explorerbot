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

static void LCD_SetDataLines(const uint8_t Data)
{
	uint8_t NewData = 0;
				
	if (Data & 0x01)
	  NewData |= LCD_DATA4;
	  
	if (Data & 0x02)
	  NewData |= LCD_DATA5;

	if (Data & 0x04)
	  NewData |= LCD_DATA6;

	if (Data & 0x08)
	  NewData |= LCD_DATA7;

	PORTF = (PORTF & ~(LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4)) | NewData;
	
	Delay_MS(1);
	PORTE |= LCD_E;
	Delay_MS(1);
	PORTE &= ~LCD_E;
}
		
static uint8_t LCD_GetDataLines(void)
{
	uint8_t CurrDataLines;
	uint8_t NewData = 0;
	
	PORTE |= LCD_E;
	Delay_MS(1);
	PORTE &= ~LCD_E;
	Delay_MS(1);
	
	CurrDataLines = PINF;
				
	if (CurrDataLines & LCD_DATA4)
	  NewData |= 0x01;
	  
	if (CurrDataLines & LCD_DATA5)
	  NewData |= 0x02;
	
	if (CurrDataLines & LCD_DATA6)
	  NewData |= 0x04;

	if (CurrDataLines & LCD_DATA7)
	  NewData |= 0x08;

	return NewData;
}

void LCD_Init(void)
{
	DDRE |= (LCD_E | LCD_RS | LCD_RW);
	DDRF |= (LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4);
	DDRB |= (1 << 4);
	
	TCCR2A = ((1 << COM2A1) | (1 << WGM20));

	LCD_SetBacklight(0);

	PORTE &= ~LCD_RS;
	LCD_SetDataLines(0b0011);
	Delay_MS(5);
	LCD_SetDataLines(0b0011);
	Delay_MS(1);
	LCD_SetDataLines(0b0011);
	LCD_SetDataLines(0b0010);
	
	LCD_WriteByte(0x88);
	LCD_WriteByte(0x08);
	LCD_WriteByte(0x01);
	LCD_WriteByte(0x04);
	LCD_WriteByte(0x0C);
	PORTE |=  LCD_RS;
	
	LCD_Clear();
	LCD_SetCursor(1, 0);
}

void LCD_SetBacklight(const uint8_t Intensity)
{	
	if (!(Intensity))
	{
		TCCR2B = 0;
		PORTB &= ~(1 << 4);
	}
	else
	{
		OCR2A  = Intensity;
		TCCR2B = (1 << CS21);
	}
}

void LCD_Clear(void)
{
	PORTE &= ~LCD_RS;
	LCD_WriteByte(0x01);
	PORTE |=  LCD_RS;
}

void LCD_SetCursor(const uint8_t Y,
                   const uint8_t X)
{
	PORTE &= ~LCD_RS;
	LCD_WriteByte(0x80 | (((Y - 1) * 64) + X));
	PORTE |=  LCD_RS;
}

uint8_t LCD_ReadByte(void)
{
	uint8_t Byte = 0;

	PORTF &= ~(LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4);
	DDRF  &= ~(LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4);
	PORTE |= LCD_RW;

	Byte |= (LCD_GetDataLines() << 4);
	Byte |= LCD_GetDataLines();
	
	return Byte;
}

void LCD_WriteByte(const uint8_t Byte)
{
	PORTE &= ~LCD_RW;
	DDRF  |= LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4;
	
	LCD_SetDataLines(Byte >> 4);
	LCD_SetDataLines(Byte & 0x0F);
}

void LCD_WriteString(const char* String)
{
	while (*String != 0x00)
	{
		LCD_WriteByte(*String);
		String++;
	}
}

void LCD_WriteString_P(const char* String)
{
	uint8_t CurrByte;

	while ((CurrByte = pgm_read_byte(String)) != 0x00)
	{
		LCD_WriteByte(CurrByte);
		String++;
	}
}
