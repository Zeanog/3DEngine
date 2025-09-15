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

SourceVoice* AudioSystem::CreateSourceVoice(const WAVEFORMATEX& format, SubmixVoice* destVoice) {
	UINT64 hash = GenerateHash(format);
	if (m_FormatMap.Contains(hash)) {
		return m_FormatMap[hash];
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, format, destVoice->Voice());
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

SourceVoice* AudioSystem::CreateSourceVoice(Sound* sound, SubmixVoice* destVoice) {
	UINT64 hash = GenerateHash(sound->Format()->Format);
	if (m_FormatMap.Contains(hash)) {
		return m_FormatMap[hash];
	}
	SourceVoice* newVoice = new SourceVoice(m_Audio2, sound->Format()->Format, destVoice->Voice());
	m_FormatMap.Add(hash, newVoice);
	m_Voices.Add(newVoice);

	newVoice->Submit(sound);
	return newVoice;
}

#include <xaudio2fx.h>

SubmixVoice* AudioSystem::CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate) {
	SubmixVoice* newVoice = new SubmixVoice();
	newVoice->Init(m_Audio2, numChannels, sampleRate);
	m_Voices.Add(newVoice);
	return newVoice;
}