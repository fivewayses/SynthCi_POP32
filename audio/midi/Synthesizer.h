#pragma once

#include <cstddef>
#include <cstdint>
#include <POP32.h>
#include <HardwareTimer.h>
#include "Note.h"
#include "waveforms.h"
#include "error/error.h"

#define MAX_VOICES 256
#define SAMPLE_RATE 24000

struct voice_t {
	uint32_t phase_inc = 0;
	uint16_t phase = 0;
	bool is_active = false;
};

typedef enum {
	SQUARE = 1
} waveform_e;

static voice_t voices[MAX_VOICES];

static void AudioCallback(void)
{
	int16_t audio_sample = 0;
	int32_t num_active_voices = 0;
		
	for (int j = 0; j < MAX_VOICES; ++j) {
		voice_t &v = voices[j];
			
		if (v.is_active) {
			v.phase += v.phase_inc;
			audio_sample += TRIANGLE_SAMPLE.data[v.phase];
			++num_active_voices;
		}
	}
		
	TIM3->CCR1 = num_active_voices ? (uint8_t)(audio_sample / num_active_voices + 128) : 0;
}

class Synthesizer {
	
public:
	Synthesizer() {}
	
	~Synthesizer()
	{
		Close();
	}
	
	void Play()
	{
		tim.resume();
	}
	
	void Stop() {
		tim.pause();
	}
	
	bool Open() {
		if (is_open) {
			PushError("Audio device is already open");
			return false;
		}
		
		tim.setup(TIM3);
		tim.setOverflow(SAMPLE_RATE, HERTZ_FORMAT);
		is_open = true;
		
		return true;
	}
	
	bool Close() {
		if (!is_open) {
			PushError("Audio device is already closed");
			return false;
		}
			
		tim.~HardwareTimer();
		is_open = false;
		
		return true;
	}
	
	inline bool IsOpen() const {
		return is_open;
	}
	
	uint32_t AddVoice(float freq) {
		// Find free voice slot
		for (int i = 0; i < MAX_VOICES; ++i) {
			voice_t &v = voices[i];
			if (v.is_active) continue;
			
			v.phase_inc = (uint32_t)(freq * 65536.0f / (float)SAMPLE_RATE);
			v.phase = 0;
			v.is_active = true;
		
			return i + 1;
		}
		
		return 0;
	}
	
	uint32_t AddVoice(uint8_t midi_note) {
		return AddVoice(Note(midi_note));
	}
	
	uint32_t AddVoice(note_e type, int32_t octave = 4) {
		return AddVoice(Note(type, octave));
	}
	
	bool RemoveVoice(uint32_t index)
	{
		if (index <= 0 || index > MAX_VOICES) {
			PushError("Voice index (%d) out of range of 1 - %d", index, MAX_VOICES);
			return false;
		}
		
		voice_t &v = voices[index-1];
		if (!v.is_active) {
			PushError("Voice #%d is not active", index);
			return false;
		}
		
		v.is_active = false;
		
		return true;
	}

private:
	HardwareTimer tim{};
	
	bool is_open = false;
};
