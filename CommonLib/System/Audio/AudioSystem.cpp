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

//SourceVoice* AudioSystem::CreateSourceVoice(const WAVEFORMATEX& format, SubmixVoice* destVoice) {
//	UINT64 hash = GenerateHash(format);
//	if (m_FormatMap.Contains(hash)) {
//		return m_FormatMap[hash];
//	}
//
//	SourceVoice* newVoice = new SourceVoice(m_Audio2, format, destVoice->Voice());
//	m_FormatMap.Add(hash, newVoice);
//	m_Voices.Add(newVoice);
//	return newVoice;
//}

SourceVoice* AudioSystem::CreateSourceVoice(const WAVEFORMATEX& format) {
	SourceVoice* voice{};
	UINT64 hash{};
	if (FindSourceVoice(format, hash, voice)) {
		return voice;
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, format);
	auto& list = m_FormatToSourceMap[hash];
	list.Add(newVoice);
	m_Voices.Add(newVoice);
	return newVoice;
}

//SourceVoice* AudioSystem::CreateSourceVoice(Sound* sound, SubmixVoice* destVoice) {
//	UINT64 hash = GenerateHash(sound->Format()->Format);
//	if (m_FormatMap.Contains(hash)) {
//		return m_FormatMap[hash];
//	}
//	SourceVoice* newVoice = new SourceVoice(m_Audio2, sound->Format()->Format, destVoice->Voice());
//	m_FormatMap.Add(hash, newVoice);
//	m_Voices.Add(newVoice);
//
//	newVoice->Submit(sound);
//	return newVoice;
//}

SourceVoice* AudioSystem::CreateSourceVoice(const Sound* sound) {
	SourceVoice* voice{};
	UINT64 hash{};
	if (FindSourceVoice(sound->Format()->Format, hash, voice)) {
		return voice;
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, sound->Format()->Format);
	auto& list = m_FormatToSourceMap[hash];
	list.Add(newVoice);
	m_Voices.Add(newVoice);

	newVoice->Submit(sound);
	return newVoice;
}

SubmixVoice* AudioSystem::CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate) {
	SubmixVoice* newVoice = new SubmixVoice();
	newVoice->Init(m_Audio2, numChannels, sampleRate);
	m_Voices.Add(newVoice);
	return newVoice;
}

Bool AudioSystem::FindSourceVoice(const WAVEFORMATEX& format, UINT64& outHash, SourceVoice*& outVoice) {
	outVoice = nullptr;

	outHash = GenerateHash(format);
	if (!m_FormatToSourceMap.Contains(outHash)) {
		return false;
	}

	auto& list = m_FormatToSourceMap[outHash];
	for (UInt32 ix = 0; ix < list.Length(); ++ix) {
		auto v = list[ix];
		if (!v->IsPlaying()) {
			outVoice = v;
			return true;
		}
	}

	return false;
}