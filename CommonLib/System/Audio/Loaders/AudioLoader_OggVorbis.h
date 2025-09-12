#pragma once

#include "AudioLoader.h"

#include <vector>

class AudioLoader_OggVorbis : public AudioLoader {
public:
	virtual Bool	Load(const Char* fileName);
};