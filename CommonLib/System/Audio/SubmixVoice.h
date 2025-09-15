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

	/*Float32			Volume() const;
	Bool			Volume(Float32 newVolume) const;
	Bool			Volume(Float32 newVolume, UInt32 operationSet) const;*/
};