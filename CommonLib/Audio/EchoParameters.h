#pragma once

#include "AParameters.h"
#include <guiddef.h>
#include <xapofx.h>

template<>
class Reflector<FXECHO_PARAMETERS> : public AReflectorJson {
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {
		REGISTER_MEMBER(TReflected, WetDryMix);
		REGISTER_MEMBER(TReflected, Feedback);
		REGISTER_MEMBER(TReflected, Delay);
	}

public:
	typedef FXECHO_PARAMETERS	TReflected;
};

/////////////////////////////////////////////

struct EchoParameters : public AParameters<FXECHO_PARAMETERS> {
	INHERITED_CLASS_TYPEDEFS(EchoParameters, AParameters)

public:
	typedef TSuper::TParameters	TParameters;

public:
	static constexpr void	SetToDefault(TParameters& params) {
		params.WetDryMix = FXECHO_DEFAULT_WETDRYMIX;
		params.Feedback = FXECHO_DEFAULT_FEEDBACK;
		params.Delay = FXECHO_DEFAULT_DELAY;
	}

	static IUnknown* CreateParams();

	template<typename TVoice>
	static Bool UpdateParams(TVoice* voice, UInt32 fxIndex, const rapidjson::Value& value) {
		TParameters params;
		SetToDefault(params);
		if (!UpdateFrom(value, params)) {
			assert(0);
			return false;
		}

		if (!voice->SetEffectParameters(fxIndex, &params, sizeof(decltype(params)))) {
			assert(0);
			return false;
		}

		if (!voice->EnableEffect(fxIndex)) {
			assert(0);
			return false;
		}

		return true;
	}
};