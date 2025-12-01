#include "SubmixVoice.h"
#include "AudioSystem.h"

SubmixVoice::SubmixVoice() {
}

Bool SubmixVoice::Init(IXAudio2* audio, UInt16 numChannels, UInt16 sampleRate) {
	assert(!m_Voice);

	m_NumChannels = numChannels;
	m_SampleRate = sampleRate;
	m_FormatHash = Singleton<AudioSystem>::GetInstance()->GenerateHash(numChannels, sampleRate);

	return SUCCEEDED(audio->CreateSubmixVoice(&m_Voice, m_NumChannels, m_SampleRate, 0, 0, nullptr, nullptr));
}