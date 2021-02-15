#include "ImageLoader_JPG.h"
#include "freeimage.h"

Bool ImageLoader_JPG::Load(const Char* fileName) {
	m_Data.clear();

	FIBITMAP* encodedData = FreeImage_Load(FIF_JPEG, fileName, 0);
	if (!encodedData) {
		return false;
	}

	m_Width = FreeImage_GetWidth(encodedData);
	m_Height = FreeImage_GetHeight(encodedData);
	m_BytesPerPixel = FreeImage_GetBPP(encodedData) / 8;

	RGBQUAD pixel;
	int pitch = FreeImage_GetPitch(encodedData);
	m_Data.resize(pitch * m_Height);
	for (UInt32 iy = 0; iy < m_Height; ++iy) {
		for (UInt32 ix = 0; ix < m_Width; ++ix) {
			verify( FreeImage_GetPixelColor(encodedData, ix, iy, &pixel) );

			int index = (ix * m_BytesPerPixel) + iy * pitch;
			m_Data[index] = pixel.rgbRed;
			m_Data[index + 1] = pixel.rgbGreen;
			m_Data[index + 2] = pixel.rgbBlue;
		}
	}

	FreeImage_Unload(encodedData);
	encodedData = NULL;

	return !m_Data.empty();
}

Bool ImageLoader_JPG::IsValid() const {
	return m_Data.size() > 0;
}

const Byte* ImageLoader_JPG::Data() const {
	return &m_Data[0];
}