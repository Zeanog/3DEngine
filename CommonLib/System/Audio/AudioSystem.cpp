#include "AudioSystem.h"

#include "Sound.h"
#include "MasteringVoice.h"
#include "SourceVoice.h"
#include "SubmixVoice.h"
#include "EchoParameters.h"
#include "ReverbParameters.h"
#include "EQParameters.h"
#include "System/DataStructureLibrary.h"

#include <cstdint>
#include <limits>

std::mutex AudioSystem::m_Mutex;

Bool AudioSystem::Init() {
	HRESULT hr = ::XAudio2Create(&m_Audio2, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		return false;
	}

	m_MasteringVoice = CreateMasteringVoice();

	m_FxNameToInfoMap.Add("Reverb", { ReverbParameters::InstantiateFX, ReverbParameters::UpdateParams });
	m_FxNameToInfoMap.Add("Echo", { EchoParameters::InstantiateFX, EchoParameters::UpdateParams });
	m_FxNameToInfoMap.Add("EQ", { EQParameters::InstantiateFX, EQParameters::UpdateParams });
	return true;
}

void AudioSystem::Destroy() {
	m_CategoryNameToVoiceMap.Clear();
	m_FormatToVoiceListMap.Clear();
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
UInt64 AudioSystem::GenerateHash(const WAVEFORMATEX& format) {
	static constexpr Byte numChannelShift = 56;
	static constexpr Byte samplesPerSecShift = 32;
	static constexpr Byte bitsPerSampleShift = 16;
	static constexpr auto uint16BitMask = std::numeric_limits<UInt16>::max();
	static constexpr auto byteBitMask = std::numeric_limits<Byte>::max();

	auto val = (UInt64)format.nChannels << numChannelShift | (UInt64)format.nSamplesPerSec << samplesPerSecShift | (UInt64)format.wBitsPerSample << bitsPerSampleShift | format.wFormatTag;

	assert(((val >> numChannelShift) & byteBitMask) == format.nChannels);
	assert(((val >> samplesPerSecShift) & uint16BitMask) == format.nSamplesPerSec);
	assert(((val >> bitsPerSampleShift) & byteBitMask) == format.wBitsPerSample);
	assert((val & byteBitMask) == format.wFormatTag);

	return val;
}

UInt64 AudioSystem::GenerateHash(UInt32 numChannels, UInt32 sampleRate) {
	static constexpr Byte numChannelShift = 56;
	static constexpr Byte samplesPerSecShift = 32;
	static constexpr auto byteBitMask = std::numeric_limits<Byte>::max();

	auto val = (UInt64)numChannels << numChannelShift | (UInt64)sampleRate << samplesPerSecShift;

	assert(((val >> numChannelShift) & byteBitMask) == numChannels);
	assert(((val >> samplesPerSecShift) & byteBitMask) == sampleRate);

	return val;
}

SubmixVoice* AudioSystem::GetCategoryVoice(const StaticString& name) const {
	assert(m_CategoryNameToVoiceMap.Contains(name));
	return m_CategoryNameToVoiceMap[name];
}

const StaticString& AudioSystem::GetCategoryName(SubmixVoice* categoryVoice) const {
	assert(m_VoiceToCategoryNameMap.Contains(categoryVoice));
	return m_VoiceToCategoryNameMap[categoryVoice];
}

SubmixVoice* AudioSystem::AddCategory(const StaticString& name, UInt32 numChannels, UInt32 sampleRate) {
	assert(!m_CategoryNameToVoiceMap.Contains(name));

	auto voice = CreateSubmixVoice(numChannels, sampleRate);
	m_CategoryNameToVoiceMap.Add(name, voice);
	m_VoiceToCategoryNameMap.Add(voice, name);

	return voice;
}

SourceVoice* AudioSystem::Play(const Sound& snd, const StaticString& categoryName) {
	auto voice = GetSourceVoice(GetCategoryVoice(categoryName), snd);
	verify(voice->Start());
	return voice;
}

SourceVoice* AudioSystem::Play(const Sound* snd, const StaticString& categoryName) {
	auto voice = GetSourceVoice(GetCategoryVoice(categoryName), snd);
	verify(voice->Start());
	return voice;
}

#include "System\Audio\Loaders\SoundManager.h"
Float32 AudioSystem::Play(const StaticString& filePath, const StaticString& categoryName, SourceVoice*& outVoice) {
	auto snd = Singleton<SoundManager>::GetInstance()->Get(filePath);
	if (!snd) {
		return 0.0f;
	}
	outVoice = GetSourceVoice(GetCategoryVoice(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	verify(outVoice->Start());
	return snd->Duration();
}

Float32 AudioSystem::Submit(const StaticString& filePath, const StaticString& categoryName, SourceVoice*& outVoice) {
	auto snd = Singleton<SoundManager>::GetInstance()->Get(filePath);
	if (!snd) {
		return 0.0f;
	}
	outVoice = GetSourceVoice(GetCategoryVoice(categoryName), snd);
	//voice->Volume(1.0f);//TODO: Set volume back to full
	return snd->Duration();
}

void AudioSystem::StopAllVoices() {
	FOREACH(iterVoiceList, m_FormatToVoiceListMap) {
		FOREACH(iterVoice, iterVoiceList->second) {
			auto v = *iterVoice;
			v->Stop();
		}
	}
}

Bool AudioSystem::SetEffectDescriptors(const StaticString& categoryName, const Char** fxNames, UInt32 numFx) {
	return SetEffectDescriptors(GetCategoryVoice(categoryName), fxNames, numFx);
}

Bool AudioSystem::SetEffectDescriptors(SubmixVoice* category, const Char** fxNames, UInt32 numFx) {
	__try {
		auto descriptors = STACK_ALLOC(XAUDIO2_EFFECT_DESCRIPTOR, numFx);
		XAUDIO2_EFFECT_CHAIN chain{ numFx, descriptors };

		for (UInt32 ix = 0; ix < numFx; ++ix) {
			auto name = fxNames[ix];
			auto info = &m_FxNameToInfoMap[name];
			descriptors[ix] = { info->Create(), false, category->NumChannels() };
		}

		Bool result = category->SetEffectChain(&chain);
		assert(result);

		for (UInt32 ix = 0; ix < numFx; ++ix) {
			assert(descriptors[ix].pEffect);
			verify(descriptors[ix].pEffect->Release() > 0);
		}

		return result;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}

Bool AudioSystem::SetEffectDescriptors(SubmixVoice* category, FXInfo** fxInfos, UInt32 numFx) {
	__try {
		auto descriptors = STACK_ALLOC(XAUDIO2_EFFECT_DESCRIPTOR, numFx);
		XAUDIO2_EFFECT_CHAIN chain{ numFx, descriptors };

		for (UInt32 ix = 0; ix < numFx; ++ix) {
			auto info = fxInfos[ix];
			descriptors[ix] = { info->Create(), false, category->NumChannels() };
		}

		Bool result = category->SetEffectChain(&chain);
		assert(result);

		for (UInt32 ix = 0; ix < numFx; ++ix) {
			assert(descriptors[ix].pEffect);
			verify(descriptors[ix].pEffect->Release() > 0);
		}

		return result;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}

Bool AudioSystem::IsEffectEnabled(const StaticString& categoryName, UInt32 index) {
	return GetCategoryVoice(categoryName)->IsEffectEnabled(index);
}

Bool AudioSystem::EnableEffect(const StaticString& categoryName, UInt32 index) {
	return GetCategoryVoice(categoryName)->EnableEffect(index);
}

Bool AudioSystem::EnableEffect(const StaticString& categoryName, UInt32 index, UInt32 operationSet) {
	return GetCategoryVoice(categoryName)->EnableEffect(index, operationSet);
}

Bool AudioSystem::DisableEffect(const StaticString& categoryName, UInt32 index) {
	return GetCategoryVoice(categoryName)->DisableEffect(index);
}

Bool AudioSystem::DisableEffect(const StaticString& categoryName, UInt32 index, UInt32 operationSet) {
	return GetCategoryVoice(categoryName)->DisableEffect(index, operationSet);
}

Bool AudioSystem::SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
	return GetCategoryVoice(categoryName)->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U);
}

Bool AudioSystem::SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
	return GetCategoryVoice(categoryName)->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet);
}

MasteringVoice* AudioSystem::CreateMasteringVoice() {
	MasteringVoice* newVoice = new MasteringVoice();
	verify(newVoice->Init(m_Audio2));
	return newVoice;
}

SourceVoice* AudioSystem::GetSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format) {
	SourceVoice* voice{};
	if (FindSourceVoice(format, voice)) {
		voice->SetOutputTo(voiceCategory);
		return voice;
	}

	voice = new SourceVoice(m_Audio2, format);
	verify(AddSourceVoice(voice));
	voice->SetOutputTo(voiceCategory);
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

Bool AudioSystem::FindSourceVoice(const WAVEFORMATEX& format, SourceVoice*& outVoice) const {
	outVoice = nullptr;

	std::lock_guard<std::mutex> guard(m_Mutex);

	UInt64 hash = GenerateHash(format);//TODO: Possibly cache this hash, in Sound, to avoid recomputing it
	if (!m_FormatToVoiceListMap.Contains(hash)) {
		return false;
	}

	auto& voiceList = m_FormatToVoiceListMap[hash];
	if (voiceList.Length() <= 0) {
		return false;
	}

	FOREACH_CONST_REV(iter, voiceList) {
		auto v = *iter;
		if (!v->IsPlaying()) {
			outVoice = v;
			return true;
		}
	}

	return false;
}

Bool AudioSystem::AddSourceVoice(SourceVoice* voice) {
	std::lock_guard<std::mutex> guard(m_Mutex);

	auto hash = GenerateHash(voice->Format());//TODO: Possibly cache this hash, in Sound, to avoid recomputing it
	if (!m_FormatToVoiceListMap.Contains(hash)) {
		m_FormatToVoiceListMap.Add(hash, TVoiceList());
	}
	auto& voiceList = m_FormatToVoiceListMap[hash];
	voiceList.Add(voice);

	m_Voices.Add(voice);

	voice->OnVoiceError.AddListener(this, &AudioSystem::OnVoiceErrorHandler);
	voice->OnBufferStart.AddListener(this, &AudioSystem::OnBufferStartHandler);
	voice->OnBufferEnd.AddListener(this, &AudioSystem::OnBufferEndHandler);

	return true;
}

void AudioSystem::OnVoiceErrorHandler(SourceVoice* voice, void* context, HRESULT result) {
	std::lock_guard<std::mutex> guard(m_Mutex);

	assert(context);
	Sound* snd = (Sound*)context;

	assert(0);
}

void AudioSystem::OnBufferStartHandler(SourceVoice* voice, void* context) {
	std::lock_guard<std::mutex> guard(m_Mutex);

	assert(context);
	Sound* snd = (Sound*)context;
};

void AudioSystem::OnBufferEndHandler(SourceVoice* voice, void* context) {
	std::lock_guard<std::mutex> guard(m_Mutex);

	assert(context);
	Sound* snd = (Sound*)context;

	auto hash = GenerateHash(snd->Format().Format);//TODO: Possibly cache this hash, in Sound, to avoid recomputing it
	assert(m_FormatToVoiceListMap.Contains(hash));
	auto& voiceList = m_FormatToVoiceListMap[hash];

	assert(!voice->IsPlaying());
	voice->SetOutputTo();//Clear output voices to avoid audio glitches when reusing the voice

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

UInt32	AudioSystem::LoadEffects(const StaticString& path, const StaticString& categoryName) {
	return LoadEffects(path, GetCategoryVoice(categoryName));
}

#include "System/JsonLoader.h"
UInt32	AudioSystem::LoadEffects(const StaticString& path, SubmixVoice* category) {//TODO: Possibly accept SourceVoices also
	try {
		rapidjson::Document	doc;
		if (!rapidjson::LoadFrom(path, doc)) {
			assert(0);
			return 0;
		}

		if (!doc.IsArray()) {
			assert(0);
			return 0;
		}

		Byte	index = 0;
		UInt32	numFx = doc.Size();
		auto	fxInfoList = STACK_ALLOC(FXInfo*, numFx);
		auto	valueList = STACK_ALLOC(rapidjson::Value, numFx);

		FOREACH(fxIter, doc) {
			assert(fxIter->IsObject());
			auto& typeIter = fxIter->FindMember("Type");
			assert(index < numFx);

			fxInfoList[index] = &m_FxNameToInfoMap[typeIter->value.GetString()];
			valueList[index] = std::move(*fxIter);
			++index;
		}
		assert(numFx == index);

		verify(SetEffectDescriptors(category, fxInfoList, numFx));

		for(UInt32 ix = 0; ix < numFx; ++ix) {
			auto& fxInfo = fxInfoList[ix];
			fxInfo->Update(category, ix, valueList[ix]);
		}
		
		return numFx;//Return number of effects loaded
	}
	catch (...) {
		return 0;
	}
}