#pragma once

#include "AVoice.h"
#include <xaudio2.h>

template<typename TVoiceInterface>
class ASourceVoice : public AVoice {
	INHERITEDCLASS_TYPEDEFS(ASourceVoice, AVoice)

protected:
	TVoiceInterface* m_Voice;

	ASourceVoice() {} //Hide constructor to make it like an abstract class

public:
	virtual void Destroy() override {
		::DestroyVoice(m_Voice);
	}

	TVoiceInterface* Voice() {
		return m_Voice;
	}

	const TVoiceInterface* Voice() const {
		return m_Voice;
	}

	void Voice(TVoiceInterface* vi) {
		m_Voice = vi;
	}

	virtual Float32 Volume() const {
		Float32 volume{};
		m_Voice->GetVolume(&volume);
		return volume;
	}

	virtual Bool Volume(Float32 newVolume) const {
		return SUCCEEDED(m_Voice->SetVolume(newVolume));
	}

	virtual Bool			Volume(Float32 newVolume, UInt32 operationSet) const {
		return SUCCEEDED(m_Voice->SetVolume(newVolume, operationSet));
	}

	virtual Bool SetOutputVoices(const XAUDIO2_VOICE_SENDS* destVoices) {
		return SUCCEEDED(m_Voice->SetOutputVoices(destVoices));
	}

	template <typename... Voices>
	void SetOutputTo(Voices... destVoices) {
		static constexpr UINT32 NumDescriptors = sizeof...(Voices);
		XAUDIO2_SEND_DESCRIPTOR descriptors[NumDescriptors];
		XAUDIO2_VOICE_SENDS sends{ NumDescriptors, descriptors };

		std::size_t i = 0;
		(void)std::initializer_list<int>{
			(descriptors[i++] = { 0, destVoices }, 0)...
		};

		SetOutputVoices(&sends);
	}

	virtual Bool SetEffectChain(const XAUDIO2_EFFECT_CHAIN* chain) {
		return SUCCEEDED(m_Voice->SetEffectChain(chain));
	}

	template<typename... EffectDescripts>
	void SetEffectDescriptors(EffectDescripts... effectDescripts) {
		static constexpr UINT32 NumDescriptors = sizeof...(EffectDescripts);
		XAUDIO2_EFFECT_DESCRIPTOR descriptors[NumDescriptors]{ effectDescripts... };
		XAUDIO2_EFFECT_CHAIN chain{ NumDescriptors, descriptors };

		SetEffectChain(&chain);
	}

	Bool EnableEffect(UInt32 index) {
		return SUCCEEDED( m_Voice->EnableEffect(index) );
	}

	Bool EnableEffect(UInt32 index, UInt32 operationSet) {
		return SUCCEEDED( m_Voice->EnableEffect(index, operationSet) );
	}

	Bool SetEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
		return SUCCEEDED(m_Voice->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U));
	}

	Bool SetEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		return SUCCEEDED(m_Voice->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet));
	}
};