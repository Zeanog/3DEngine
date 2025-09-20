#pragma once

#include "System\Typedefs.h"
#include <xaudio2.h>
#include <memory>

class SourceVoice;
class AudioLoader;

class Sound {
protected:
	std::unique_ptr<BYTE>		m_Data;

	XAUDIO2_BUFFER				m_Buffer{};
	WAVEFORMATEXTENSIBLE		m_Format{};
	Float32						m_Duration{};

public:
	DECLARE_GETSET( Format )
	DECLARE_GETSET( Duration )

	const XAUDIO2_BUFFER* Buffer() const {
		return &m_Buffer;
	}

	Bool	UploadData(AudioLoader& loader);
};