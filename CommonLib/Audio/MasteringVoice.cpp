#include "MasteringVoice.h"

MasteringVoice::MasteringVoice() {
}

Bool MasteringVoice::Init(IXAudio2* audio) {
	assert(!m_Voice);
	HRESULT hr = audio->CreateMasteringVoice(&m_Voice, XAUDIO2_DEFAULT_CHANNELS, 0, 0, NULL, NULL);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void MasteringVoice::Destroy() {
	::DestroyVoice(m_Voice);
}