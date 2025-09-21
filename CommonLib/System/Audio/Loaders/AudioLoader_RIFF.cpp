#include "AudioLoader_RIFF.h"
#include "..\..\File.h"
#include <xaudio2.h>

#ifdef _XBOX //Big-Endian
enum FourCC : UInt32 {
	RIFF = 'RIFF',
	DATA = 'data',
	FMT = ' fmt',
	WAVE = 'WAVE',
	XWMA = 'XWMA',
	DPDS = 'dpds'
};
#else //Little-Endian
enum FourCC : UInt32 {
	RIFF = 'FFIR',
	DATA = 'atad',
	FMT = ' tmf',
	WAVE = 'EVAW',
	XWMA = 'AMWX',
	DPDS = 'sdpd'
};
#endif

Bool FindChunk(File& file, UInt32 fourcc, UInt32& outChunkSize, UInt32& outChunkDataPosition)
{
    file.Seek(0, SEEK_SET);

    UInt32 dwChunkType{};
    UInt32 dwChunkDataSize{};
    UInt32 dwRIFFDataSize{};
    UInt32 dwFileType{};
    UInt32 bytesRead{};
    UInt32 dwOffset{};

    errno_t err{};
    while (!err)
    {
        if (!file.Read(dwChunkType)) {
            return false;
        }

        if (!file.Read(dwChunkDataSize)) {
            return false;
        }

        switch (dwChunkType)
        {
        case FourCC::RIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (!file.Read(dwFileType)) {
                return false;
            }
            break;

        default:
            file.Seek(dwChunkDataSize, SEEK_CUR);
        }

        dwOffset += sizeof(UInt32) * 2;

        if (dwChunkType == fourcc)
        {
            outChunkSize = dwChunkDataSize;
            outChunkDataPosition = dwOffset;
            return true;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return false;
    }

    return false;
}

Bool AudioLoader_RIFF::Load(const Char* fileName) {
	File	file;

	m_AudioDataSize = 0;
	m_AudioData.reset(nullptr);

	if (!file.Open(fileName, "rb")) {
		return false;
	}

	UInt32 dwChunkSize{};
	UInt32 dwChunkPosition{};

	//TODO: All these FindChunk calls are probably inefficient

	//check the file type, should be fourccWAVE or 'XWMA'
	verify(FindChunk(file, FourCC::RIFF, dwChunkSize, dwChunkPosition));
	file.Seek(dwChunkPosition, SEEK_SET);

	UInt32 filetype{};
	file.Read(filetype);
	if (filetype != FourCC::WAVE) {
		return false;
	}

	verify(FindChunk(file, FourCC::FMT, dwChunkSize, dwChunkPosition));
	file.Seek(dwChunkPosition, SEEK_SET);
	if (!file.Read(m_Format)) {
		return false;
	}

	verify(FindChunk(file, FourCC::DATA, dwChunkSize, dwChunkPosition));
	file.Seek(dwChunkPosition, SEEK_SET);

	BYTE* data = new BYTE[dwChunkSize];
	if (!file.Read(data, dwChunkSize)) {
		DeletePtr(data);
		return false;
	}
	
	m_AudioData.reset(data);//Take ownership of the data
	m_AudioDataSize = dwChunkSize;
	
	return true;
}