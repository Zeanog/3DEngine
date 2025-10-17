#pragma once

#include "AAudioLoader.h"

class AudioLoader_OggVorbis : public AAudioLoader {
public:
	virtual Bool	Load(const Char* fileName);
};