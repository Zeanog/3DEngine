#pragma once

#include "System\Typedefs.h"
#include <xaudio2.h>

class SourceVoice;
class AudioLoader;

class Sound {
protected:
	const XAUDIO2_BUFFER*		m_Data{};
	const WAVEFORMATEXTENSIBLE*	m_Format{};

public:
	DECLARE_GETSET( Format )

	const XAUDIO2_BUFFER* Data() const {
		return m_Data;
	}

	Bool	UploadData(const AudioLoader& loader);
};