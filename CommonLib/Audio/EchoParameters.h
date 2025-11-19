#pragma once

#include "AParameters.h"

#include <guiddef.h>
#include <xapofx.h>

template<>
class Reflector<FXECHO_PARAMETERS> : public AReflector {
	INHERITEDCLASS_TYPEDEFS(Reflector, AReflector)
	SINGLETON_DECLARATIONS(TSelf) {
		REGISTER_MEMBER(TReflected, WetDryMix);
		REGISTER_MEMBER(TReflected, Feedback);
		REGISTER_MEMBER(TReflected, Delay);
	}

public:
	typedef FXECHO_PARAMETERS	TReflected;
};

/////////////////////////////////////////////

struct EchoParameters : public AParameters<FXECHO_PARAMETERS> {
	INHERITEDCLASS_TYPEDEFS(EchoParameters, AParameters)

public:
	static constexpr void	SetToDefault(FXECHO_PARAMETERS& params) {
		params.WetDryMix = FXECHO_DEFAULT_WETDRYMIX;
		params.Feedback = FXECHO_DEFAULT_FEEDBACK;
		params.Delay = FXECHO_DEFAULT_DELAY;
	}

	static IUnknown* InstantiateFX();
	static Bool UpdateParams(class SubmixVoice* category, UInt32 fxIndex, const rapidjson::Value& value);
};