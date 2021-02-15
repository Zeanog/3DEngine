#include "ImageLoader_TGA.h"
#include "System/File.h"

#include <map>
#include <vector>



struct ColorPalette {
	std::vector< Byte >	m_ColorTable;

	UInt32	GetNumEntries() const {
		return m_ColorTable.size();
	}

	Bool	GetPixelData( UInt32 index, Byte* buffer, UInt32 bufferLength ) const {
		assert( index < m_ColorTable.size() );
		memcpy_s( buffer, bufferLength, &m_ColorTable[index], m_ColorTable.size() - index );
		return true;
	}

	Byte&	operator[]( UInt32 index ) {
		return m_ColorTable[ index ];
	}

	const Byte&	operator[]( UInt32 index ) const {
		return m_ColorTable[ index ];
	}

	Byte&	operator[]( Int32 index ) {
		return m_ColorTable[ index ];
	}

	const Byte&	operator[]( Int32 index ) const {
		return m_ColorTable[ index ];
	}

	Bool	Load( const Header& header, File& file ) {
		assert( header.HasPalette() );

		const ColorMapSpecification& specification = header.ColorMapSpec;
		Byte* garbage = STACK_ALLOC( Byte, specification.Offset );

		assert( specification.Offset > 0 );

		file.Read( garbage, specification.Offset );
		
		m_ColorTable.reserve( specification.NumEntries );
		UInt32 bytesPerPixel = specification.BitsPerPixel / 8;
		
		Byte* pixelData = STACK_ALLOC( Byte, bytesPerPixel );

		for( UInt32 ix = 0; ix < specification.NumEntries; ++ix ) {
			file.Read( pixelData, bytesPerPixel );

			for( UInt32 iy = 0; iy < bytesPerPixel; ++iy ) {
				m_ColorTable.push_back( pixelData[iy] );
			}
		}

		return true;
	}
};

class ColorMap {
public:
	typedef std::vector<Byte>	TContainer;

protected:
	TContainer	m_Image;

public:
	ColorMap() {
	}

	virtual ~ColorMap() {
		m_Image.clear();
	}

	virtual Bool	Load( const Header& header, File& file ) = 0;

	const TContainer&	GetImage() const {
		return m_Image;
	}

protected:
	virtual void	UnpackPixel( Byte* dest, UInt32 destBytesPerPixel, Byte* src, UInt32 bytesPerPixel ) = 0;
};

class ColorMap_Palletted : public ColorMap {
public:

protected:
	const ColorPalette&	m_ColorPalette;

public:
	ColorMap_Palletted( const ColorPalette& palette ) : m_ColorPalette(palette) {
	}

	virtual Bool	Load( const Header& header, File& file ) {
		return true;
	}

protected:
	virtual void	UnpackPixel( Byte* dest, UInt32 destBytesPerPixel, Byte* src, UInt32 bytesPerPixel ) {
	}
};

class ColorMap_Raw : public ColorMap {
public:

protected:

public:
	ColorMap_Raw() {
	}

	virtual Bool	Load( const Header& header, File& file ) {
		assert( !header.HasPalette() );

		if( header.IsCompressed() ) {
			LoadCompressed( header, file );
		} else {
			LoadUncompressed( header, file );
		}

		return true;
	}

	Bool		LoadUncompressed( const Header& header, File& file ) {
		UInt32 pixelBufferSize = header.BytesPerPixel();
		Byte* pixelBuffer = STACK_ALLOC( Byte, pixelBufferSize );
		memset( pixelBuffer, 0, pixelBufferSize );

		UInt32 destBytesPerPixel = pixelBufferSize == 3 ? 3 : 4;
		UInt32 pixelsToRead = header.Width() * header.Height();
		UInt32 destIndex = 0;

		m_Image.resize( pixelsToRead * destBytesPerPixel );

		for( UInt32 pixelIndex = 0; pixelIndex < pixelsToRead; ++pixelIndex, destIndex += destBytesPerPixel ) {
			if( !file.Read(pixelBuffer, pixelBufferSize) ) {
				return false;
			}
			UnpackPixel( &(m_Image[destIndex]), destBytesPerPixel, pixelBuffer, pixelBufferSize );
		}

		return true;
	}

