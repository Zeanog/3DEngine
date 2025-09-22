#pragma once

#include "AVoice.h"
#include <xaudio2.h>

template<typename TVoiceInterface>
class ASourceVoice : public AVoice {
	INHERITEDCLASS_TYPEDEFS(ASourceVoice, AVoice)

protected:
	TVoiceInterface* m_Voice{};

	ASourceVoice() {} //Hide constructor to make it like an abstract class

public:
	virtual void Destroy() override {
		::DestroyVoice(m_Voice);
	}

	virtual TVoiceInterface* Voice() {
		return m_Voice;
	}

	virtual const TVoiceInterface* Voice() const {
		return m_Voice;
	}

	virtual void Voice(TVoiceInterface* vi) {
		m_Voice = vi;
	}

	virtual UInt32	NumChannels() const = 0;

	virtual Float32 Volume() const {
		Float32 volume{};
		assert(m_Voice);
		m_Voice->GetVolume(&volume);
		return volume;
	}

	virtual Bool Volume(Float32 newVolume) const {
		assert(m_Voice);
		return SUCCEEDED(m_Voice->SetVolume(newVolume));
	}

	virtual Bool			Volume(Float32 newVolume, UInt32 operationSet) const {
		assert(m_Voice);
		return SUCCEEDED(m_Voice->SetVolume(newVolume, operationSet));
	}

	virtual Bool SetOutputVoices(const XAUDIO2_VOICE_SENDS* destVoices) {
		assert(m_Voice);
		return SUCCEEDED(m_Voice->SetOutputVoices(destVoices));
	}

	template <typename... Voices>
	Bool SetOutputTo(Voices... destVoices) {
		static constexpr UINT32 NumDescriptors = sizeof...(Voices);
		XAUDIO2_SEND_DESCRIPTOR descriptors[NumDescriptors];
		XAUDIO2_VOICE_SENDS sends{ NumDescriptors, descriptors };

		std::size_t i = 0;
		std::initializer_list<int>{
			(descriptors[i++] = { 0, destVoices->Voice() }, 0)...
		};

		return SetOutputVoices(&sends);
	}

	virtual Bool SetEffectChain(const XAUDIO2_EFFECT_CHAIN& chain) {
		assert(m_Voice);
		auto result = m_Voice->SetEffectChain(&chain);
		return SUCCEEDED(result);
	}

	virtual Bool SetEffectChain(const XAUDIO2_EFFECT_CHAIN* chain) {
		assert(m_Voice);
		auto result = m_Voice->SetEffectChain(chain);
		return SUCCEEDED(result);
	}

	template<typename... EffectDescriptors>
	Bool SetEffectDescriptors(EffectDescriptors... effectDescriptors) {
		static constexpr UINT32 NumDescriptors = sizeof...(EffectDescriptors);
		XAUDIO2_EFFECT_DESCRIPTOR descriptors[NumDescriptors]{ effectDescriptors... };
		XAUDIO2_EFFECT_CHAIN chain{ NumDescriptors, descriptors };

		return SetEffectChain(chain);
	}

	Bool SetEffectDescriptors(XAUDIO2_EFFECT_DESCRIPTOR* effectDescriptors, UInt32 count) {
		XAUDIO2_EFFECT_CHAIN chain{ count, effectDescriptors };
		return SetEffectChain(chain);
	}

	virtual Bool SetEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
		assert(m_Voice);
		return SUCCEEDED(m_Voice->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U));
	}

	virtual Bool SetEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		assert(m_Voice);
		return SUCCEEDED(m_Voice->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet));
	}

	Bool EnableEffect(UInt32 index) {
		assert(m_Voice);
		auto result = m_Voice->EnableEffect(index);
		return SUCCEEDED(result);
	}

	Bool EnableEffect(UInt32 index, UInt32 operationSet) {
		assert(m_Voice);
		auto result = m_Voice->EnableEffect(index, operationSet);
		return SUCCEEDED(result);
	}

	Bool DisableEffect(UInt32 index) {
		assert(m_Voice);
		auto result = m_Voice->DisableEffect(index);
		return SUCCEEDED(result);
	}

	Bool DisableEffect(UInt32 index, UInt32 operationSet) {
		assert(m_Voice);
		auto result = m_Voice->DisableEffect(index, operationSet);
		return SUCCEEDED(result);
	}
};