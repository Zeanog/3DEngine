#pragma once

#include "System/Typedefs.h"
#include "ImageLoader.h"

#include <vector>

class ImageLoader_RAW : public ImageLoader {
	INHERITED_CLASS_TYPEDEFS( ImageLoader_RAW, ImageLoader );

public:
	typedef std::vector<Byte>	TContainer;

protected:
	TContainer		m_ColorMap;

public:
	ImageLoader_RAW();

	virtual ~ImageLoader_RAW();

	using TSuper::Load;
	virtual Bool	Load( const Char* fileName ) override;

	virtual Bool	IsValid() const override;

	virtual const Byte*		Data() const override;

	virtual void	Clear() override {
		m_ColorMap.clear();
	}
};