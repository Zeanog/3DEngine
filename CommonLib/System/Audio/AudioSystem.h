#pragma once

#include "System\Typedefs.h"
#include "System/Singleton.h"
#include "System\List.h"
#include "System\LinkedList.h"
#include "System\Map.h"

#include <mutex>
#include <xaudio2.h>


class AVoice;
class MasteringVoice;
class SourceVoice;
class SubmixVoice;
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

	struct FXInfo {
		Functor<IUnknown*>															CreateFX;
		Functor<Bool, TYPELIST_3(SubmixVoice*, UInt32, const rapidjson::Value&)>	Update;
	};
	Map<StaticString, FXInfo>				m_FxNameToInfoMap;

	static std::mutex						m_Mutex;

protected:
	MasteringVoice*		CreateMasteringVoice();

	static UINT64		GenerateHash(const WAVEFORMATEX& format);
	static UINT64		GenerateHash(UInt32 numChannels, UInt32 sampleRate);

	Bool				FindSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format, SourceVoice*& outVoice) const;
	Bool				AddSourceVoice(SubmixVoice* voiceCategory, SourceVoice* voice);

	void				OnVoiceErrorHandler(SourceVoice* voice, void* context, HRESULT result);
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

	SubmixVoice*		AddCategory(const StaticString& name, UInt32 numChannels, UInt32 sampleRate);

	SourceVoice* Play(const Sound& snd, const StaticString& categoryName);
	SourceVoice* Play(const Sound* snd, const StaticString& categoryName);
	Float32		 Play(const StaticString& filePath, const StaticString& categoryName, SourceVoice*& outVoice);
	Float32		 Submit(const StaticString& filePath, const StaticString& categoryName, SourceVoice*& outVoice);

	void StopAllVoices();

	Bool SetEffectDescriptors(const StaticString& categoryName, const Char** fxNames, UInt32 numFx);
	Bool SetEffectDescriptors(SubmixVoice* category, const Char** fxNames, UInt32 numFx);

	/*Bool SetEffectDescriptors(const StaticString& categoryName, const List<const Char*>& fxNames) {
		XAUDIO2_EFFECT_DESCRIPTOR* descriptors = STACK_ALLOC(XAUDIO2_EFFECT_DESCRIPTOR, fxNames.Length());

		auto category = GetCategory(categoryName);

		IUnknown* pEffect{};
		for (UInt32 ix = 0; ix < fxNames.Length(); ++ix) {
			auto& info = m_FxNameToInfoMap[fxNames[ix]];
			verify(SUCCEEDED(CreateFX(info.Guid, &pEffect)));
			descriptors[ix] = { pEffect, true, category->NumChannels() };
		}

		return category->SetEffectDescriptors(descriptors, fxNames.Length());
	}*/

	/*template<typename... FXGUIDS>
	Bool SetEffectDescriptors(const StaticString& categoryName, const FXGUIDS&... fxGuids) {
		static constexpr UINT32 NumDescriptors = sizeof...(FXGUIDS);
		XAUDIO2_EFFECT_DESCRIPTOR effectDescriptors[NumDescriptors];
		IUnknown* pEffect{};

		auto category = GetCategory(categoryName);

		std::size_t i = 0;
		std::initializer_list<int>{(
				verify(SUCCEEDED(CreateFX(fxGuids, &pEffect))),
				effectDescriptors[i++] = {pEffect, true, category->NumChannels()}
			, 0)...
		};

		verify(category->SetEffectDescriptors(effectDescriptors, NumDescriptors));

		i = 0;
		std::initializer_list<int>{
			(fxGuids, effectDescriptors[i++].pEffect->Release(), 0)...
		};

		return true;
	}*/

	Bool			IsEffectEnabled(const StaticString& categoryName, UInt32 index);

	virtual Bool	EnableEffect(const StaticString& categoryName, UInt32 index);

	virtual Bool	EnableEffect(const StaticString& categoryName, UInt32 index, UInt32 operationSet);

	virtual Bool	DisableEffect(const StaticString& categoryName, UInt32 index);

	virtual Bool	DisableEffect(const StaticString& categoryName, UInt32 index, UInt32 operationSet);

	template<typename TParameters>
	Bool GetEffectParameters(const StaticString& categoryName, UInt32 index, TParameters& params) {
		return GetEffectParameters(GetCategory(categoryName), index, params);
	}

	template<typename TParameters>
	Bool GetEffectParameters(SubmixVoice* category, UInt32 index, TParameters& params) {
		UInt32 outSize{};
		assert(category);
		auto result = category->GetEffectParameters(index, &params, outSize);
		assert(params.SizeOf() == outSize);
		return result;
	}

	Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize);
	Bool SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet);

	void ReloadAssets();

	UInt32	LoadEffects(const StaticString& path, const StaticString& categoryName);
	UInt32	LoadEffects(const StaticString& path, SubmixVoice* category);
};