#pragma once

#include "AParameters.h"

#include <xaudio2fx.h>

template<>
class Reflector<XAUDIO2FX_REVERB_PARAMETERS> : public AReflector {
	INHERITEDCLASS_TYPEDEFS(Reflector, AReflector)
	SINGLETON_DECLARATIONS(TSelf) {
		REGISTER_MEMBER(TReflected, WetDryMix);
		REGISTER_MEMBER(TReflected, ReflectionsDelay);
		REGISTER_MEMBER(TReflected, ReverbDelay);
		REGISTER_MEMBER(TReflected, RearDelay);
		REGISTER_MEMBER(TReflected, PositionLeft);
		REGISTER_MEMBER(TReflected, PositionRight);
		REGISTER_MEMBER(TReflected, PositionMatrixLeft);
		REGISTER_MEMBER(TReflected, PositionMatrixRight);
		REGISTER_MEMBER(TReflected, EarlyDiffusion);
		REGISTER_MEMBER(TReflected, LateDiffusion);
		REGISTER_MEMBER(TReflected, LowEQGain);
		REGISTER_MEMBER(TReflected, LowEQCutoff);
		REGISTER_MEMBER(TReflected, HighEQGain);
		REGISTER_MEMBER(TReflected, HighEQCutoff);
		REGISTER_MEMBER(TReflected, RoomFilterFreq);
		REGISTER_MEMBER(TReflected, RoomFilterMain);
		REGISTER_MEMBER(TReflected, RoomFilterHF);
		REGISTER_MEMBER(TReflected, Density);
		REGISTER_MEMBER(TReflected, ReflectionsGain);
		REGISTER_MEMBER(TReflected, ReverbGain);
		REGISTER_MEMBER(TReflected, DecayTime);
		REGISTER_MEMBER(TReflected, RoomSize);
		REGISTER_MEMBER(TReflected, DisableLateField);
	}

public:
	typedef XAUDIO2FX_REVERB_PARAMETERS	TReflected;
};

/////////////////////////////////////////////

class ReverbParameters : public AParameters<XAUDIO2FX_REVERB_PARAMETERS> {
	INHERITEDCLASS_TYPEDEFS(ReverbParameters, AParameters)

public:
	virtual void	SetToDefault() override {
		WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
		ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
		ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
		RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
		PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
		LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
		LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
		LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
		HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
		HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
		RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
		RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
		RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
		Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
		ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
		ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
		DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
		RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
		DisableLateField = FALSE;  // enable late reverb tail
	}
};