#pragma once

#include "AParameters.h"

#include <guiddef.h>
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

struct ReverbParameters : public AParameters<XAUDIO2FX_REVERB_PARAMETERS> {
	INHERITEDCLASS_TYPEDEFS(ReverbParameters, AParameters)

public:
	static constexpr void	SetToDefault(XAUDIO2FX_REVERB_PARAMETERS& params) {
		params.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
		params.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
		params.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
		params.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
		params.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		params.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		params.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		params.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		params.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
		params.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
		params.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
		params.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
		params.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
		params.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
		params.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
		params.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
		params.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
		params.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
		params.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
		params.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
		params.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
		params.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
		params.DisableLateField = FALSE;  // enable late reverb tail
	}

	static IUnknown* InstantiateFX();
	static Bool UpdateParams(const StaticString& categoryName, UInt32 index, const rapidjson::Value& value);
};