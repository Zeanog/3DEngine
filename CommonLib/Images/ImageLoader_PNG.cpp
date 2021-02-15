#include "ImageLoader_PNG.h"

#include "LodePNG/lodepng.h"

void SwapByteChunk(Byte* lhs, Byte* rhs, UInt32 chunkSize) {
	Byte* temp = STACK_ALLOC(Byte, chunkSize);
	memcpy_s(temp, chunkSize, lhs, chunkSize);
	memcpy_s(lhs, chunkSize, rhs, chunkSize);
	memcpy_s(rhs, chunkSize, temp, chunkSize);
}

Bool ImageLoader_PNG::Load(const Char* fileName) {
	m_Data.clear();

	std::vector<unsigned char> png;
	lodepng::State state; //optionally customize this one

	UInt32 error = 0;

	error = lodepng::load_file(png, fileName); //load the image file with given filename
	if (error) {
		return false;
	}

	error = lodepng::decode(m_Data, m_Width, m_Height, state, png);
	if (error) {
		return false;
	}

	m_BytesPerPixel = state.info_raw.colortype == LCT_RGBA ? 4 : 3;

	//Flip Vertically
	UInt32 topIndex = 0;
	UInt32 btmIndex = 0;
	UInt32 halfHeight = m_Height / 2;
	UInt32 widthInBytes = m_Width * m_BytesPerPixel;
	for (UInt32 rowIndex = 0; rowIndex < halfHeight; ++rowIndex) {
		topIndex = rowIndex * widthInBytes;
		btmIndex = ((m_Height - 1) - rowIndex) * widthInBytes;

		SwapByteChunk(&m_Data[topIndex], &m_Data[btmIndex], widthInBytes);
	}
	//Flip Vertically

	return true;
}

Bool ImageLoader_PNG::IsValid() const {
	return m_Data.size() > 0;
}

const Byte* ImageLoader_PNG::Data() const {
	return &m_Data[0];
}