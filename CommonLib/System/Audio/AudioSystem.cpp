#include "AudioSystem.h"

#include "Sound.h"
#include "MasteringVoice.h"
#include "SourceVoice.h"
#include "SubmixVoice.h"

#include <cstdint>
#include <limits>

std::mutex AudioSystem::m_Mutex;

Bool AudioSystem::Init() {
	HRESULT hr = ::XAudio2Create(&m_Audio2, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		return false;
	}

	m_MasteringVoice = CreateMasteringVoice();
	return true;
}

void AudioSystem::Destroy() {
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

	auto voice = CreateSubmixVoice(numChannels, sampleRate);
	m_CategoryNameToVoiceMap.Add(name, voice);
	m_VoiceToCategoryNameMap.Add(voice, name);

	m_CategoryToVoiceListMap.Add(voice, TFormatToVoiceList());

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

#include "xaudio2fx.h"
Bool AudioSystem::AddEffectDescriptors(const StaticString& categoryName, UInt32 numDescriptors) {
	XAUDIO2_EFFECT_DESCRIPTOR* effectDescriptors = STACK_ALLOC(XAUDIO2_EFFECT_DESCRIPTOR, numDescriptors);

	auto category = GetCategory(categoryName);
	for (UInt32 ix = 0; ix < numDescriptors; ++ix) {
		IUnknown* pReverbEffect{};
		verify(SUCCEEDED(XAudio2CreateReverb(&pReverbEffect)));
		effectDescriptors[ix] = { pReverbEffect, true, category->NumChannels() };
	}

	verify(category->SetEffectDescriptors(effectDescriptors, numDescriptors));

	for (UInt32 ix = 0; ix < numDescriptors; ++ix) {
		effectDescriptors[ix].pEffect->Release();
	}

	return true;
}

#include "ReverbParameters.h"
Bool AudioSystem::SetEffectParameters(const StaticString& categoryName, UInt32 index, const ReverbParameters& params) {
	return GetCategory(categoryName)->SetEffectParameters(index, &params, sizeof(decltype(params)), 0U);
}

SourceVoice* AudioSystem::Play(const StaticString& categoryName, const Sound& snd) {
	auto voice = GetSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify(voice->Start());
	return voice;
}

SourceVoice* AudioSystem::Play(const StaticString& categoryName, const Sound* snd) {
	auto voice = GetSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify(voice->Start());
	return voice;
}

#include "System\Audio\Loaders\SoundManager.h"
Float32 AudioSystem::Play(const StaticString& categoryName, const StaticString& filePath, SourceVoice*& outVoice) {
	auto snd = Singleton<SoundManager>::GetInstance()->Get(filePath);
	if (!snd) {
		return 0.0f;
	}
	outVoice = GetSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify(outVoice->Start());
	return snd->Duration();
}

Float32 AudioSystem::Submit(const StaticString& categoryName, const StaticString& filePath, SourceVoice*& outVoice) {
	auto snd = Singleton<SoundManager>::GetInstance()->Get(filePath);
	if (!snd) {
		return 0.0f;
	}
	outVoice = GetSourceVoice(GetCategory(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	return snd->Duration();
}

void AudioSystem::StopAllVoices() {
	FOREACH(iterVoiceFormatMap, m_CategoryToVoiceListMap) {
		FOREACH(iterVoiceList, iterVoiceFormatMap->second) {
			FOREACH(iterVoice, iterVoiceList->second) {
				auto v = *iterVoice;
				v->Stop();
			}
		}
	}
}

MasteringVoice* AudioSystem::CreateMasteringVoice() {
	MasteringVoice* newVoice = new MasteringVoice();
	newVoice->Init(m_Audio2);
	return newVoice;
}

SourceVoice* AudioSystem::GetSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format) {
	SourceVoice* voice{};
	if (FindSourceVoice(voiceCategory, format, voice)) {
		return voice;
	}

	voice = new SourceVoice(m_Audio2, format);
	verify(AddSourceVoice(voiceCategory, voice));
	return voice;
}

SourceVoice* AudioSystem::GetSourceVoice(SubmixVoice* voiceCategory, const Sound* sound) {
	SourceVoice* voice = GetSourceVoice(voiceCategory, sound->Format().Format);
	voice->Submit(sound);
	return voice;
}

SourceVoice* AudioSystem::GetSourceVoice(SubmixVoice* voiceCategory, const Sound& sound) {
	return GetSourceVoice(voiceCategory, &sound);
}

SubmixVoice* AudioSystem::CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate) {
	SubmixVoice* newVoice = new SubmixVoice();
	newVoice->Init(m_Audio2, numChannels, sampleRate);
	m_Voices.Add(newVoice);
	return newVoice;
}

Bool AudioSystem::FindSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format, SourceVoice*& outVoice) const {
	outVoice = nullptr;

	std::lock_guard<std::mutex> guard(m_Mutex);

	auto& categoryMap = m_CategoryToVoiceListMap[voiceCategory];
	if (categoryMap.Length() <= 0) {
		return false;
	}

	UInt64 hash = GenerateHash(format);
	auto& voiceList = categoryMap[hash];
	
	FOREACH_CONST_R(iter, voiceList) {
		auto v = *iter;
		if (!v->IsPlaying()) {
			outVoice = v;
			return true;
		}
	}

	return false;
}

