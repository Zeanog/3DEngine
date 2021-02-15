#pragma once

#include "System/TypeDefs.h"
#include "ImageLoader.h"

struct ColorMapSpecification {
	UInt16	Offset;
	UInt16	NumEntries;
	Byte	BitsPerPixel;

	Bool	Load(File& file) {
		if (!file.Read(Offset)) {
			return false;
		}

		if (!file.Read(NumEntries)) {
			return false;
		}

		if (!file.Read(BitsPerPixel)) {
			return false;
		}

		return true;
	}
};

struct ImageSpecification {
	Int16	OriginX;
	Int16	OriginY;
	Int16	Width;
	Int16	Height;
	Byte	PixelDepth;
	Byte	Descripter;

	Bool	Load(File& file) {
		if (!file.Read(OriginX)) {
			return false;
		}

		if (!file.Read(OriginY)) {
			return false;
		}

		if (!file.Read(Width)) {
			return false;
		}

		if (!file.Read(Height)) {
			return false;
		}

		if (!file.Read(PixelDepth)) {
			return false;
		}

		if (!file.Read(Descripter)) {
			return false;
		}

		return true;
	}

	Byte	AlphaChannelDepth() const {
		return Descripter & 0xF;
	}

	Byte	Direction() const {
		return (Descripter >> 4) & 0x3;
	}
};

struct Header {
	Byte					ColorMapType;
	Byte					ImageType;
	ColorMapSpecification	ColorMapSpec;
	ImageSpecification		ImageSpec;

	Bool	HasPalette() const {
		return !!(ColorMapType & 0x1);
	}

	Bool	IsCompressed() const {
		return ImageType != 2 && ImageType == 10;
	}

	UInt32	BytesPerPixel() const {
		return ImageSpec.PixelDepth / 8;
	}

	UInt32	Width() const {
		return ImageSpec.Width;
	}

	UInt32	Height() const {
		return ImageSpec.Height;
	}

	Bool	Load(File& file) {
		Byte	idLength = 0;

		if (!file.Read(idLength)) {
			return false;
		}

		if (!file.Read(ColorMapType)) {
			return false;
		}

		if (!file.Read(ImageType)) {
			return false;
		}

		if (!ColorMapSpec.Load(file)) {
			return false;
		}

		if (!ImageSpec.Load(file)) {
			return false;
		}

		if (idLength > 0) {
			Byte* id = STACK_ALLOC(Byte, idLength);
			if (!file.Read(id, idLength)) {
				return false;
			}
		}

		return true;
	}
};

class ColorMap;

class ImageLoader_TGA : public ImageLoader {
	INHERITEDCLASS_TYPEDEFS(ImageLoader_TGA, ImageLoader);

public:

protected:
	ColorMap*		m_ColorMap;

	Header			m_Header;

public:
	ImageLoader_TGA();

	virtual ~ImageLoader_TGA();

	using TSuper::Load;
	virtual Bool	Load(const Char* fileName);

	virtual Bool	IsValid() const;

	virtual const Byte*		Data() const;

	virtual void	Clear();
};