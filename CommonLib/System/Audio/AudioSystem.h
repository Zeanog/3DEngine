#pragma once

#include "System\Typedefs.h"
#include "System/Singleton.h"
#include "System\List.h"
#include "System\LinkedList.h"
#include "System\Map.h"
#include "SubmixVoice.h"

#include <mutex>
#include <xaudio2.h>

class AVoice;
class MasteringVoice;
class SourceVoice;
class Sound;

class AudioSystem {
	CLASS_TYPEDEFS(AudioSystem)
	SINGLETON_DECLARATIONS(AudioSystem) {
	}

protected:
	IXAudio2*		m_Audio2{};

	MasteringVoice* m_MasteringVoice{};

	List<AVoice*>	m_Voices{};

	Map<SubmixVoice*, Map<UINT64, LinkedList<SourceVoice*>>>	m_CategoryToVoiceListMap{};
	Map<SourceVoice*, SubmixVoice*>								m_VoiceToCategoryMap{};

	Map<StaticString, SubmixVoice*>								m_CategoryNameToVoiceMap{};
	Map<SubmixVoice*, StaticString>								m_VoiceToCategoryNameMap{};

	static std::mutex							m_Mutex;

protected:
	MasteringVoice* CreateMasteringVoice();

	static UINT64		GenerateHash(const WAVEFORMATEX& format);
	static UINT64		GenerateHash(UInt32 numChannels, UInt32 sampleRate);

	Bool FindSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format, SourceVoice*& outVoice);
	Bool AddSourceVoice(SubmixVoice* voiceCategory, SourceVoice* voice);
	void OnBufferEndHandler(SourceVoice* voice, void* v);

	SubmixVoice* GetCategory(const StaticString& name) const;
	const StaticString& GetCategoryName(SubmixVoice* categoryVoice) const;

	SourceVoice* CreateSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format);
	SourceVoice* CreateSourceVoice(SubmixVoice* voiceCategory, const Sound* sound);
	SourceVoice* CreateSourceVoice(SubmixVoice* voiceCategory, const Sound& sound);

	SubmixVoice* CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate);
	Bool	CommitChanges(UInt32 operationSet) {
		return SUCCEEDED(m_Audio2->CommitChanges(operationSet));
	}

public:
	virtual Bool	Init();
	virtual void	Release();

	Bool		AddCategory(const StaticString& name, UInt32 numChannels, UInt32 sampleRate);

	SourceVoice* Play(const StaticString& categoryName, const Sound& snd);
	SourceVoice* Play(const StaticString& categoryName, const Sound* snd);
	SourceVoice* Play(const StaticString& categoryName, const StaticString& filePath, Sound*& outSnd);
	
	template<typename... Effects>
	Bool AddEffectDescriptors(const StaticString& categoryName, Effects... effects) {
		static constexpr UINT32 NumDescriptors = sizeof...(Effects);
		XAUDIO2_EFFECT_DESCRIPTOR descriptors[NumDescriptors];

		auto category = GetCategory(categoryName);

		std::size_t i = 0;
		std::initializer_list<int>{
			(descriptors[i++] = { effects, true, category->NumChannels() }, 0)...
		};

		return category->SetEffectDescriptors(descriptors, NumDescriptors);
	}

	virtual Bool	EnableEffect(const StaticString& categoryName, UInt32 index) {
		return GetCategory(categoryName)->EnableEffect(index);
	}

	virtual Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
		return GetCategory(categoryName)->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U);
	}

	virtual Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		return GetCategory(categoryName)->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet);
	}
};