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

/* The Audio Callback Which Generates Sound */
static void AudioCallback(void *userdata, uint8_t *stream, int len) {
	for (int i = 0; i < len; ++i) {
		int16_t audio_sample = 0;
		int32_t num_active_voices = 0;
		
		// The envelope calculation will run every
		// ITERS_PER_ENVELOPE_CALC iterations.
		if (envelope_phase++ % ITERS_PER_ENVELOPE_CALC == 0) {
			sound_envelope_t env = global_envelope;
			
			for (int j = 0; j < MAX_VOICES; ++j) {
				// Copy the voice to the stack to prevent
				// race condition.
				voice_t v = voices[j];

				// If the voice is not active, check first
				// whether time is negative, representing
				// the release envelope.
				if (!v.is_active && v.time_lticks >= 0) continue;
				
				int32_t t = env.attack;
				int64_t time_ms = v.time_lticks / LTICKS_PER_MS;
				
				if (v.time_lticks < 0) {
					// Releasing: sound fades out
					voices[j].envelope = env.sustain * -time_ms / env.release;
				} else if (time_ms < t) {
					// Attacking: sound increases sharply
					voices[j].envelope = 256 * time_ms / env.attack;
				} else {
					t += env.decay;
					if (time_ms < t) {
						// Decaying: sound levels out
						voices[j].envelope = env.sustain + (t - time_ms) * (256 - env.sustain) / env.decay;
					} else {
						// Sustaining: sound stays the same
						voices[j].envelope = env.sustain;
					}
				}
			}
		}
		
		for (int j = 0; j < MAX_VOICES; ++j) {
			voice_t &v = voices[j];
			
			// If the voice is not active, check first
			// whether time is negative, representing
			// the release envelope.
			if (!v.is_active && v.time_lticks >= 0) continue;
			
			v.time_lticks += SAMPLE_TIME_INC;
			v.phase += v.phase_inc;
			
			uint8_t vel = VELOCITY_TABLE.data[v.envelope];
			int32_t wsample = waveform_sample_table[v.phase / WAVEFORM_SAMPLE_DIV];
			audio_sample += (int16_t)(wsample * vel / 256);
			
			++num_active_voices;
		}
		
		stream[i] = num_active_voices ? (uint8_t)(audio_sample / num_active_voices + 128) : 128;
	}
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
		tim.attachInterrupt([] {
			uint8_t sample;
			AudioCallback(NULL, &sample, 1);

			TIM3->CCR1 = sample;
		})
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

	uint8_t *TestAudio(uint32_t sample_size = 1024)
	{
		uint8_t *sample = new uint8_t[sample_size];
		AudioCallback(NULL, sample, sample_size);
		
		return sample;
	}

private:
	HardwareTimer tim{};
	
	bool is_open = false;
};
