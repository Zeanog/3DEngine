#include "AudioSystem.h"

#include "Sound.h"
#include "MasteringVoice.h"
#include "SourceVoice.h"
#include "SubmixVoice.h"

#include <cstdint>
#include <limits>

Bool AudioSystem::Init() {
	HRESULT hr = ::XAudio2Create(&m_Audio2, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		return false;
	}

	m_MasteringVoice = CreateMasteringVoice();
	return true;
}

void AudioSystem::Release() {
	m_CategoryMap.Clear();

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

#undef max
UINT64 AudioSystem::GenerateHash(const WAVEFORMATEX& format) {
	static constexpr Byte numChannelShift = 56;
	static constexpr Byte samplesPerSecShift = 32;
	static constexpr auto samplesPerSecMask = std::numeric_limits<UInt16>::max();
	static constexpr Byte bitsPerSampleShift = 16;
	static constexpr auto bitsPerSampleMask = std::numeric_limits<Byte>::max();
	static constexpr auto formatTagMask = std::numeric_limits<Byte>::max();

	auto val = (UINT64)format.nChannels << numChannelShift | (UINT64)format.nSamplesPerSec << samplesPerSecShift | (UINT64)format.wBitsPerSample << bitsPerSampleShift | format.wFormatTag;

	assert((val >> numChannelShift) == format.nChannels);
	assert(((val >> samplesPerSecShift) & samplesPerSecMask) == format.nSamplesPerSec);
	assert(((val >> bitsPerSampleShift) & bitsPerSampleMask) == format.wBitsPerSample);
	assert((val & formatTagMask) == format.wFormatTag);

	return val;
}

SubmixVoice* AudioSystem::GetCategory(const StaticString& name) const {
	assert(m_CategoryMap.Contains(name));
	return m_CategoryMap[name];
}

Bool AudioSystem::AddCategory(const StaticString& name, UInt32 numChannels, UInt32 sampleRate) {
	assert(!m_CategoryMap.Contains(name));

	m_CategoryMap.Add(name, CreateSubmixVoice(numChannels, sampleRate));
	return true;
}

UINT64 AudioSystem::GenerateHash(UInt32 numChannels, UInt32 sampleRate) {
	static constexpr Byte numChannelShift = 56;
	static constexpr Byte samplesPerSecShift = 32;

	auto val = (UINT64)numChannels << numChannelShift | (UINT64)sampleRate << samplesPerSecShift;

	assert(((val >> numChannelShift) & std::numeric_limits<Byte>::max()) == numChannels);
	assert(((val >> samplesPerSecShift) & std::numeric_limits<Byte>::max()) == sampleRate);

	return val;
}

SourceVoice* AudioSystem::Play(const StaticString& categoryName, const Sound& snd) {
	auto voice = CreateSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify( voice->Start() );
	return voice;
}

SourceVoice* AudioSystem::Play(const StaticString& categoryName, const Sound* snd) {
	auto voice = CreateSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
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
	if (FindSourceVoice(voiceCategory, format, voice)) {
		return voice;
	}

	voice = new SourceVoice(m_Audio2, format);
	verify(AddSourceVoice(voiceCategory, voice));
	return voice;
}

SourceVoice* AudioSystem::CreateSourceVoice(SubmixVoice* voiceCategory, const Sound* sound) {
	SourceVoice* voice{};
	if (FindSourceVoice(voiceCategory, sound->Format().Format, voice)) {
		return voice;
	}

	voice = new SourceVoice(m_Audio2, sound->Format().Format);
	verify(AddSourceVoice(voiceCategory, voice));

	voice->Submit(sound);
	return voice;
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

Bool AudioSystem::FindSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format, SourceVoice*& outVoice) {
	outVoice = nullptr;

	auto& categoryMap = m_SoundCategoryMap[voiceCategory];

	UInt64 hash = GenerateHash(format);
	auto& list = categoryMap[hash];
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

	auto& categoryVoiceMap = m_SoundCategoryMap[voiceCategory];
	auto hash = GenerateHash(voice->Format());
	auto voiceList = categoryVoiceMap[hash];
	voiceList.Add(voice);
	m_Voices.Add(voice);

	voice->SetOutputTo(voiceCategory);

	return true;
}