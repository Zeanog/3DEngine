#pragma once

#include "System\Typedefs.h"
#include "ASourceVoice.h"
#include "SourceVoiceCallbacks.h"
#include "System\Functors\MultiFunctor.h"

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
		return Stop(XAUDIO2_COMMIT_NOW);
	}

	virtual Bool	Stop( UInt32 operationSet ) {
		return SUCCEEDED(m_Voice->Stop(0, operationSet));
	}

	Bool	FlushBuffers() {
		return SUCCEEDED(m_Voice->FlushSourceBuffers());
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

	virtual UInt64 SamplesPlayed() const {
		assert(m_Voice);
		XAUDIO2_VOICE_STATE state;
		m_Voice->GetState(&state);
		return state.SamplesPlayed;
	}

	virtual Bool IsPlaying() const {
		assert(m_Voice);
		XAUDIO2_VOICE_STATE state;
		m_Voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
		return state.BuffersQueued > 0;
	}

	virtual const Sound* PlayingSound() const {
		assert(m_Voice);
		XAUDIO2_VOICE_STATE state;
		m_Voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
		if (state.BuffersQueued <= 0) {
			return nullptr;
		}
		//This is assuming we set a sounds context to itself
		return (const Sound*)state.pCurrentBufferContext;
	}

	virtual Sound* PlayingSound() {
		assert(m_Voice);
		XAUDIO2_VOICE_STATE state;
		m_Voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
		if (state.BuffersQueued <= 0) {
			return nullptr;
		}
		//This is assuming we set a sounds context to itself
		return (Sound*)state.pCurrentBufferContext;
	}

public:
	Delegate<TYPELIST_2(SourceVoice*, UINT32)>			OnVoiceProcessingPassStart;
	Delegate<TYPELIST_1(SourceVoice*)>					OnVoiceProcessingPassEnd;
	Delegate<TYPELIST_1(SourceVoice*)>					OnStreamEnd;
	Delegate<TYPELIST_2(SourceVoice*, void*)>			OnBufferStart;
	Delegate<TYPELIST_2(SourceVoice*, void*)>			OnBufferEnd;
	Delegate<TYPELIST_2(SourceVoice*, void*)>			OnLoopEnd;
	Delegate<TYPELIST_3(SourceVoice*, void*, HRESULT)>	OnVoiceError;
};