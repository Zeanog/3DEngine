#pragma once

#include "AParameters.h"

#include <guiddef.h>
#include <xapofx.h>

template<>
class Reflector<FXEQ_PARAMETERS> : public AReflector {
	INHERITEDCLASS_TYPEDEFS(Reflector, AReflector)
	SINGLETON_DECLARATIONS(TSelf) {
		REGISTER_MEMBER(TReflected, FrequencyCenter0);
		REGISTER_MEMBER(TReflected, Gain0);
		REGISTER_MEMBER(TReflected, Bandwidth0);
		REGISTER_MEMBER(TReflected, FrequencyCenter1);
		REGISTER_MEMBER(TReflected, Gain1);
		REGISTER_MEMBER(TReflected, Bandwidth1);
		REGISTER_MEMBER(TReflected, FrequencyCenter2);
		REGISTER_MEMBER(TReflected, Gain2);
		REGISTER_MEMBER(TReflected, Bandwidth2);
		REGISTER_MEMBER(TReflected, FrequencyCenter3);
		REGISTER_MEMBER(TReflected, Gain3);
		REGISTER_MEMBER(TReflected, Bandwidth3);
	}

public:
	typedef FXEQ_PARAMETERS	TReflected;
};

/////////////////////////////////////////////

struct EQParameters : public AParameters<FXEQ_PARAMETERS> {
	INHERITEDCLASS_TYPEDEFS(EQParameters, AParameters)

public:
	static constexpr void	SetToDefault(FXEQ_PARAMETERS& params) {
		params.FrequencyCenter0 = FXEQ_DEFAULT_FREQUENCY_CENTER_0;
		params.Gain0 = FXEQ_DEFAULT_GAIN;
		params.Bandwidth0 = FXEQ_DEFAULT_BANDWIDTH;
		params.FrequencyCenter1 = FXEQ_DEFAULT_FREQUENCY_CENTER_1;
		params.Gain1 = FXEQ_DEFAULT_GAIN;
		params.Bandwidth1 = FXEQ_DEFAULT_BANDWIDTH;
		params.FrequencyCenter2 = FXEQ_DEFAULT_FREQUENCY_CENTER_2;
		params.Gain2 = FXEQ_DEFAULT_GAIN;
		params.Bandwidth2 = FXEQ_DEFAULT_BANDWIDTH;
		params.FrequencyCenter3 = FXEQ_DEFAULT_FREQUENCY_CENTER_3;
		params.Gain3 = FXEQ_DEFAULT_GAIN;
		params.Bandwidth3 = FXEQ_DEFAULT_BANDWIDTH;
	}

	static IUnknown* InstantiateFX();
	static Bool UpdateParams(class SubmixVoice* category, UInt32 fxIndex, const rapidjson::Value& value);
};