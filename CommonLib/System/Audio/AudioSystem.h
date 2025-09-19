#pragma once

#include "System\Typedefs.h"
#include "System/Singleton.h"
#include "System\List.h"
#include "System\Map.h"
#include "SubmixVoice.h"
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

	Map<const SubmixVoice*, Map<UINT64, List<SourceVoice*>>>	m_SoundCategoryMap{};

	SubmixVoice* m_MusicMixVoice{};
	SubmixVoice* m_FxMixVoice{};

protected:
	MasteringVoice* CreateMasteringVoice();

	static UINT64		GenerateHash(const WAVEFORMATEX& format);
	static UINT64		GenerateHash(UInt32 numChannels, UInt32 sampleRate);

	Bool FindSourceVoice(SubmixVoice* voiceCategory, const WAVEFORMATEX& format, SourceVoice*& outVoice);
	Bool AddSourceVoice(SubmixVoice* voiceCategory, SourceVoice* voice);

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

	SourceVoice* PlayFx(const Sound& snd);
	SourceVoice* PlayMusic(const Sound& snd);

	SourceVoice* PlayFx(const Sound* snd);
	SourceVoice* PlayMusic(const Sound* snd);

	//XAUDIO2_EFFECT_DESCRIPTOR{ pReverbEffect , true, fxMixVoice->NumChannels() }
	template<typename... Effects>
	Bool AddFxEffectDescriptors(Effects... effects) {
		static constexpr UINT32 NumDescriptors = sizeof...(Effects);
		XAUDIO2_EFFECT_DESCRIPTOR descriptors[NumDescriptors];

		std::size_t i = 0;
		std::initializer_list<int>{
			(descriptors[i++] = { effects, true, m_FxMixVoice->NumChannels() }, 0)...
		};

		return m_FxMixVoice->SetEffectDescriptors(descriptors, NumDescriptors);
	}

	/*template<typename... EffectDescriptors>
	Bool SetFxEffectDescriptors(EffectDescriptors... effectDescriptors) {
		return m_FxMixVoice->SetEffectDescriptors(effectDescriptors...);
	}*/

	template<typename... Effects>
	Bool AddMusicEffectDescriptors(Effects... effects) {
		static constexpr UINT32 NumDescriptors = sizeof...(Effects);
		XAUDIO2_EFFECT_DESCRIPTOR descriptors[NumDescriptors];

		std::size_t i = 0;
		std::initializer_list<int>{
			(descriptors[i++] = { effects, true, m_MusicMixVoice->NumChannels() }, 0)...
		};

		return m_MusicMixVoice->SetEffectDescriptors(&descriptors);
	}

	virtual Bool	EnableFxEffect(UInt32 index) {
		return m_FxMixVoice->EnableEffect(index);
	}

	virtual Bool	EnableMusicEffect(UInt32 index) {
		return m_MusicMixVoice->EnableEffect(index);
	}

	virtual Bool SetFxEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
		return m_FxMixVoice->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U);
	}

	virtual Bool SetFxEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		return m_FxMixVoice->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet);
	}

	virtual Bool SetMusicEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
		return m_MusicMixVoice->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U);
	}

	virtual Bool SetMusicEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		return m_MusicMixVoice->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet);
	}
};