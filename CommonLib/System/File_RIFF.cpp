#include "File_RIFF.h"

Bool File_RIFF::FindChunk(UInt32 fourcc, UInt32& dwChunkSize, UInt32& dwChunkDataPosition)
{
    Seek(0, SEEK_SET);

    UInt32 dwChunkType{};
    UInt32 dwChunkDataSize{};
    UInt32 dwRIFFDataSize{};
    UInt32 dwFileType{};
    UInt32 bytesRead{};
    UInt32 dwOffset{};

    errno_t err{};
    while (!err)
    {
        if (!Read(dwChunkType)) {
            return false;
        }

        if (!Read(dwChunkDataSize)) {
            return false;
        }

        switch (dwChunkType)
        {
        case FourCC::RIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (!Read(dwFileType)) {
                return false;
            }
            break;

        default:
            Seek(dwChunkDataSize, SEEK_CUR);
        }

        dwOffset += sizeof(UInt32) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return true;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return false;
    }

    return true;
}