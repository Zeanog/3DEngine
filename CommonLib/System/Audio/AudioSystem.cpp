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
	m_CategoryToVoiceListMap.Clear();
	m_CategoryNameToVoiceMap.Clear();
	m_VoiceToCategoryMap.Clear();
	m_VoiceToCategoryNameMap.Clear();

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
	static constexpr Byte bitsPerSampleShift = 16;
	static constexpr auto uint16BitMask = std::numeric_limits<UInt16>::max();
	static constexpr auto byteBitMask = std::numeric_limits<Byte>::max();

	auto val = (UINT64)format.nChannels << numChannelShift | (UINT64)format.nSamplesPerSec << samplesPerSecShift | (UINT64)format.wBitsPerSample << bitsPerSampleShift | format.wFormatTag;

	assert(((val >> numChannelShift) & byteBitMask) == format.nChannels);
	assert(((val >> samplesPerSecShift) & uint16BitMask) == format.nSamplesPerSec);
	assert(((val >> bitsPerSampleShift) & byteBitMask) == format.wBitsPerSample);
	assert((val & byteBitMask) == format.wFormatTag);

	return val;
}

SubmixVoice* AudioSystem::GetCategory(const StaticString& name) const {
	assert(m_CategoryNameToVoiceMap.Contains(name));
	return m_CategoryNameToVoiceMap[name];
}

const StaticString& AudioSystem::GetCategoryName(SubmixVoice* categoryVoice) const {
	assert(m_VoiceToCategoryNameMap.Contains(categoryVoice));
	return m_VoiceToCategoryNameMap[categoryVoice];
}

Bool AudioSystem::AddCategory(const StaticString& name, UInt32 numChannels, UInt32 sampleRate) {
	assert(!m_CategoryNameToVoiceMap.Contains(name));

	m_CategoryNameToVoiceMap.Add(name, CreateSubmixVoice(numChannels, sampleRate));
	m_VoiceToCategoryNameMap.Add(m_CategoryNameToVoiceMap[name], name);
	return true;
}

UINT64 AudioSystem::GenerateHash(UInt32 numChannels, UInt32 sampleRate) {
	static constexpr Byte numChannelShift = 56;
	static constexpr Byte samplesPerSecShift = 32;
	static constexpr auto eightBitMask = std::numeric_limits<Byte>::max();

	auto val = (UINT64)numChannels << numChannelShift | (UINT64)sampleRate << samplesPerSecShift;

	assert(((val >> numChannelShift) & eightBitMask) == numChannels);
	assert(((val >> samplesPerSecShift) & eightBitMask) == sampleRate);

	return val;
}

SourceVoice* AudioSystem::Play(const StaticString& categoryName, const Sound& snd) {
	auto voice = CreateSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify(voice->Start());
	return voice;
}

SourceVoice* AudioSystem::Play(const StaticString& categoryName, const Sound* snd) {
	auto voice = CreateSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify(voice->Start());
	return voice;
}

#include "System\Audio\Loaders\SoundManager.h"
SourceVoice* AudioSystem::Play(const StaticString& categoryName, const StaticString& filePath, Sound*& outSnd) {
	outSnd = Singleton<SoundManager>::GetInstance()->Get(filePath);
	if (!outSnd) {
		return nullptr;
	}
	auto voice = CreateSourceVoice(GetCategory(categoryName), outSnd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify(voice->Start());
	return voice;
}

MasteringVoice* AudioSystem::CreateMasteringVoice() {
	MasteringVoice* newVoice = new MasteringVoice();
	newVoice->Init(m_Audio2);
	return newVoice;
}

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
	SourceVoice* voice = CreateSourceVoice(voiceCategory, sound->Format().Format);
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

	auto& categoryMap = m_CategoryToVoiceListMap[voiceCategory];

	UInt64 hash = GenerateHash(format);
	auto& voiceList = categoryMap[hash];
	for (UInt32 ix = 0; ix < voiceList.Length(); ++ix) {
		auto v = voiceList[ix];
		if (!v->IsPlaying()) {
			outVoice = v;
			return true;
		}
	}

	return false;
}

Bool AudioSystem::AddSourceVoice(SubmixVoice* voiceCategory, SourceVoice* voice) {
	assert(m_CategoryToVoiceListMap.Contains(voiceCategory));

	auto& categoryVoiceMap = m_CategoryToVoiceListMap[voiceCategory];
	auto hash = GenerateHash(voice->Format());
	auto& voiceList = categoryVoiceMap[hash];
	voiceList.Add(voice);
	m_Voices.Add(voice);
	m_VoiceToCategoryMap.Add(voice, voiceCategory);

	voice->OnBufferEnd.AddListener(this, &AudioSystem::OnBufferEndHandler);

	voice->SetOutputTo(voiceCategory);

	return true;
}

void AudioSystem::OnBufferEndHandler(SourceVoice* voice, void* v) {
	Sound* snd = (Sound*)v;
	auto category = m_VoiceToCategoryMap[voice];
	auto& voiceListMap = m_CategoryToVoiceListMap[category];
	auto& voiceList = voiceListMap[GenerateHash(snd->Format().Format)];

	//Put non-playing voices at the top of the list.  Speed up finding open voices
	voiceList.Sort([](const SourceVoice* lhs, const SourceVoice* rhs) {
		if (!lhs->IsPlaying()) {
			return true;
		}

		if (!rhs->IsPlaying()) {
			return false;
		}

		return true;
	});
};