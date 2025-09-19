#include "Sound.h"

#include "SourceVoice.h"
#include "Loaders/AudioLoader.h"

Bool Sound::UploadData(const AudioLoader& loader) {
	m_Data = loader.Buffer();
	m_Format = loader.Format();

	m_Duration = (Float32)m_Data->AudioBytes / ((Float32)(m_Format->Format.wBitsPerSample / 8.0f) * (Float32)m_Format->Format.nSamplesPerSec);
	return true;
}