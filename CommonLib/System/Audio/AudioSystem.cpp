#include "AudioSystem.h"

#include "Sound.h"
#include "MasteringVoice.h"
#include "SourceVoice.h"
#include "SubmixVoice.h"

Bool AudioSystem::Init() {
	HRESULT hr = ::XAudio2Create(&m_Audio2, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		return false;
	}

	m_MasteringVoice = CreateMasteringVoice();

	return true;
}

void AudioSystem::Release() {
	FOREACH(iter, m_Voices) {
		(*iter)->Destroy();
	}
	m_Voices.Clear();

	if (m_MasteringVoice) {
		m_MasteringVoice->Destroy();
		m_MasteringVoice = nullptr;
	}

	::Release(m_Audio2);
}

MasteringVoice* AudioSystem::CreateMasteringVoice() {
	MasteringVoice* newVoice = new MasteringVoice();
	newVoice->Init(m_Audio2);
	return newVoice;
}

template<typename TArg>
XAUDIO2_SEND_DESCRIPTOR	BuildSendList(TArg arg) {
	return { 0, destVoice->Voice() };
}

template<typename TArg, typename... TArgs>
XAUDIO2_SEND_DESCRIPTOR	BuildSendList(TArgs... args) {

}

SourceVoice* AudioSystem::CreateSourceVoice(const WAVEFORMATEX& format, SubmixVoice* destVoice) {
	UINT64 hash = GenerateHash(format);
	if (m_FormatMap.Contains(hash)) {
		return m_FormatMap[hash];
	}

	XAUDIO2_SEND_DESCRIPTOR fxSend = { 0, destVoice->Voice() };
	XAUDIO2_VOICE_SENDS sendList = { 1, &fxSend };

	SourceVoice* newVoice = new SourceVoice(m_Audio2, format, &sendList);
	m_FormatMap.Add(hash, newVoice);
	m_Voices.Add(newVoice);
	return newVoice;
}

SourceVoice* AudioSystem::CreateSourceVoice(const WAVEFORMATEX& format) {
	UINT64 hash = GenerateHash(format);
	if (m_FormatMap.Contains(hash)) {
		return m_FormatMap[hash];
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, format);
	m_FormatMap.Add(hash, newVoice);
	m_Voices.Add(newVoice);
	return newVoice;
}

SourceVoice* AudioSystem::CreateSourceVoice(Sound* sound) {
	SourceVoice* newVoice = CreateSourceVoice(sound->Format()->Format);
	newVoice->Submit(sound);
	return newVoice;
}

SubmixVoice* AudioSystem::CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate) {
	SubmixVoice* newVoice = new SubmixVoice();
	newVoice->Init(m_Audio2, numChannels, sampleRate);
	return newVoice;
}