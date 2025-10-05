#pragma once

#include "AudioLoader.h"

class AudioLoader_OggVorbis : public AudioLoader {
public:
	virtual Bool	Load(const Char* fileName);
};