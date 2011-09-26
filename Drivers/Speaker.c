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

#include "Speaker.h"

const uint8_t Sequence_Connected[]     = {SPEAKER_SEQESCAPE, SPEAKER_ESCCMD_ToneDuration, 6,
                                          SPEAKER_HZ(349.23), 0, SPEAKER_HZ(440.0), 0,
                                          SPEAKER_SEQESCAPE, SPEAKER_ESCCMD_EndOfSequence};

const uint8_t Sequence_Disconnected[]  = {SPEAKER_SEQESCAPE, SPEAKER_ESCCMD_ToneDuration, 6,
                                          SPEAKER_HZ(440.0), 0, SPEAKER_HZ(349.23), 0,
                                          SPEAKER_SEQESCAPE, SPEAKER_ESCCMD_EndOfSequence};
										  
const uint8_t Sequence_LaCucaracha[]   = {SPEAKER_SEQESCAPE, SPEAKER_ESCCMD_ToneDuration, 5,
                                          SPEAKER_HZ(261.63), 0, SPEAKER_HZ(261.63), 0, SPEAKER_HZ(261.63), 0,
                                          SPEAKER_SEQESCAPE, SPEAKER_ESCCMD_ToneDuration, 10,
                                          SPEAKER_HZ(349.23), 0, SPEAKER_HZ(440.0), 0,
                                          SPEAKER_SEQESCAPE, SPEAKER_ESCCMD_EndOfSequence};

const uint8_t* Sequence_Table[]        = {Sequence_Connected, Sequence_Disconnected, Sequence_LaCucaracha};

static const uint8_t* SequencePosition = NULL;
static uint8_t SequenceTicksElapsed    = 0;
static uint8_t SequenceTickDuration    = 0;

/** Initializes the Speaker hardware driver ready for use. This must be called before any other
 *  functions in the Speaker hardware driver.
 */
void Speaker_Init(void)
{
	DDRB |= (1 << 7);

	TCCR0A = ((1 << COM0A0) | (1 << WGM01));

	Speaker_SetPWM(0);
}

void Speaker_NextSequenceTone(void)
{
	if (!(SequencePosition))
	  return;
	
	if (SequenceTicksElapsed++ >= SequenceTickDuration)
	{
		SequenceTicksElapsed = 0;
		
		uint8_t NextTone;
		
		while ((NextTone = *(SequencePosition++)) == SPEAKER_SEQESCAPE)
		{
			switch (*(SequencePosition++))
			{
				case SPEAKER_ESCCMD_EndOfSequence:
					SequencePosition = NULL;
					Speaker_SetPWM(0);
					return;
				case SPEAKER_ESCCMD_ToneDuration:
					SequenceTickDuration = *(SequencePosition++);
					break;
			}
		}
		
		Speaker_SetPWM(NextTone);
	}
}

void Speaker_PlaySequence(const uint8_t SequenceID)
{
	SequencePosition     = Sequence_Table[SequenceID];
	SequenceTicksElapsed = 0;
	SequenceTickDuration = 10;
}

void Speaker_Tone(const uint8_t PWMValue)
{
	if (!(SequencePosition))
	  Speaker_SetPWM(PWMValue);
}

