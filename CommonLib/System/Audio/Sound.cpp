#include "Sound.h"

#include "SourceVoice.h"
#include "Loaders/AudioLoader.h"

Bool Sound::UploadData(const AudioLoader& loader) {
	m_Data = loader.Buffer();
	m_Format = loader.Format();
	return true;
}