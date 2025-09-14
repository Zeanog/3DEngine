#include "SubmixVoice.h"

#include <xaudio2fx.h>

SubmixVoice::SubmixVoice() {
}

void SubmixVoice::Destroy() {
	::DestroyVoice(m_Voice);
}

Bool SubmixVoice::Init(IXAudio2* audio, UInt32 numChannels, UInt32 sampleRate) {
	m_NumChannels = numChannels;
	m_SampleRate = sampleRate;

	if (FAILED(audio->CreateSubmixVoice(&m_Voice, m_NumChannels, m_SampleRate, 0, 0, nullptr, nullptr))) {
		return false;
	}

	return true;
}

Float32 SubmixVoice::Volume() const {
	Float32 volume;
	m_Voice->GetVolume(&volume);
	return volume;
}

Bool SubmixVoice::Volume(Float32 newVolume) const {
	return SUCCEEDED(m_Voice->SetVolume(newVolume));
}

Bool SubmixVoice::Volume(Float32 newVolume, UInt32 operationSet) const {
	return SUCCEEDED(m_Voice->SetVolume(newVolume, operationSet));
}