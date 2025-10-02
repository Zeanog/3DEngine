#pragma once

#include "AParameters.h"

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

class EQParameters : public AParameters<FXEQ_PARAMETERS> {
	INHERITEDCLASS_TYPEDEFS(EQParameters, AParameters)

public:
	virtual void	SetToDefault() override {
		FrequencyCenter0 = FXEQ_DEFAULT_FREQUENCY_CENTER_0;
		Gain0 = FXEQ_DEFAULT_GAIN;
		Bandwidth0 = FXEQ_DEFAULT_BANDWIDTH;
		FrequencyCenter1 = FXEQ_DEFAULT_FREQUENCY_CENTER_1;
		Gain1 = FXEQ_DEFAULT_GAIN;
		Bandwidth1 = FXEQ_DEFAULT_BANDWIDTH;
		FrequencyCenter2 = FXEQ_DEFAULT_FREQUENCY_CENTER_2;
		Gain2 = FXEQ_DEFAULT_GAIN;
		Bandwidth2 = FXEQ_DEFAULT_BANDWIDTH;
		FrequencyCenter3 = FXEQ_DEFAULT_FREQUENCY_CENTER_3;
		Gain3 = FXEQ_DEFAULT_GAIN;
		Bandwidth3 = FXEQ_DEFAULT_BANDWIDTH;
	}
};