#include "AudioLoader_OggVorbis.h"
#include <vorbis\vorbisfile.h>
#include <xaudio2.h>

#define WAVE_FORMAT_VORBIS1   0x674f // 'Og'

Bool AudioLoader_OggVorbis::Load(const Char* fileName) {
 
    OggVorbis_File oggFile;
    if (ov_fopen(fileName, &oggFile) < 0) {
        //std::cerr << "Failed to open OGG file" << std::endl;
        return false;
    }

    vorbis_info* vi = ov_info(&oggFile, -1);
    m_Format.Format.wFormatTag = 1; // 1 for WAVE_FORMAT_PCM
    m_Format.Format.nChannels = vi->channels;
    m_Format.Format.nSamplesPerSec = vi->rate;
    m_Format.Format.wBitsPerSample = 16; // stb_vorbis decodes to 16-bit PCM
    m_Format.Format.nBlockAlign = m_Format.Format.nChannels * (m_Format.Format.wBitsPerSample / 8);
    m_Format.Format.nAvgBytesPerSec = m_Format.Format.nSamplesPerSec * m_Format.Format.nBlockAlign;
    m_Format.Format.cbSize = sizeof(m_Format) - sizeof(m_Format.Format);

    m_Format.Samples.wValidBitsPerSample = m_Format.Format.wBitsPerSample;
    m_Format.dwChannelMask = (m_Format.Format.nChannels == 1) ? SPEAKER_FRONT_CENTER : (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT);
    m_Format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    long total_sections = ov_streams(&oggFile);
    ogg_int64_t total_samples = ov_pcm_total(&oggFile, -1);
    
    // Calculate total size in bytes for a 16-bit, interleaved buffer
    // 16-bit samples = 2 bytes per sample
    ogg_int64_t total_bytes = total_samples * sizeof(short) * vi->channels;
    BYTE* buffer = new BYTE[total_bytes];

    long bytes_read_total = 0;
    int current_section = 0;

    while (bytes_read_total < total_bytes) {
        long bytes_read = ov_read(&oggFile, (char*)buffer + bytes_read_total, total_bytes - bytes_read_total, 0, 2, 1, &current_section);

        if(bytes_read == 0) {
            break; // End of file
        }
        else if (bytes_read < 0) {
            
            return false;
        }
        bytes_read_total += bytes_read;
    }

    ov_clear(&oggFile);

    m_AudioDataSize = total_bytes;
    m_AudioData.reset(buffer);
    return true;
}