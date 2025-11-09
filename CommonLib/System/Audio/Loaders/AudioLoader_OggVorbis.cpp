#include "AudioLoader_OggVorbis.h"
#include <vorbis\vorbisfile.h>
#include <xaudio2.h>

Bool AudioLoader_OggVorbis::Load(const Char* fileName) {
    m_AudioDataSize = 0;
    m_AudioData.reset(nullptr);//Clear the pointer;

    OggVorbis_File oggFile;
    if (ov_fopen(fileName, &oggFile) < 0) {
        //std::cerr << "Failed to open OGG file" << std::endl;
        return false;
    }

    vorbis_info* vi = ov_info(&oggFile, -1);
    m_Format.Format.wFormatTag = WAVE_FORMAT_PCM;
    m_Format.Format.nChannels = vi->channels;
    m_Format.Format.nSamplesPerSec = vi->rate;
    m_Format.Format.wBitsPerSample = 16; // stb_vorbis decodes to 16-bit PCM
    m_Format.Format.nBlockAlign = m_Format.Format.nChannels * (m_Format.Format.wBitsPerSample / 8);
    m_Format.Format.nAvgBytesPerSec = m_Format.Format.nSamplesPerSec * m_Format.Format.nBlockAlign;
    m_Format.Format.cbSize = sizeof(m_Format) - sizeof(m_Format.Format);

    m_Format.Samples.wValidBitsPerSample = m_Format.Format.wBitsPerSample;
    //TODO: Handle more channel configurations
    m_Format.dwChannelMask = (m_Format.Format.nChannels == 1) ? SPEAKER_FRONT_CENTER : (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT);
    m_Format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    long total_sections = ov_streams(&oggFile);
    Int64 total_samples = (Int64)ov_pcm_total(&oggFile, -1);
    
    m_AudioDataSize = total_samples * (m_Format.Format.wBitsPerSample / 8) * m_Format.Format.nChannels;
    m_AudioData.reset(new Byte[m_AudioDataSize]);

    UInt32 bytes_read_total = 0;
    int current_section = 0;

    while (bytes_read_total < m_AudioDataSize) {
        auto bytes_read = ov_read(&oggFile, (char*)m_AudioData.get() + bytes_read_total, m_AudioDataSize - bytes_read_total, 0, 2, 1, &current_section);

        if(bytes_read == 0) {
            break; // End of file
        }
        else if (bytes_read < 0) {
            m_AudioData.reset(nullptr);
            ov_clear(&oggFile);
            return false;
        }
        bytes_read_total += bytes_read;
    }

    ov_clear(&oggFile);

    return true;
}