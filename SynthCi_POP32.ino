/**
 * SynthCi_POP32 - Fivewayses (TM)
 *
 * ===============================
 *
 * MIT License
 * 
 * Copyright (C) Fivewayses (2026).
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files 
 * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 *  [*] The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 *  [*] THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 *      FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *      CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "audio/midi/Note.h"
#include "audio/midi/Synthesizer.h"
#include "audio/midi/MIDIData.h"
#include "example/all_instruments_test.h"

#define CheckError(status, ...) if (!(status)) {\
	oled.clear();\
	oled.text(0, 0, __VA_ARGS__);\
	exit(1);\
}

static Synthesizer synth;

void setup()
{
	asm(".global _printf_float");

	MIDIData midi(all_instruments_test, all_instruments_test_len);
	CheckError(
		midi.IsValid(),
		"MIDI parsing failed: %s", GetError().c_str()
	);

	CheckError(
		synth.Open(),
		"Could not initialize Synthesizer: %s", GetError().c_str()
	);
	
	synth.Play();
	
	synth.AddVoice(note_e::C);
	delay(250);
	
	synth.AddVoice(note_e::E);
	delay(250);
	
	synth.AddVoice(note_e::G);
	delay(250);
	
	synth.AddVoice(note_e::C, 5);
	delay(250);
	
	synth.RemoveVoice(4);
	delay(250);
	
	synth.RemoveVoice(3);
	delay(250);
	
	synth.RemoveVoice(2);
	delay(250);
	
	synth.RemoveVoice(1);
	delay(250);

	midi.Play(synth);
	
	synth.Close();
}

void loop() {
	
}
