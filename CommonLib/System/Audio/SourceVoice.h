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
	class SourceVoiceCallbacks* m_Callbacks{};

	virtual void		Destroy() override;

public:
	SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format);

	DEFINE_MEMBER_EX(WAVEFORMATEX, Format)

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

	virtual Bool	Stop() {
		return Stop(0);
	}

	virtual Bool	Stop( UInt32 operationSet ) {
		return SUCCEEDED(m_Voice->Stop(0, operationSet));
	}

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
		assert(m_Voice);
		XAUDIO2_VOICE_STATE state;
		m_Voice->GetState(&state);
		return state.BuffersQueued > 0;
	}

public:
	Delegate<TYPELIST_1(UINT32)>						OnVoiceProcessingPassStart;
	Delegate<>											OnVoiceProcessingPassEnd;
	Delegate<>											OnStreamEnd;
	Delegate<TYPELIST_2(SourceVoice*, void*)>			OnBufferStart;
	Delegate<TYPELIST_2(SourceVoice*, void*)>			OnBufferEnd;
	Delegate<TYPELIST_2(SourceVoice*, void*)>			OnLoopEnd;
	Delegate<TYPELIST_3(SourceVoice*, void*, HRESULT)>	OnVoiceError;
};