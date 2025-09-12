#pragma once

#include "AVoice.h"

#include <xaudio2.h>

class SubmixVoice : public AVoice {
	INHERITEDCLASS_TYPEDEFS(SubmixVoice, AVoice)

	friend class AudioSystem;

protected:
	IXAudio2SubmixVoice* m_Voice{};

	UInt32		m_NumChannels{};
	UInt32		m_SampleRate{};

protected:
	virtual void	Destroy();

public:
	SubmixVoice();

	DECLARE_GETSET(Voice)

	virtual Bool	Init(IXAudio2* audio, UInt32 numChannels, UInt32 sampleRate);

	Float32			Volume() const;
	Bool			Volume(Float32 newVolume) const;
	Bool			Volume(Float32 newVolume, UInt32 operationSet) const;
};