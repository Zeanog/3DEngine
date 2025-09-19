#pragma once

#include "..\Typedefs.h"
#include "ASourceVoice.h"
#include "SourceVoiceCallbacks.h"
#include "System\Functors\MultiFunctor.h"
#include <array>

class Sound;

class SourceVoice : public ASourceVoice<IXAudio2SourceVoice>{
	INHERITEDCLASS_TYPEDEFS(SourceVoice, ASourceVoice<IXAudio2SourceVoice>)

	friend class AudioSystem;

protected:
	WAVEFORMATEX		m_Format;

	class SourceVoiceCallbacks* m_Callbacks{};

	virtual void		Destroy() override;

public:
	SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format);

	DECLARE_GETSET(Format)

	virtual UInt32	NumChannels() const override {
		return m_Format.nChannels;
	}

	virtual Bool	Submit(const Sound& sound);
	virtual Bool	Submit(const Sound* sound);
	virtual Bool	Start(const Sound& sound);
	virtual Bool	Start(const Sound* sound);
	virtual Bool	Start();

	virtual Bool	Start(const Sound& sound, UInt32 operationSet);
	virtual Bool	Start(const Sound* sound, UInt32 operationSet);
	virtual Bool	Start(UInt32 operationSet);

	virtual void SetFrequencyRatio(Float32 ratio) {
		verify(SUCCEEDED(m_Voice->SetFrequencyRatio(ratio)));
	}

	virtual void SetFrequencyRatio(Float32 ratio, UInt32 operationSet) {
		verify(SUCCEEDED(m_Voice->SetFrequencyRatio(ratio, operationSet)));
	}

	virtual Float32 GetFrequencyRatio() const {
		Float32 ratio;
		m_Voice->GetFrequencyRatio(&ratio);
		return ratio;
	}

	virtual Bool IsPlaying() const {
		XAUDIO2_VOICE_STATE state;
		m_Voice->GetState(&state);
		return state.BuffersQueued > 0;
	}

public:
	Delegate<TYPELIST_1(UINT32)>			OnVoiceProcessingPassStart;
	Delegate<>								OnVoiceProcessingPassEnd;
	Delegate<>								OnStreamEnd;
	Delegate<TYPELIST_1(void*)>				OnBufferStart;
	Delegate<TYPELIST_1(void*)>				OnBufferEnd;
	Delegate<TYPELIST_1(void*)>				OnLoopEnd;
	Delegate<TYPELIST_2(void*, HRESULT)>	OnVoiceError;
};