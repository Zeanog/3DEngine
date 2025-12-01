#pragma once

#include "ASourceVoice.h"

class SubmixVoice : public ASourceVoice<IXAudio2SubmixVoice> {
	INHERITEDCLASS_TYPEDEFS(SubmixVoice, ASourceVoice<IXAudio2SubmixVoice>)

	friend class AudioSystem;

protected:
	UInt16		m_NumChannels{};
	UInt16		m_SampleRate{};

public:
	SubmixVoice();

	virtual Bool	Init(IXAudio2* audio, UInt16 numChannels, UInt16 sampleRate);

	virtual UInt16	NumChannels() const override {
		return m_NumChannels;
	}

	UInt16	SampleRate() const {
		return m_SampleRate;
	}
};