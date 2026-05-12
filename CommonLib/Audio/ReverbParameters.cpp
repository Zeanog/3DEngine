#include "ReverbParameters.h"
#include "SubmixVoice.h"

IUnknown* ReverbParameters::CreateParams() {
	IUnknown* pEffect = nullptr;
	verify(SUCCEEDED(XAudio2CreateReverb(&pEffect)));
	return pEffect;
}