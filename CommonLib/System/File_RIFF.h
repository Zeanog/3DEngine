#pragma once

#include "File.h"

class File_RIFF : public File {
public:
#ifdef _XBOX //Big-Endian
	enum FourCC : UInt32 {
		RIFF = 'RIFF',
		DATA = 'data',
		FMT = ' fmt',
		WAVE = 'WAVE',
		XWMA = 'XWMA',
		DPDS = 'dpds'
	};
#endif

#ifndef _XBOX //Little-Endian
	enum FourCC : UInt32 {
		RIFF = 'FFIR',
		DATA = 'atad',
		FMT = ' tmf',
		WAVE = 'EVAW',
		XWMA = 'AMWX',
		DPDS = 'sdpd'
	};
#endif

public:
	Bool FindChunk(UInt32 fourcc, UInt32& dwChunkSize, UInt32& dwChunkDataPosition);
};