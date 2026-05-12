#include "SourceVoice.h"
#include "SourceVoiceCallbacks.h"
#include "AudioSystem.h"

SourceVoice::SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format) : TSelf(audio, format, Singleton<AudioSystem>::GetInstance()->GenerateHash(format)) {
	
}

SourceVoice::SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format, UInt64 formatHash) {
	m_Format = format;
	m_FormatHash = formatHash;

	m_Callbacks = new SourceVoiceCallbacks(this);
	HRESULT hr = audio->CreateSourceVoice(&m_Voice, &m_Format, 0, 2.0f, m_Callbacks);
	assert(SUCCEEDED(hr));
}

void SourceVoice::Destroy() {
	TSuper::Destroy();

	//Destroy the voice first to avoid possible dangling callback pointer
	DeletePtr(m_Callbacks);
}

Bool SourceVoice::Submit(const XAUDIO2_BUFFER& buffer) {
	assert(m_Voice);
	return SUCCEEDED(m_Voice->SubmitSourceBuffer(&buffer));
}

Bool SourceVoice::Submit(const XAUDIO2_BUFFER* buffer) {
	assert(m_Voice);
	return SUCCEEDED(m_Voice->SubmitSourceBuffer(buffer));
}

Bool SourceVoice::Start(const XAUDIO2_BUFFER& buffer, UInt32 operationSet) {
	verify(FlushBuffers());
	if (!Submit(buffer)) {
		return false;
	}
	return Start(operationSet);
}

Bool SourceVoice::Start(const XAUDIO2_BUFFER* buffer, UInt32 operationSet) {
	verify(FlushBuffers());
	if (!Submit(buffer)) {
		return false;
	}
	return Start(operationSet);
}

Bool SourceVoice::Start() {
	return Start((UInt32)XAUDIO2_COMMIT_NOW);
}

Bool SourceVoice::Start(UInt32 operationSet) {
	assert(m_Voice);
	return SUCCEEDED(m_Voice->Start(0U, operationSet));
}