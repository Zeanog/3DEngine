#pragma once

#include "System\Typedefs.h"
#include "Images\ImageLoader.h"
#include <vector>

class ImageLoader_JPG : public ImageLoader {
public:

protected:
	std::vector<Byte>	m_Data;

public:
	virtual Bool	Load(const Char* fileName) override;

	virtual Bool	IsValid() const override;

	virtual const Byte*		Data() const override;
};