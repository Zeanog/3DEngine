#pragma once

#include "System\Typedefs.h"

#include <xaudio2.h>

class SourceVoice;

class SourceVoiceCallbacks : public IXAudio2VoiceCallback {
protected:
	SourceVoice* m_Voice;

public:
	SourceVoiceCallbacks(SourceVoice* source);

	virtual void __stdcall OnVoiceProcessingPassStart(UINT32 bytesRequired);

	virtual void __stdcall OnVoiceProcessingPassEnd();

	virtual void __stdcall OnStreamEnd();

	virtual void __stdcall OnBufferStart(void* pBufferContext);

	virtual void __stdcall OnBufferEnd(void* pBufferContext);

	virtual void __stdcall OnLoopEnd(void* pBufferContext);

	virtual void __stdcall OnVoiceError(void* pBufferContext, HRESULT error);
};