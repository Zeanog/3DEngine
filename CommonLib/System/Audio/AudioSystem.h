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

	typedef LinkedList<SourceVoice*>	TVoiceList;
	typedef Map<UInt64, TVoiceList>		TFormatToVoiceList;

protected:
	IXAudio2* m_Audio2{};

	MasteringVoice* m_MasteringVoice{};

	List<AVoice*>	m_Voices{};

	Map<SubmixVoice*, TFormatToVoiceList>	m_CategoryToVoiceListMap{};
	Map<SourceVoice*, SubmixVoice*>			m_VoiceToCategoryMap{};

	Map<StaticString, SubmixVoice*>			m_CategoryNameToVoiceMap{};
	Map<SubmixVoice*, StaticString>			m_VoiceToCategoryNameMap{};

	static std::mutex						m_Mutex;

protected:
	MasteringVoice* CreateMasteringVoice();

	static UINT64		GenerateHash(const WAVEFORMATEX& format);
	static UINT64		GenerateHash(UInt32 numChannels, UInt32 sampleRate);

	Bool				FindSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format, SourceVoice*& outVoice) const;
	Bool				AddSourceVoice(SubmixVoice* voiceCategory, SourceVoice* voice);
	void				OnBufferStartHandler(SourceVoice* voice, void* context);
	void				OnBufferEndHandler(SourceVoice* voice, void* context);

	SubmixVoice* GetCategory(const StaticString& name) const;
	const StaticString& GetCategoryName(SubmixVoice* categoryVoice) const;

	SourceVoice* GetSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format);
	SourceVoice* GetSourceVoice(SubmixVoice* voiceCategory, const Sound* sound);
	SourceVoice* GetSourceVoice(SubmixVoice* voiceCategory, const Sound& sound);

	SubmixVoice* CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate);

	Bool		CommitChanges(UInt32 operationSet) {
		return SUCCEEDED(m_Audio2->CommitChanges(operationSet));
	}

public:
	virtual Bool	Init();
	virtual void	Destroy();

	Bool		AddCategory(const StaticString& name, UInt32 numChannels, UInt32 sampleRate);

	SourceVoice* Play(const StaticString& categoryName, const Sound& snd);
	SourceVoice* Play(const StaticString& categoryName, const Sound* snd);
	Float32		 Play(const StaticString& categoryName, const StaticString& filePath, SourceVoice*& outVoice);
	Float32		 Submit(const StaticString& categoryName, const StaticString& filePath, SourceVoice*& outVoice);

	void StopAllVoices();

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

	Bool AddEffectDescriptors(const StaticString& categoryName, UInt32 numDescriptors);

	virtual Bool	EnableEffect(const StaticString& categoryName, UInt32 index) {
		return GetCategory(categoryName)->EnableEffect(index);
	}

	virtual Bool	EnableEffect(const StaticString& categoryName, UInt32 index, UInt32 operationSet) {
		return GetCategory(categoryName)->EnableEffect(index, operationSet);
	}

	template<typename TParameters>
	Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const TParameters& params) {
		return GetCategory(categoryName)->SetEffectParameters(index, &params, params.Sizeof(), 0U);
	}

	template<typename TParameters>
	Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const TParameters& params, UInt32 operationSet) {
		return GetCategory(categoryName)->SetEffectParameters(index, &params, params.Sizeof(), operationSet);
	}

	template<typename TParameters>
	Bool GetEffectParameters(const StaticString& categoryName, UInt32 index, TParameters& params) {
		UInt32 outSize{};
		auto result = GetCategory(categoryName)->GetEffectParameters(index, &params, outSize);
		assert(params.SizeOf() == outSize);
		return result;
	}

	virtual Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
		return GetCategory(categoryName)->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U);
	}

	virtual Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		return GetCategory(categoryName)->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet);
	}

	void ReloadAssets();
};