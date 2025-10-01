#pragma once

#include "AParameters.h"

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

class EchoParameters : public AParameters<FXECHO_PARAMETERS> {
	INHERITEDCLASS_TYPEDEFS(EchoParameters, AParameters)

public:
	virtual void	SetToDefault() override {
		WetDryMix = FXECHO_DEFAULT_WETDRYMIX;
		Feedback = FXECHO_DEFAULT_FEEDBACK;
		Delay = FXECHO_DEFAULT_DELAY;
	}
};