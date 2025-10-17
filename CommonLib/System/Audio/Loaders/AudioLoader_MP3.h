#pragma once

#include "AAudioLoader.h"

class AudioLoader_MP3 : public AAudioLoader {
protected:
	typedef int16_t	TData;

public:
	virtual Bool	Load(const Char* fileName);
};