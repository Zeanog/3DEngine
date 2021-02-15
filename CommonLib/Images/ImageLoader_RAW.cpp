#include "ImageLoader_RAW.h"
#include "System/File.h"

ImageLoader_RAW::ImageLoader_RAW() {
}

ImageLoader_RAW::~ImageLoader_RAW() {
	m_ColorMap.clear();
}

Bool ImageLoader_RAW::Load( const Char* fileName ) {
	File	file;

	Clear();

	if( !file.Open(fileName, "rb") ) {
		return false;
	}

	m_ColorMap.resize( file.Length() );

	m_Width = (UInt32)sqrt( file.Length() / 3.0f );
	m_Height = m_Width;
	m_BytesPerPixel = 3;

	return file.Read( &m_ColorMap[0], m_ColorMap.size() );
}

Bool ImageLoader_RAW::IsValid() const {
	return m_ColorMap.size() > 0;
}

const Byte* ImageLoader_RAW::Data() const {
	return &(m_ColorMap[0]);
}