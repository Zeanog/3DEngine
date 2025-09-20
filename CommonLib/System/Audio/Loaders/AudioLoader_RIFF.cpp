#include "AudioLoader_RIFF.h"
#include "..\..\File_RIFF.h"
#include <xaudio2.h>


Bool AudioLoader_RIFF::Load(const Char* fileName) {
	File_RIFF	file;

	m_AudioDataSize = 0;
	m_AudioData = nullptr;

	if (!file.Open(fileName, "rb")) {
		return false;
	}

	UInt32 dwChunkSize{};
	UInt32 dwChunkPosition{};

	//check the file type, should be fourccWAVE or 'XWMA'
	file.FindChunk(File_RIFF::FourCC::RIFF, dwChunkSize, dwChunkPosition);
	file.Seek(dwChunkPosition, SEEK_SET);

	UInt32 filetype{};
	file.Read(filetype);
	if (filetype != File_RIFF::FourCC::WAVE) {
		return false;
	}

	file.FindChunk(File_RIFF::FourCC::FMT, dwChunkSize, dwChunkPosition);
	file.Seek(dwChunkPosition, SEEK_SET);

	if (!file.Read(m_Format)) {
		return false;
	}

	file.FindChunk(File_RIFF::FourCC::DATA, dwChunkSize, dwChunkPosition);
	file.Seek(dwChunkPosition, SEEK_SET);

	BYTE* data = new BYTE[dwChunkSize];
	if (!file.Read(data, dwChunkSize)) {
		return false;
	}

	m_AudioData.reset(data);
	m_AudioDataSize = dwChunkSize;
	
	return true;
}