#pragma once

#include "AVoice.h"
#include <xaudio2.h>

template<typename TVoiceInterface>
class ASourceVoice : public AVoice {
	ABSTRACT_INHERITED_CLASS_TYPEDEFS(ASourceVoice, AVoice) {}

	DEFINE_MEMBER_EX(UInt64, FormatHash)

protected:
	TVoiceInterface* m_Voice{};

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

	virtual UInt16	NumChannels() const = 0;

	virtual Float32 Volume() const {
		Float32 volume{};
		assert(m_Voice);
		m_Voice->GetVolume(&volume);
		return volume;
	}

	virtual Bool Volume(Float32 newVolume) const {
		return Volume(newVolume, XAUDIO2_COMMIT_NOW);
	}

	virtual Bool			Volume(Float32 newVolume, UInt32 operationSet) const {
		assert(m_Voice);
		return SUCCEEDED(m_Voice->SetVolume(newVolume, operationSet));
	}

	virtual Bool SetOutputVoices(const XAUDIO2_VOICE_SENDS* destVoices) {
		assert(m_Voice);
		assert(destVoices);
		return SUCCEEDED(m_Voice->SetOutputVoices(destVoices));
	}

	template <typename... Voices>
	Bool SetOutputTo(Voices... destVoices) {
		static constexpr UINT32 NumDescriptors = sizeof...(Voices);
		XAUDIO2_SEND_DESCRIPTOR descriptors[max(1, NumDescriptors)];//Needed if we pass in no voices
		XAUDIO2_VOICE_SENDS sends{ NumDescriptors, descriptors };

		std::size_t i = 0;
		((descriptors[i++] = XAUDIO2_SEND_DESCRIPTOR{ 0, destVoices->Voice() }), ...);

		return SetOutputVoices(&sends);
	}

	virtual Bool SetEffectChain(const XAUDIO2_EFFECT_CHAIN& chain) {
		assert(m_Voice);
		auto result = m_Voice->SetEffectChain(&chain);
		assert(SUCCEEDED(result));
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
		return SetEffectParameters(index, parameterData, parameterDataByteSize, XAUDIO2_COMMIT_NOW);
	}

	virtual Bool SetEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		assert(m_Voice);
		const Char* msg{};

		auto result = m_Voice->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet);
		/*if (FAILED(result)) {
			msg = GetErrorMessage(result);
		}*/
		return SUCCEEDED(result);
	}

	virtual Bool GetEffectParameters(UInt32 index, void* outParameterData, UInt32& outParameterDataByteSize) {
		assert(m_Voice);
		return SUCCEEDED(m_Voice->GetEffectParameters(index, outParameterData, outParameterDataByteSize));
	}

	Bool			IsEffectEnabled(UInt32 index) {
		assert(m_Voice);
		BOOL	enabled{};
		m_Voice->GetEffectState(index, &enabled);
		return (Bool)enabled;
	}

	Bool EnableEffect(UInt32 index) {
		return EnableEffect(index, XAUDIO2_COMMIT_NOW);
	}

	Bool EnableEffect(UInt32 index, UInt32 operationSet) {
		assert(m_Voice);
		auto result = m_Voice->EnableEffect(index, operationSet);
		return SUCCEEDED(result);
	}

	Bool DisableEffect(UInt32 index) {
		return DisableEffect(index, XAUDIO2_COMMIT_NOW);
	}

	Bool DisableEffect(UInt32 index, UInt32 operationSet) {
		assert(m_Voice);
		auto result = m_Voice->DisableEffect(index, operationSet);
		return SUCCEEDED(result);
	}
};