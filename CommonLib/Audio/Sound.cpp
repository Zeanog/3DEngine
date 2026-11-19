#include "Sound.h"

#include "SourceVoice.h"
#include "Loaders/AAudioLoader.h"

//namespace Neo {
	Sound::~Sound() {
	}

	void Sound::PopulateBuffer(XAUDIO2_BUFFER& outBuffer) const {
		outBuffer.AudioBytes = m_DataSize;
		outBuffer.pAudioData = m_Data.get();
		outBuffer.pContext = (void*)this;
		outBuffer.Flags = XAUDIO2_END_OF_STREAM;
	}

#include "AudioSystem.h"
	Bool Sound::UploadData(AAudioLoader& loader) {
		m_Format = loader.Format();
		m_FormatHash = Singleton<AudioSystem>::GetInstance()->GenerateHash(m_Format.Format);

		m_DataSize = loader.DataSize();
		m_Data.release();
		m_Data = std::move(loader.Data());
		assert(!loader.IsValid());

		auto bytesPerSample = m_Format.Format.wBitsPerSample / 8.0f;
		m_Duration = (Float32)m_DataSize / (bytesPerSample * (Float32)m_Format.Format.nSamplesPerSec);
		return true;
	}
//}