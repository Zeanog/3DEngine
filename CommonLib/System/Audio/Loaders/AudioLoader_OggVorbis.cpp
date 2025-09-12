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

    std::vector<BYTE> buffer;
    buffer.reserve(1024 * 1024); // Allocate a buffer for audio data

    // Read and decode OGG data into PCM format
    char pcmData[4096];
    int bytesRead = 0;
    int section = 0;

    while (true) {
        bytesRead = ov_read(&oggFile, pcmData, sizeof(pcmData), 0, 2, 1, &section);
        if (bytesRead == 0) {
            break; // End of file
        }
        else if (bytesRead < 0) {
            
            return false;
        }
        buffer.insert(buffer.end(), pcmData, pcmData + bytesRead);
    }

    ov_clear(&oggFile);

    m_BufferInfo.AudioBytes = static_cast<UINT32>(buffer.size());
    BYTE* data = new BYTE[m_BufferInfo.AudioBytes];
    memcpy_s(data, m_BufferInfo.AudioBytes, buffer.data(), buffer.size());
    m_BufferInfo.pAudioData = data;
    m_BufferInfo.Flags = XAUDIO2_END_OF_STREAM;
    return true;
}