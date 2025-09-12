#pragma once

#include "AVoice.h" 
#include <xaudio2.h>

class MasteringVoice : public AVoice {
	INHERITEDCLASS_TYPEDEFS(MasteringVoice, AVoice)

	friend class AudioSystem;

protected:
	IXAudio2MasteringVoice* m_Voice = nullptr;

	virtual void	Destroy();

public:
	MasteringVoice();

	virtual Bool	Init(IXAudio2* audio);
};