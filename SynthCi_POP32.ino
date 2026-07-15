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
#include "example/note_c.h"

#define Log(...) oled.clear(); oled.text(0, 0, __VA_ARGS__);

#define CheckError(status, ...) if (!(status)) {\
	Log(__VA_ARGS__);\
	exit(1);\
}

static Synthesizer synth;

void setup()
{
	asm(".global _printf_float");

	const bool do_midi_test = false;
	const bool do_audio_sample_test = false;
	const bool do_time_logging = false;
	
	MIDIData midi(note_c, note_c_len);
	CheckError(
		midi.IsValid(),
		"MIDI parsing failed: %s", GetError().c_str()
	);
	
	CheckError(
		synth.Open(),
		"Could not initialize Synthesizer: %s\n", GetError().c_str()
	);
	
	synth.Play();
	
	Log("Playing concert A at 440Hz...\n");
	synth.AddVoice(note_e::A, 5);
	
	if (do_audio_sample_test) {
		Log("Audio sample testing is not supported\n");
		//Test();
	}
	
	if (do_time_logging) {
		for (int i = 1; i <= 20; ++i) {
			delay(100);
			Log("%dms\n", i * 100);
		}
	} else {
		delay(2000);
	}
	
	synth.RemoveVoice(1);
	delay(1000);
	
	if (do_midi_test) {
		Log("Playing MIDI...\n");
		midi.Play(synth);
	}
	
	synth.Close();
	
	Log("Finished!\n");
	
	exit(0);
}

void loop() {
	
}