Bool AudioSystem::AddSourceVoice(SubmixVoice* voiceCategory, SourceVoice* voice) {
	std::lock_guard<std::mutex> guard(m_Mutex);

	assert(m_CategoryToVoiceListMap.Contains(voiceCategory));
	auto& categoryVoiceMap = m_CategoryToVoiceListMap[voiceCategory];
	auto hash = GenerateHash(voice->Format());

	if (!categoryVoiceMap.Contains(hash)) {
		categoryVoiceMap.Add(hash, TVoiceList());
	}
	auto& voiceList = categoryVoiceMap[hash];
	voiceList.Add(voice);

	m_Voices.Add(voice);
	m_VoiceToCategoryMap.Add(voice, voiceCategory);

	voice->OnBufferStart.AddListener(this, &AudioSystem::OnBufferStartHandler);
	voice->OnBufferEnd.AddListener(this, &AudioSystem::OnBufferEndHandler);

	return voice->SetOutputTo(voiceCategory);
}

void AudioSystem::OnBufferStartHandler(SourceVoice* voice, void* context) {
	std::lock_guard<std::mutex> guard(m_Mutex);

	assert(context);
	Sound* snd = (Sound*)context;
};

void AudioSystem::OnBufferEndHandler(SourceVoice* voice, void* context) {
	std::lock_guard<std::mutex> guard(m_Mutex);

	assert(m_VoiceToCategoryMap.Contains(voice));
	auto category = m_VoiceToCategoryMap[voice];

	assert(m_CategoryToVoiceListMap.Contains(category));
	auto& voiceListMap = m_CategoryToVoiceListMap[category];

	assert(context);
	Sound* snd = (Sound*)context;

	auto hash = GenerateHash(snd->Format().Format);
	assert(voiceListMap.Contains(hash));
	auto& voiceList = voiceListMap[hash];

	assert(!voice->IsPlaying());

	//Place the voice to the back of the list so we can find open voices quickly
	voiceList.Remove(voice);
	voiceList.Add(voice);
};

//#include "System/DataStructureLibrary.h"

void AudioSystem::ReloadAssets() {
	//List<SourceVoice*>* playingVoices = Singleton<DataStructureLibrary<List<SourceVoice*>>>::GetInstance()->CheckOut();
	//playingVoices->Clear();

	////Store which voices are playing
	//FOREACH(iterVoiceFormatMap, m_CategoryToVoiceListMap) {
	//	FOREACH(iterVoiceList, iterVoiceFormatMap->second) {
	//		FOREACH(iterVoice, iterVoiceList->second) {
	//			auto v = *iterVoice;
	//			if (v->IsPlaying()) {
	//				playingVoices->Add(v);// This will lose any voices playing with an operationSet
	//			}
	//			v->Stop();
	//		}
	//	}
	//}
	//Singleton<SoundManager>::GetInstance()->ReloadAll();

	//FOREACH(iter, *playingVoices) {
	//	auto v = *iter;
	//	v->Start();
	//}

	//Singleton<DataStructureLibrary<List<SourceVoice*>>>::GetInstance()->Return(playingVoices);
}