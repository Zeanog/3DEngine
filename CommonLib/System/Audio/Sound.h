#pragma once

#include "System\Typedefs.h"
#include <xaudio2.h>
#include <memory>

class SourceVoice;
class AudioLoader;

class Sound {
protected:
	std::unique_ptr<Byte>		m_Data;

	XAUDIO2_BUFFER				m_Buffer{};

	DEFINE_MEMBER_EX(WAVEFORMATEXTENSIBLE, Format)//TODO: Possibly cache the Hash value for this format
	DEFINE_MEMBER_EX(Float32, Duration)

public:
	~Sound();

	void		SetContext(void* cxt) {
		m_Buffer.pContext = cxt;
	}

	const XAUDIO2_BUFFER* Buffer() const {
		return &m_Buffer;
	}

	Bool	UploadData(AudioLoader& loader);
};