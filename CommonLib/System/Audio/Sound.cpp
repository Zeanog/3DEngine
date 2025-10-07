#include "Sound.h"

#include "SourceVoice.h"
#include "Loaders/AudioLoader.h"

Sound::~Sound() {
}

Bool Sound::UploadData(AudioLoader& loader) {
	m_Format = loader.Format();

	m_Data = loader.Data();//Take ownership of the data.  This avoids another allocation and copy
	assert(!loader.IsValid());//Verify that the pointer is nullptr

	m_Buffer.AudioBytes = loader.DataSize();
	m_Buffer.pAudioData = m_Data.get();
	m_Buffer.pContext = this;
	m_Buffer.Flags = XAUDIO2_END_OF_STREAM;

	m_Duration = (Float32)m_Buffer.AudioBytes / ((Float32)(m_Format.Format.wBitsPerSample / 8.0f) * (Float32)m_Format.Format.nSamplesPerSec);
	return true;
}