#include "SourceVoice.h"
#include "Sound.h"
#include "SourceVoiceCallbacks.h"

SourceVoice::SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format) {
	m_Format = format;

	m_Callbacks = new SourceVoiceCallbacks(this);
	HRESULT hr = audio->CreateSourceVoice(&m_Voice, &m_Format, 0, 2.0f, m_Callbacks);
	assert(SUCCEEDED(hr));
}

void SourceVoice::Destroy() {
	TSuper::Destroy();

	//Destroy the voice first to avoid possible dangling callback pointer
	DeletePtr(m_Callbacks);
}

Bool SourceVoice::Submit(const Sound& sound) {
	return Submit(&sound);
}

Bool SourceVoice::Submit(const Sound* sound) {
	assert(m_Voice);
	return SUCCEEDED(m_Voice->SubmitSourceBuffer(sound->Buffer()));
}

Bool SourceVoice::Start(const Sound& sound) {
	return Start(&sound);
}

Bool SourceVoice::Start(const Sound* sound) {
	assert(m_Voice);
	m_Voice->FlushSourceBuffers();
	if (!Submit(sound)) {
		return false;
	}
	return Start();
}

Bool SourceVoice::Start() {
	assert(m_Voice);
	return SUCCEEDED(m_Voice->Start());
}

Bool SourceVoice::Start(const Sound& sound, UInt32 operationSet) {
	return Start(&sound, operationSet);
}

Bool SourceVoice::Start(const Sound* sound, UInt32 operationSet) {
	//m_Voice->FlushSourceBuffers();
	if (!Submit(sound)) {
		return false;
	}
	return Start(operationSet);
}

Bool SourceVoice::Start(UInt32 operationSet) {
	assert(m_Voice);
	return SUCCEEDED(m_Voice->Start(0, operationSet));
}