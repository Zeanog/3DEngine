#pragma once

#include "AudioLoader.h"

class AudioLoader_MP3 : public AudioLoader {
protected:
	typedef int16_t	TData;

public:
	virtual Bool	Load(const Char* fileName);
};