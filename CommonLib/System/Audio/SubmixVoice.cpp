#include "SubmixVoice.h"

#include <xaudio2fx.h>

SubmixVoice::SubmixVoice() {
}

Bool SubmixVoice::Init(IXAudio2* audio, UInt32 numChannels, UInt32 sampleRate) {
	m_NumChannels = numChannels;
	m_SampleRate = sampleRate;

	if (FAILED(audio->CreateSubmixVoice(&m_Voice, m_NumChannels, m_SampleRate, 0, 0, nullptr, nullptr))) {
		return false;
	}

	return true;
}