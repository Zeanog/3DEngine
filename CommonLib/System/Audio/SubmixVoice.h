#pragma once

#include "ASourceVoice.h"

class SubmixVoice : public ASourceVoice<IXAudio2SubmixVoice> {
	INHERITEDCLASS_TYPEDEFS(SubmixVoice, ASourceVoice<IXAudio2SubmixVoice>)

	friend class AudioSystem;

protected:
	UInt32		m_NumChannels{};
	UInt32		m_SampleRate{};

public:
	SubmixVoice();

	virtual Bool	Init(IXAudio2* audio, UInt32 numChannels, UInt32 sampleRate);

	virtual UInt32	NumChannels() const override {
		return m_NumChannels;
	}
};