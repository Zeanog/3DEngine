#include "EchoParameters.h"
#include "AudioSystem.h"

IUnknown* EchoParameters::InstantiateFX() {
	IUnknown* pEffect = nullptr;
	HRESULT hr = CreateFX(CLSID_FXEcho , &pEffect);
	if (FAILED(hr)) {
		return nullptr;
	}
	return pEffect;
}

Bool EchoParameters::UpdateParams(const StaticString& categoryName, UInt32 index, const rapidjson::Value& value) {
	FXECHO_PARAMETERS params;
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