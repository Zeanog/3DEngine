#include "EchoParameters.h"
#include "SubmixVoice.h"

IUnknown* EchoParameters::InstantiateFX() {
	IUnknown* pEffect = nullptr;
	verify(SUCCEEDED(CreateFX(CLSID_FXEcho, &pEffect)));
	return pEffect;
}

Bool EchoParameters::UpdateParams(SubmixVoice* category, UInt32 index, const rapidjson::Value& value) {
	TParameters params;
	SetToDefault(params);
	if (!UpdateFrom(params, value)) {
		assert(0);
		return false;
	}

	if (!category->SetEffectParameters(index, &params, sizeof(decltype(params)))) {
		assert(0);
		return false;
	}

	if (!category->EnableEffect(index)) {
		assert(0);
		return false;
	}

	return true;
}