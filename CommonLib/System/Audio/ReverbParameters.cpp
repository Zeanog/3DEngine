#include "ReverbParameters.h"
#include "AudioSystem.h"

IUnknown* ReverbParameters::InstantiateFX() {
	IUnknown* pEffect = nullptr;
	HRESULT hr = XAudio2CreateReverb(&pEffect);
	if (FAILED(hr)) {
		return nullptr;
	}
	return pEffect;
}

Bool ReverbParameters::UpdateParams(const StaticString& categoryName, UInt32 index, const rapidjson::Value& value) {
	XAUDIO2FX_REVERB_PARAMETERS params;
	SetToDefault(params);
	if (!UpdateFrom(&params, value)) {
		assert(0);
		return false;
	}

	if(!Singleton<AudioSystem>::GetInstance()->SetEffectParameters(categoryName, index, &params, sizeof(decltype(params)))) {
		assert(0);
		return false;
	}

	if (!Singleton<AudioSystem>::GetInstance()->EnableEffect(categoryName, index)) {
		assert(0);
		return false;
	}

	return true;
}