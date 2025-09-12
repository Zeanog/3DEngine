#include "SourceVoice.h"
#include "Sound.h"
#include "SourceVoiceCallbacks.h"

SourceVoice::SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format) {
	m_Format = format;

	m_Callbacks = new SourceVoiceCallbacks(this);
	HRESULT hr = audio->CreateSourceVoice(&m_Voice, &m_Format, 0, 2.0f, m_Callbacks);
	assert(SUCCEEDED(hr));
}

SourceVoice::SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format, const XAUDIO2_VOICE_SENDS* destVoices) {
	m_Format = format;

	m_Callbacks = new SourceVoiceCallbacks(this);
	HRESULT hr = audio->CreateSourceVoice(&m_Voice, &m_Format, 0, 2.0f, m_Callbacks, destVoices);
	assert(SUCCEEDED(hr));
}

void SourceVoice::Destroy() {
	::DestroyVoice(m_Voice);

	DeletePtr(m_Callbacks);
}

Bool SourceVoice::Submit(const Sound& sound) {
	return Submit(&sound);
}

Bool SourceVoice::Submit(const Sound* sound) {
	return SUCCEEDED(m_Voice->SubmitSourceBuffer(sound->Data()));
}

Bool SourceVoice::Start(const Sound& sound) {
	return Start(&sound);
}

Bool SourceVoice::Start(const Sound* sound) {
	m_Voice->FlushSourceBuffers();
	if (!Submit(sound)) {
		return false;
	}
	return Start();
}

Bool SourceVoice::Start() {
	return SUCCEEDED(m_Voice->Start());
}

Bool SourceVoice::Start(const Sound& sound, UInt32 operationSet) {
	return Start(&sound, operationSet);
}

Bool SourceVoice::Start(const Sound* sound, UInt32 operationSet) {
	m_Voice->FlushSourceBuffers();
	if (!Submit(sound)) {
		return false;
	}
	return Start(operationSet);
}

Bool SourceVoice::Start(UInt32 operationSet) {
	return SUCCEEDED(m_Voice->Start(0, operationSet));
}

Float32 SourceVoice::Volume() const {
	Float32 volume{};
	m_Voice->GetVolume(&volume);
	return volume;
}

Bool SourceVoice::Volume(Float32 newVolume) const {
	return SUCCEEDED(m_Voice->SetVolume(newVolume));
}

void SourceVoice::SetFrequencyRatio(Float32 ratio) {
	verify(SUCCEEDED(m_Voice->SetFrequencyRatio(ratio)));
}

void SourceVoice::SetFrequencyRatio(Float32 ratio, UInt32 operationSet) {
	verify(SUCCEEDED(m_Voice->SetFrequencyRatio(ratio, operationSet)));
}

Float32 SourceVoice::GetFrequencyRatio() const {
	Float32 ratio;
	m_Voice->GetFrequencyRatio(&ratio);
	return ratio;
}

Bool SourceVoice::SetOutputVoices(const XAUDIO2_VOICE_SENDS* destVoices) {
	return SUCCEEDED(m_Voice->SetOutputVoices(destVoices));
}