#include "SubmixVoice.h"

#include <xaudio2fx.h>

SubmixVoice::SubmixVoice() {
}

Bool SubmixVoice::Init(IXAudio2* audio, UInt32 numChannels, UInt32 sampleRate) {
	assert(!m_Voice);

	m_NumChannels = numChannels;
	m_SampleRate = sampleRate;

	return SUCCEEDED(audio->CreateSubmixVoice(&m_Voice, m_NumChannels, m_SampleRate, 0, 0, nullptr, nullptr));
}