#include "SourceVoiceCallbacks.h"

#include "SourceVoice.h"

SourceVoiceCallbacks::SourceVoiceCallbacks(SourceVoice* source) {
	m_Voice = source;
}

void SourceVoiceCallbacks::OnVoiceProcessingPassStart(UINT32 bytesRequired) {
	assert(m_Voice);
	m_Voice->OnVoiceProcessingPassStart(m_Voice, bytesRequired);
}

void SourceVoiceCallbacks::OnVoiceProcessingPassEnd() {
	assert(m_Voice);
	m_Voice->OnVoiceProcessingPassEnd(m_Voice);
}

void SourceVoiceCallbacks::OnStreamEnd() {
	assert(m_Voice);
	m_Voice->OnStreamEnd(m_Voice);
}

void SourceVoiceCallbacks::OnBufferStart(void* pBufferContext) {
	assert(m_Voice);
	m_Voice->OnBufferStart(m_Voice, pBufferContext);
}

void SourceVoiceCallbacks::OnBufferEnd(void* pBufferContext) {
	assert(m_Voice);
	m_Voice->OnBufferEnd(m_Voice, pBufferContext);
}

void SourceVoiceCallbacks::OnLoopEnd(void* pBufferContext) {
	assert(m_Voice);
	m_Voice->OnLoopEnd(m_Voice, pBufferContext);
}

void SourceVoiceCallbacks::OnVoiceError(void* pBufferContext, HRESULT error) {
	assert(m_Voice);
	m_Voice->OnVoiceError(m_Voice, pBufferContext, error);
}