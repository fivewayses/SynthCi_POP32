#include "audio/midi/Note.h"
#include "audio/midi/Synthesizer.h"
#include "audio/midi/MIDIData.h"

#define CheckError(status, ...) if (!(status)) {\
	oled.clear();\
	oled.text(0, 0, __VA_ARGS__);\
	exit(1);\
}

static Synthesizer synth;

void setup()
{
	CheckError(
		synth.Open(),
		"Could not initialize Synthesizer: %s\n", "No error"
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
	
	synth.Close();
}

void loop() {
	
}
