#include "ReverbParameters.h"
#include "SubmixVoice.h"

IUnknown* ReverbParameters::InstantiateFX() {
	IUnknown* pEffect = nullptr;
	verify(SUCCEEDED(XAudio2CreateReverb(&pEffect)));
	return pEffect;
}

Bool ReverbParameters::UpdateParams(SubmixVoice* category, UInt32 index, const rapidjson::Value& value) {
	TParameters params;
	SetToDefault(params);
	if (!UpdateFrom(&params, value)) {
		assert(0);
		return false;
	}

	if(!category->SetEffectParameters(index, &params, sizeof(decltype(params)))) {
		assert(0);
		return false;
	}

	if (!category->EnableEffect(index)) {
		assert(0);
		return false;
	}

	return true;
}