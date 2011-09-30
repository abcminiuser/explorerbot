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

#ifndef _SPEAKER_H_
#define _SPEAKER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdbool.h>
		#include <stddef.h>
		
		#include <LUFA/Common/Common.h>
	
	/* Macros: */
		#define SPEAKER_SEQESCAPE    0xFF
		
		#define SPEAKER_HZ(Freq)     (uint8_t)((F_CPU / 1024) / Freq)
	
	/* Enums: */
		enum Speaker_EscapeValues_t
		{
			SPEAKER_ESCCMD_EndOfSequence = 0,
			SPEAKER_ESCCMD_ToneDuration  = 1,
		};
		
		enum Speaker_SequenceIDs_t
		{
			SPEAKER_SEQUENCE_Connecting    = 0,
			SPEAKER_SEQUENCE_Connected     = 1,
			SPEAKER_SEQUENCE_Disconnected  = 2,
			SPEAKER_SEQUENCE_ConnectFailed = 3,
			SPEAKER_SEQUENCE_LaCucaracha   = 4,
		};
		
	/* Inline Functions: */
		/** Sets the tone generated from the speaker. This tone value does not have a defined frequency correspondance.
		 *
		 *  \param[in] PWMValue  PWM value to load into the speaker PWM timer (8-bit).
		 */
		static inline void Speaker_SetPWM(const uint8_t PWMValue)
		{
			if (PWMValue)
			{
				DDRB  |= (1 << 7);
				OCR0A  = PWMValue;
				TCCR0B = ((1 << CS02) | (1 << CS00));
			}
			else
			{
				TCCR0B = 0;
				DDRB  &= ~(1 << 7);
			}
		}

	/* Function Prototypes: */
		void Speaker_Init(void);
		void Speaker_Tone(const uint8_t PWMValue);
		void Speaker_PlaySequence(const uint8_t SequenceID);
		void Speaker_NextSequenceTone(void);

#endif

