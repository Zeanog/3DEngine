#include "AudioSystem.h"

#include "Sound.h"
#include "MasteringVoice.h"
#include "SourceVoice.h"
#include "SubmixVoice.h"

Bool AudioSystem::Init() {
	HRESULT hr = ::XAudio2Create(&m_Audio2, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		return false;
	}

	m_MasteringVoice = CreateMasteringVoice();

	return true;
}

void AudioSystem::Release() {
	FOREACH(iter, m_Voices) {
		(*iter)->Destroy();
	}
	m_Voices.Clear();

	if (m_MasteringVoice) {
		m_MasteringVoice->Destroy();
		m_MasteringVoice = nullptr;
	}

	::Release(m_Audio2);
}

MasteringVoice* AudioSystem::CreateMasteringVoice() {
	MasteringVoice* newVoice = new MasteringVoice();
	newVoice->Init(m_Audio2);
	return newVoice;
}

SourceVoice* AudioSystem::CreateSourceVoice(const WAVEFORMATEX& format, SubmixVoice* destVoice) {
	UINT64 hash = GenerateHash(format);
	if (m_FormatMap.Contains(hash)) {
		return m_FormatMap[hash];
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, format, destVoice->Voice());
	m_FormatMap.Add(hash, newVoice);
	m_Voices.Add(newVoice);
	return newVoice;
}

SourceVoice* AudioSystem::CreateSourceVoice(const WAVEFORMATEX& format) {
	UINT64 hash = GenerateHash(format);
	if (m_FormatMap.Contains(hash)) {
		return m_FormatMap[hash];
	}

	SourceVoice* newVoice = new SourceVoice(m_Audio2, format);
	m_FormatMap.Add(hash, newVoice);
	m_Voices.Add(newVoice);
	return newVoice;
}

SourceVoice* AudioSystem::CreateSourceVoice(Sound* sound, SubmixVoice* destVoice) {
	UINT64 hash = GenerateHash(sound->Format()->Format);
	if (m_FormatMap.Contains(hash)) {
		return m_FormatMap[hash];
	}
	SourceVoice* newVoice = new SourceVoice(m_Audio2, sound->Format()->Format, destVoice->Voice());
	m_FormatMap.Add(hash, newVoice);
	m_Voices.Add(newVoice);

	newVoice->Submit(sound);
	return newVoice;
}

#include <xaudio2fx.h>

SubmixVoice* AudioSystem::CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate) {
	SubmixVoice* newVoice = new SubmixVoice();
	newVoice->Init(m_Audio2, numChannels, sampleRate);

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
	verify(SUCCEEDED(newVoice->Voice()->SetEffectParameters(0, &reverbParams, sizeof(reverbParams))));

	// If needed, ensure effect is enabled (it was set to InitialState=TRUE above)
	verify(SUCCEEDED(newVoice->Voice()->EnableEffect(0)));
	
	return newVoice;
}