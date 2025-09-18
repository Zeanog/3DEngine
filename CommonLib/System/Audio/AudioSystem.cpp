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

	m_MusicMixVoice = CreateSubmixVoice(1, 44100);
	m_FxMixVoice = CreateSubmixVoice(1, 44100);

	m_SoundCategoryMap.Add(m_MusicMixVoice, Map<UINT64, List<SourceVoice*>>{});
	m_SoundCategoryMap.Add(m_FxMixVoice, Map<UINT64, List<SourceVoice*>>{});

	return true;
}

void AudioSystem::Release() {
	m_MusicMixVoice = nullptr;
	m_FxMixVoice = nullptr;

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

SourceVoice* AudioSystem::PlayFx(const Sound& snd) {
	auto voice = CreateSourceVoice(m_FxMixVoice, snd);
	verify( voice->Start() );
	return voice;
}

SourceVoice* AudioSystem::PlayMusic(const Sound& snd) {
	auto voice = CreateSourceVoice(m_MusicMixVoice, snd);
	verify(voice->Start());
	return voice;
}

SourceVoice* AudioSystem::PlayFx(const Sound* snd) {
	auto voice = CreateSourceVoice(m_FxMixVoice, snd);
	verify(voice->Start());
	return voice;
}

SourceVoice* AudioSystem::PlayMusic(const Sound* snd) {
	auto voice = CreateSourceVoice(m_MusicMixVoice, snd);
	verify(voice->Start());
	return voice;
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

SourceVoice* AudioSystem::CreateSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format) {
	SourceVoice* voice{};
	UINT64 hash{};
	if (FindSourceVoice(voiceCategory, format, hash, voice)) {
		return voice;
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, format);
	verify(AddSourceVoice(voiceCategory, newVoice));
	return newVoice;
}

SourceVoice* AudioSystem::CreateSourceVoice(SubmixVoice* voiceCategory, const Sound* sound) {
	SourceVoice* voice{};
	UINT64 hash{};
	if (FindSourceVoice(voiceCategory, sound->Format()->Format, hash, voice)) {
		return voice;
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, sound->Format()->Format);
	verify(AddSourceVoice(voiceCategory, newVoice));

	newVoice->Submit(sound);
	return newVoice;
}

SourceVoice* AudioSystem::CreateSourceVoice(SubmixVoice* voiceCategory, const Sound& sound) {
	return CreateSourceVoice(voiceCategory, &sound);
}

SubmixVoice* AudioSystem::CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate) {
	SubmixVoice* newVoice = new SubmixVoice();
	newVoice->Init(m_Audio2, numChannels, sampleRate);
	m_Voices.Add(newVoice);
	return newVoice;
}

Bool AudioSystem::FindSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format, UINT64& outHash, SourceVoice*& outVoice) {
	outVoice = nullptr;

	outHash = GenerateHash(format);
	outVoice = nullptr;

	if (!m_SoundCategoryMap.Contains(voiceCategory)) {
		return false;
	}

	auto categoryMap = m_SoundCategoryMap[voiceCategory];
	if (categoryMap.Contains(outHash)) {
		return false;
	}
	
	auto& list = categoryMap[outHash];
	for (UInt32 ix = 0; ix < list.Length(); ++ix) {
		auto v = list[ix];
		if (!v->IsPlaying()) {
			outVoice = v;
			return true;
		}
	}

	return false;
}

Bool AudioSystem::AddSourceVoice(SubmixVoice* voiceCategory, SourceVoice* voice) {
	assert(m_SoundCategoryMap.Contains(voiceCategory));

	auto categoryVoiceMap = m_SoundCategoryMap[voiceCategory];
	auto hash = GenerateHash(voice->Format());
	auto voiceList = categoryVoiceMap[hash];
	voiceList.Add(voice);
	m_Voices.Add(voice);

	voice->SetOutputTo(voiceCategory);

	return true;
}