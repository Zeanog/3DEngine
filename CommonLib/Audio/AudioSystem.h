#pragma once

#include "System\Typedefs.h"
#include "System/Singleton.h"
#include "System\List.h"
#include "System\LinkedList.h"
#include "System\Map.h"

#include <mutex>
#include <xaudio2.h>
#include <rapidjson\document.h>

class AVoice;
class MasteringVoice;
class SourceVoice;
class SubmixVoice;
class Sound;

class AudioSystem {
	CLASS_TYPEDEFS(AudioSystem)
	SINGLETON_DECLARATIONS(AudioSystem) {}

	//TODO: Possibly have a cap on number of source voices per format
	typedef LinkedList<SourceVoice*>	TVoiceList;
	typedef Map<UInt64, TVoiceList>		TFormatToVoiceListMap;

protected:
	IXAudio2* m_Audio2{};

	MasteringVoice* m_MasteringVoice{};

	List<AVoice*>	m_Voices{};

	TFormatToVoiceListMap					m_FormatToVoiceListMap{};

	Map<StaticString, SubmixVoice*>			m_CategoryNameToVoiceMap{};
	Map<SubmixVoice*, StaticString>			m_VoiceToCategoryNameMap{};

	struct FXInfo {
		Functor<IUnknown*>															Create;
		Functor<Bool, TYPELIST_3(SubmixVoice*, UInt32, const rapidjson::Value&)>	Update;
	};
	Map<StaticString, FXInfo>				m_FxNameToInfoMap{};

	static std::mutex						m_Mutex;

protected:
	MasteringVoice*		CreateMasteringVoice();

	Bool				FindOpenSourceVoice(const WAVEFORMATEX& format, SourceVoice*& outVoice) const;
	Bool				FindOpenSourceVoice(UInt64 formatHash, SourceVoice*& outVoice) const;
	Bool				AddSourceVoice(SourceVoice* voice);

	void				OnVoiceErrorHandler(SourceVoice* voice, void* context, HRESULT result);
	void				OnBufferStartHandler(SourceVoice* voice, void* context);
	void				OnBufferEndHandler(SourceVoice* voice, void* context);

	SubmixVoice*		GetCategoryVoice(const StaticString& name) const;
	const StaticString& GetCategoryName(SubmixVoice* categoryVoice) const;

	SourceVoice*		GetSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format);
	SourceVoice*		GetSourceVoice(SubmixVoice* voiceCategory, const Sound* sound);
	SourceVoice*		GetSourceVoice(SubmixVoice* voiceCategory, const Sound& sound);

	SubmixVoice*		CreateSubmixVoice(UInt16 numChannels, UInt16 sampleRate);

	Bool				CommitChanges(UInt32 operationSet) {
		return SUCCEEDED(m_Audio2->CommitChanges(operationSet));
	}

public:
	virtual Bool	Init();
	virtual void	Destroy();

	static UInt64	GenerateHash(const WAVEFORMATEX& format);
	static UInt64	GenerateHash(UInt16 numChannels, UInt16 sampleRate);

	SubmixVoice* AddCategory(const StaticString& name, UInt16 numChannels, UInt16 sampleRate);

	SourceVoice* Play(const Sound& snd, const StaticString& categoryName);
	SourceVoice* Play(const Sound* snd, const StaticString& categoryName);
	Float32		 Play(const Char* filePath, const StaticString& categoryName, SourceVoice*& outVoice) {
		return Play(StaticString(filePath), categoryName, outVoice);
	}
	Float32		 Submit(const Char* filePath, const StaticString& categoryName, SourceVoice*& outVoice) {
		return Submit(StaticString(filePath), categoryName, outVoice);
	}
	Float32		 Play(const StaticString& filePath, const StaticString& categoryName, SourceVoice*& outVoice);
	Float32		 Submit(const StaticString& filePath, const StaticString& categoryName, SourceVoice*& outVoice);

	void		StopAllVoices();

	Bool		SetEffectDescriptors(const StaticString& categoryName, const Char** fxNames, UInt32 numFx);
	Bool		SetEffectDescriptors(SubmixVoice* category, const Char** fxNames, UInt32 numFx);
	Bool		SetEffectDescriptors(SubmixVoice* category, FXInfo** fxNames, UInt32 numFx);

	Bool			IsEffectEnabled(const StaticString& categoryName, UInt32 index);

	virtual Bool	EnableEffect(const StaticString& categoryName, UInt32 index);
	virtual Bool	EnableEffect(const StaticString& categoryName, UInt32 index, UInt32 operationSet);

	virtual Bool	DisableEffect(const StaticString& categoryName, UInt32 index);
	virtual Bool	DisableEffect(const StaticString& categoryName, UInt32 index, UInt32 operationSet);

	template<typename TParameters>
	Bool	GetEffectParameters(const StaticString& categoryName, UInt32 index, TParameters& params) {
		return GetEffectParameters(GetCategoryVoice(categoryName), index, params);
	}

	template<typename TVoice, typename TParameters>
	Bool	GetEffectParameters(TVoice* voice, UInt32 index, TParameters& outParams) {
		UInt32 outSize{};
		assert(voice);
		auto result = voice->GetEffectParameters(index, &outParams, outSize);
		assert(outParams.SizeOf() == outSize);
		return result;
	}

	Bool	SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize);
	Bool	SetEffectParameters(const StaticString& categoryName, UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet);

	void	ReloadAssets();

	UInt32	LoadEffectsChain(const StaticString& path, const StaticString& categoryName);
	UInt32	LoadEffectsChain(const StaticString& path, SubmixVoice* category);
};