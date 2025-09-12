#pragma once

#include "AudioLoader.h"

class AudioLoader_RIFF : public AudioLoader {
public:
	virtual Bool	Load(const Char* fileName);
};