#pragma once

#include "System\Typedefs.h"
#include "System/Reflector.h"

#include <xaudio2fx.h>

template<>
class Reflector<XAUDIO2FX_REVERB_PARAMETERS> : public AReflector {
	INHERITEDCLASS_TYPEDEFS(Reflector, AReflector)
	SINGLETON_DECLARATIONS(TSelf) {
		ADD_MEMBERINFO(TReflected, WetDryMix);
		ADD_MEMBERINFO(TReflected, ReflectionsDelay);
		ADD_MEMBERINFO(TReflected, ReverbDelay);
		ADD_MEMBERINFO(TReflected, RearDelay);
		ADD_MEMBERINFO(TReflected, PositionLeft);
		ADD_MEMBERINFO(TReflected, PositionRight);
		ADD_MEMBERINFO(TReflected, PositionMatrixLeft);
		ADD_MEMBERINFO(TReflected, PositionMatrixRight);
		ADD_MEMBERINFO(TReflected, EarlyDiffusion);
		ADD_MEMBERINFO(TReflected, LateDiffusion);
		ADD_MEMBERINFO(TReflected, LowEQGain);
		ADD_MEMBERINFO(TReflected, LowEQCutoff);
		ADD_MEMBERINFO(TReflected, HighEQGain);
		ADD_MEMBERINFO(TReflected, HighEQCutoff);
		ADD_MEMBERINFO(TReflected, RoomFilterFreq);
		ADD_MEMBERINFO(TReflected, RoomFilterMain);
		ADD_MEMBERINFO(TReflected, RoomFilterHF);
		ADD_MEMBERINFO(TReflected, Density);
		ADD_MEMBERINFO(TReflected, ReflectionsGain);
		ADD_MEMBERINFO(TReflected, ReverbGain);
		ADD_MEMBERINFO(TReflected, DecayTime);
		ADD_MEMBERINFO(TReflected, RoomSize);
		ADD_MEMBERINFO(TReflected, DisableLateField);
	}

public:
	typedef XAUDIO2FX_REVERB_PARAMETERS	TReflected;
};

/////////////////////////////////////////////

class ReverbParameters : public XAUDIO2FX_REVERB_PARAMETERS {
	INHERITEDCLASS_TYPEDEFS(ReverbParameters, XAUDIO2FX_REVERB_PARAMETERS)

public:
	void	Init() {
		WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;       // how much reverb vs dry
		ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY; // delay for first reflections
		ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;      // delay before the reverb tail
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

	Bool	UpdateFrom(const rapidjson::Value& reverbVal) {
		if (!reverbVal.IsObject()) {
			return false;
		}

		for (auto iter = reverbVal.MemberBegin(); iter != reverbVal.MemberEnd(); ++iter) {
			Singleton<Reflector<TSuper>>::GetInstance()->Set(iter->name.GetString(), this, iter->value);
		}
		return true;
	}
};