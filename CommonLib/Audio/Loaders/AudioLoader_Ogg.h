#pragma once

#include "AAudioLoader.h"

class AudioLoader_Ogg : public AAudioLoader {
public:
	virtual Bool	Load(const Char* fileName);
};