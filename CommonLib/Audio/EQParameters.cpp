#include "EQParameters.h"
#include "SubmixVoice.h"

IUnknown* EQParameters::CreateParams() {
	IUnknown* pEffect = nullptr;
	verify(SUCCEEDED(CreateFX(CLSID_FXEQ, &pEffect)));
	return pEffect;
}