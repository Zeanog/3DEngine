#include "EchoParameters.h"
#include "SubmixVoice.h"

IUnknown* EchoParameters::CreateParams() {
	IUnknown* pEffect = nullptr;
	verify(SUCCEEDED(CreateFX(CLSID_FXEcho, &pEffect)));
	return pEffect;
}