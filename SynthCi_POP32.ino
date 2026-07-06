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
