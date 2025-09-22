#pragma once

#include "System\Typedefs.h"

#include <xaudio2.h>

class SourceVoice;

class SourceVoiceCallbacks : public IXAudio2VoiceCallback {
protected:
	SourceVoice* m_Voice;

public:
	SourceVoiceCallbacks(SourceVoice* source);

	virtual void __stdcall OnVoiceProcessingPassStart(UINT32 bytesRequired) override;

	virtual void __stdcall OnVoiceProcessingPassEnd() override;

	virtual void __stdcall OnStreamEnd() override;

	virtual void __stdcall OnBufferStart(void* pBufferContext) override;

	virtual void __stdcall OnBufferEnd(void* pBufferContext) override;

	virtual void __stdcall OnLoopEnd(void* pBufferContext) override;

	virtual void __stdcall OnVoiceError(void* pBufferContext, HRESULT error) override;
};