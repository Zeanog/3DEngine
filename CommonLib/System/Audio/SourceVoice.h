#pragma once

#include "AVoice.h"
#include "SourceVoiceCallbacks.h"
#include "System\Functors\MultiFunctor.h"

#include <xaudio2.h>
#include <array>

class Sound;

class SourceVoice : public AVoice {
	INHERITEDCLASS_TYPEDEFS(SourceVoice, AVoice)

	friend class AudioSystem;

protected:
	WAVEFORMATEX		m_Format;//TODO:  Is this needed here??

	IXAudio2SourceVoice* m_Voice{};

	class SourceVoiceCallbacks* m_Callbacks{};

	virtual void		Destroy();

public:
	SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format);

	template<typename... Voices>
	SourceVoice(IXAudio2* audio, const WAVEFORMATEX& format, Voices... destVoices) {
		m_Format = format;

		//Wrap this in a macro or something
		std::array<XAUDIO2_SEND_DESCRIPTOR, sizeof...(Voices)> descriptors;
		XAUDIO2_VOICE_SENDS sends{ descriptors.size(), descriptors.data() };

		std::size_t i = 0;
		(void)std::initializer_list<int>{
			(descriptors[i++] = { 0, destVoices }, 0)...
		};
		//Wrap this in a macro or something

		m_Callbacks = new SourceVoiceCallbacks(this);
		HRESULT hr = audio->CreateSourceVoice(&m_Voice, &m_Format, 0, 2.0f, m_Callbacks, &sends);
		assert(SUCCEEDED(hr));
	}

	virtual Bool	Submit(const Sound& sound);
	virtual Bool	Submit(const Sound* sound);
	virtual Bool	Start(const Sound& sound);
	virtual Bool	Start(const Sound* sound);
	virtual Bool	Start();

	virtual Bool	Start(const Sound& sound, UInt32 operationSet);
	virtual Bool	Start(const Sound* sound, UInt32 operationSet);
	virtual Bool	Start(UInt32 operationSet);

	Float32			Volume() const;
	Bool			Volume(Float32 newVolume) const;

	void			SetFrequencyRatio(Float32 ratio);
	void			SetFrequencyRatio(Float32 ratio, UInt32 operationSet);
	Float32			GetFrequencyRatio() const;

	Bool			SetOutputVoices(const XAUDIO2_VOICE_SENDS* destVoices);

public:
	MultiFunctor<TYPELIST_1(UINT32)>	OnVoiceProcessingPassStart;
	MultiFunctor<TNull>					OnVoiceProcessingPassEnd;
	MultiFunctor<TNull>					OnStreamEnd;
	MultiFunctor<TYPELIST_1(void*)>		OnBufferStart;
	MultiFunctor<TYPELIST_1(void*)>		OnBufferEnd;
	MultiFunctor<TYPELIST_1(void*)>		OnLoopEnd;
	MultiFunctor<TYPELIST_2(void*, HRESULT)> OnVoiceError;
};