#include "Sound.h"

#include "SourceVoice.h"
#include "Loaders/AudioLoader.h"

Float32 Sound::Duration() const {
	return (Float32)m_Format->Samples.wValidBitsPerSample / (Float32)m_Format->Format.nSamplesPerSec;
	//return (Float32)m_Data->AudioBytes / (Float32)m_Format->Format.nAvgBytesPerSec;
}

Bool Sound::UploadData(const AudioLoader& loader) {
	m_Data = loader.Buffer();
	m_Format = loader.Format();
	return true;
}