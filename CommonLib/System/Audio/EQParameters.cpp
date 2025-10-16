#include "EQParameters.h"
#include "AudioSystem.h"

IUnknown* EQParameters::InstantiateFX() {
	IUnknown* pEffect = nullptr;
	HRESULT hr = CreateFX(CLSID_FXEQ, &pEffect);
	if (FAILED(hr)) {
		return nullptr;
	}
	return pEffect;
}

Bool EQParameters::UpdateParams(const StaticString& categoryName, UInt32 index, const rapidjson::Value& value) {
	FXEQ_PARAMETERS params;
	SetToDefault(params);
	if (!UpdateFrom(&params, value)) {
		assert(0);
		return false;
	}

	if (!Singleton<AudioSystem>::GetInstance()->SetEffectParameters(categoryName, index, &params, sizeof(decltype(params)))) {
		assert(0);
		return false;
	}

	if (!Singleton<AudioSystem>::GetInstance()->EnableEffect(categoryName, index)) {
		assert(0);
		return false;
	}

	return true;
}