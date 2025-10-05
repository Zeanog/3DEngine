#include "AudioLoader_MP3.h"
#include "System/File.h"
#include "System/List.h"

#define MINIMP3_IMPLEMENTATION
#include <System/Audio/Loaders/MiniMP3/minimp3_ex.h>
#include <xaudio2.h>

Bool AudioLoader_MP3::Load(const Char* fileName) {
    m_AudioDataSize = 0;
    m_AudioData.reset(nullptr);//Delete any existing data

    File    file;
    if (!file.Open(fileName, "rb")) {
        return false;
    }

    List<Byte> fileData;//TODO: Would be nice to use the DataStructureLibrary to avoid the allocation for each file
    verify(file.ReadContents(fileData));

    mp3dec_ex_t dec;
    Byte* data = &fileData[0];
    int error = mp3dec_ex_open_buf(&dec, data, fileData.Length(), MP3D_SEEK_TO_SAMPLE);
    if (error) {
        return false;
    }

    auto buffer = new TData[dec.samples * dec.info.channels];
    auto samples_read = mp3dec_ex_read(&dec, buffer, dec.samples * dec.info.channels);
    m_AudioDataSize = samples_read * sizeof(TData);
    m_AudioData.reset((Byte*)buffer);
 
    m_Format.Format.wFormatTag = WAVE_FORMAT_PCM;
    m_Format.Format.nChannels = dec.info.channels;
    m_Format.Format.nSamplesPerSec = dec.info.hz;
    m_Format.Format.wBitsPerSample = sizeof(TData) * 8; // stb_vorbis decodes to 16-bit PCM
    m_Format.Format.nBlockAlign = m_Format.Format.nChannels * (m_Format.Format.wBitsPerSample / 8);
    m_Format.Format.nAvgBytesPerSec = m_Format.Format.nSamplesPerSec * m_Format.Format.nBlockAlign;
    m_Format.Format.cbSize = sizeof(m_Format) - sizeof(m_Format.Format);

    m_Format.Samples.wValidBitsPerSample = m_Format.Format.wBitsPerSample;
    m_Format.dwChannelMask = (m_Format.Format.nChannels == 1) ? SPEAKER_FRONT_CENTER : (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT);
    m_Format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    mp3dec_ex_close(&dec);
    return true;
}