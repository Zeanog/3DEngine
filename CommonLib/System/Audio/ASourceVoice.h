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
	Bool SetOutputTo(Voices... destVoices) {
		static constexpr UINT32 NumDescriptors = sizeof...(Voices);
		XAUDIO2_SEND_DESCRIPTOR descriptors[NumDescriptors];
		XAUDIO2_VOICE_SENDS sends{ NumDescriptors, descriptors };

		std::size_t i = 0;
		(void)std::initializer_list<int>{
			(descriptors[i++] = { 0, destVoices }, 0)...
		};

		return SetOutputVoices(&sends);
	}

	virtual Bool SetEffectChain(const XAUDIO2_EFFECT_CHAIN& chain) {
		auto result = m_Voice->SetEffectChain(&chain);
		if (FAILED(result)) {
			return SUCCEEDED(GetLastError());
		}
		return SUCCEEDED(result);
	}

	virtual Bool SetEffectChain(const XAUDIO2_EFFECT_CHAIN* chain) {
		auto result = m_Voice->SetEffectChain(chain);
		if (FAILED(result)) {
			return SUCCEEDED(GetLastError());
		}
		return SUCCEEDED(result);
	}

	template<typename... EffectDescripts>
	Bool SetEffectDescriptors(EffectDescripts... effectDescripts) {
		static constexpr UINT32 NumDescriptors = sizeof...(EffectDescripts);
		XAUDIO2_EFFECT_DESCRIPTOR descriptors[NumDescriptors]{ effectDescripts... };
		XAUDIO2_EFFECT_CHAIN chain{ NumDescriptors, descriptors };

		return SetEffectChain(chain);
	}

	Bool EnableEffect(UInt32 index) {
		auto result = m_Voice->EnableEffect(index);
		return SUCCEEDED( result );
	}

	Bool EnableEffect(UInt32 index, UInt32 operationSet) {
		auto result = m_Voice->EnableEffect(index, operationSet);
		return SUCCEEDED( result );
	}

	/*
	IUnknown* pReverbAPO = nullptr;
	verify(SUCCEEDED(XAudio2CreateReverb(&pReverbAPO)));

	// Define the effect descriptor
	XAUDIO2_EFFECT_DESCRIPTOR effectDesc = { 0 };
	effectDesc.InitialState = TRUE;  // Start enabled
	effectDesc.OutputChannels = numChannels;  // must match what voice can output or chain requirement
	effectDesc.pEffect = pReverbAPO;

	// Define the effect chain
	XAUDIO2_EFFECT_CHAIN effectChain = { 0 };
	effectChain.EffectCount = 1;
	effectChain.pEffectDescriptors = &effectDesc;

	// Attach the effect chain to the source voice
	verify(SUCCEEDED(newVoice->Voice()->SetEffectChain(&effectChain)));
	pReverbAPO->Release();

	XAUDIO2FX_REVERB_PARAMETERS reverbParams = {};
	// Start with default constants (macros) or adjust them
	reverbParams.WetDryMix = 60.0f;//XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;       // how much reverb vs dry
	//reverbParams.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY; // delay for first reflections
	//reverbParams.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;      // delay before the reverb tail
	//reverbParams.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
	//reverbParams.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	//reverbParams.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	//reverbParams.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	//reverbParams.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	//reverbParams.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
	//reverbParams.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
	//reverbParams.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
	//reverbParams.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
	//reverbParams.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
	//reverbParams.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
	//reverbParams.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
	//reverbParams.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
	//reverbParams.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
	//reverbParams.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
	reverbParams.ReflectionsGain = -5.0f;//XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
	reverbParams.ReverbGain = -5.0f;//XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
	reverbParams.DecayTime = 0.7f;//XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
	reverbParams.RoomSize = 10.0f;//XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
	reverbParams.DisableLateField = FALSE;  // enable late reverb tail

	// Set the parameters on voice, effect index 0 since that's where reverb is
	verify(newVoice->SetEffectParameters(0, &reverbParams, sizeof(reverbParams)));

	// If needed, ensure effect is enabled (it was set to InitialState=TRUE above)
	verify(newVoice->EnableEffect(0));
	*/

	virtual Bool SetEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize) {
		return SUCCEEDED(m_Voice->SetEffectParameters(index, parameterData, parameterDataByteSize, 0U));
	}

	virtual Bool SetEffectParameters(UInt32 index, const void* parameterData, UInt32 parameterDataByteSize, UInt32 operationSet) {
		return SUCCEEDED(m_Voice->SetEffectParameters(index, parameterData, parameterDataByteSize, operationSet));
	}
};