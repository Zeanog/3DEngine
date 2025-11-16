#pragma once

#include "AAudioLoader.h"

class AudioLoader_RIFF : public AAudioLoader {
public:
	virtual Bool	Load(const Char* fileName);
};