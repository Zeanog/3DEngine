#include "ReverbParameters.h"
#include "SubmixVoice.h"

IUnknown* ReverbParameters::InstantiateFX() {
	IUnknown* pEffect = nullptr;
	verify(SUCCEEDED(XAudio2CreateReverb(&pEffect)));
	return pEffect;
}

Bool ReverbParameters::UpdateParams(SubmixVoice* category, UInt32 fxIndex, const rapidjson::Value& value) {
	TParameters params;
	SetToDefault(params);
	if (!UpdateFrom(value, params)) {
		assert(0);
		return false;
	}

	if(!category->SetEffectParameters(fxIndex, &params, sizeof(decltype(params)))) {
		assert(0);
		return false;
	}

	if (!category->EnableEffect(fxIndex)) {
		assert(0);
		return false;
	}

	return true;
}