	Bool		LoadCompressed( const Header& header, File& file ) {
		UInt32 pixelBufferSize = header.BytesPerPixel() + header.IsCompressed();
		Byte* pixelBuffer = STACK_ALLOC( Byte, pixelBufferSize );
		memset( pixelBuffer, 0, pixelBufferSize );

		UInt32 destBytesPerPixel = header.BytesPerPixel() == 3 ? 3 : 4;
		UInt32 pixelsToRead = header.Width() * header.Height();
		UInt32 j = 0;
		UInt32 destIndex = 0;

		m_Image.resize( pixelsToRead * destBytesPerPixel );

		for( UInt32 pixelIndex = 0; pixelIndex < pixelsToRead; ) {
			if( !file.Read(pixelBuffer, pixelBufferSize) ) {
				return false;
			}

			j = pixelBuffer[0] & 0x7f;
			UnpackPixel( &(m_Image[destIndex]), destBytesPerPixel, &(pixelBuffer[1]), header.BytesPerPixel() );
			++pixelIndex;
			assert( pixelIndex <= pixelsToRead );
			destIndex += destBytesPerPixel;

			if (pixelBuffer[0] & 0x80) {         /* RLE chunk */
				for( UInt32 iz = 0; iz < j; ++iz ) {
					UnpackPixel( &(m_Image[destIndex]), destBytesPerPixel, &(pixelBuffer[1]), header.BytesPerPixel() );
					++pixelIndex;
					assert( pixelIndex <= pixelsToRead );
					destIndex += destBytesPerPixel;
				}
			} else {                   /* Normal chunk */
				for( UInt32 iz = 0; iz < j; ++iz ) {
					if( !file.Read(pixelBuffer, header.BytesPerPixel()) ) {
						return false;
					}

					UnpackPixel( &(m_Image[destIndex]), destBytesPerPixel, pixelBuffer, header.BytesPerPixel() );
					++pixelIndex;
					assert( pixelIndex <= pixelsToRead );
					destIndex += destBytesPerPixel;
				}
			}
		}

		return true;
	}

protected:
	virtual void	UnpackPixel( Byte* dest, UInt32 destBytesPerPixel, Byte* src, UInt32 srcBytesPerPixel ) {
		assert( destBytesPerPixel >= srcBytesPerPixel );

		switch(  srcBytesPerPixel ) {
			case 2: {
				dest[0] = (src[1] & 0x7c) << 1;
				dest[1] = ((src[1] & 0x03) << 6) | ((src[0] & 0xe0) >> 2);
				dest[2] = (src[0] & 0x1f) << 3;
				dest[3] = (src[1] & 0x80);
				break;
			}

			case 3: {
				dest[0] = src[2];
				dest[1] = src[1];
				dest[2] = src[0];
				break;
			}

			case 4: {
				dest[0] = src[2];
				dest[1] = src[1];
				dest[2] = src[0];
				dest[3] = src[3];
				/*for (int ix = 0; ix < 4; ++ix) {
					dest[ix] = src[ix];
				}*/
				
				break;
			}

			default:
				assert( 0 );
		}
	}
};

ImageLoader_TGA::ImageLoader_TGA() {
	m_ColorMap = NULL;
}

ImageLoader_TGA::~ImageLoader_TGA() {
	DeletePtr( m_ColorMap );
}

Bool ImageLoader_TGA::Load( const Char* fileName ) {
	ColorPalette	colorPalette;

	File file;

	Clear();

	if( !file.Open(fileName, "rb") ) {
		return false;
	}

	if( !m_Header.Load(file) ) {
		return false;
	}

	m_BytesPerPixel = m_Header.BytesPerPixel();
	m_Width = m_Header.Width();
	m_Height = m_Header.Height();

	if(m_Header.HasPalette() && !colorPalette.Load(m_Header, file) ) {
		return false;
	}

	if(m_Header.HasPalette() ) {
		m_ColorMap = new ColorMap_Palletted( colorPalette );
	} else {
		m_ColorMap = new ColorMap_Raw();
	}

	m_ColorMap->Load(m_Header, file );

	return true;
}

Bool ImageLoader_TGA::IsValid() const {
	UInt32 expectedSize = ImageHeight() * ImageWidth() * m_BytesPerPixel;
	return expectedSize == m_ColorMap->GetImage().size();
}

const Byte* ImageLoader_TGA::Data() const {
	assert( m_ColorMap );
	const Byte* ptr = &(m_ColorMap->GetImage()[0]);
	return ptr;
}

void ImageLoader_TGA::Clear() {
	TSuper::Clear();

	DeletePtr( m_ColorMap );
}