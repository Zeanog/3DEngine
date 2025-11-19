#include "EchoParameters.h"
#include "SubmixVoice.h"

IUnknown* EchoParameters::InstantiateFX() {
	IUnknown* pEffect = nullptr;
	verify(SUCCEEDED(CreateFX(CLSID_FXEcho, &pEffect)));
	return pEffect;
}

Bool EchoParameters::UpdateParams(SubmixVoice* category, UInt32 fxIndex, const rapidjson::Value& value) {
	TParameters params;
	SetToDefault(params);
	if (!UpdateFrom(value, params)) {
		assert(0);
		return false;
	}

	if (!category->SetEffectParameters(fxIndex, &params, sizeof(decltype(params)))) {
		assert(0);
		return false;
	}

	if (!category->EnableEffect(fxIndex)) {
		assert(0);
		return false;
	}

	return true;
}