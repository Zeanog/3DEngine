#pragma once

#include "System/Typedefs.h"
#include "ImageLoader.h"

#include <vector>

class ImageLoader_RAW : public ImageLoader {
	INHERITEDCLASS_TYPEDEFS( ImageLoader_RAW, ImageLoader );

public:
	typedef std::vector<Byte>	TContainer;

protected:
	TContainer		m_ColorMap;

public:
	ImageLoader_RAW();

	virtual ~ImageLoader_RAW();

	using TSuper::Load;
	virtual Bool	Load( const Char* fileName );

	virtual Bool	IsValid() const;

	virtual const Byte*		Data() const;

	virtual void	Clear() {
		m_ColorMap.clear();
	}
};