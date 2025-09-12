#include "AudioLoader_RIFF.h"
#include "..\..\File_RIFF.h"
#include <xaudio2.h>


Bool AudioLoader_RIFF::Load(const Char* fileName) {
	File_RIFF	file;

	m_BufferInfo.AudioBytes = 0;
	m_BufferInfo.pAudioData = nullptr;

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
	BYTE* pDataBuffer = new BYTE[dwChunkSize];

	file.Seek(dwChunkPosition, SEEK_SET);
	if (!file.Read(pDataBuffer, dwChunkSize)) {
		return false;
	}

	m_BufferInfo.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
	m_BufferInfo.pAudioData = pDataBuffer;  //buffer containing audio data
	m_BufferInfo.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	return true;
}