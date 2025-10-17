#pragma once

#include "Images\ImageLoader.h"
#include "System/List.h"

class ImageLoader_PNG : public ImageLoader {
public:

protected:
	List<Byte>	m_Data;

public:
	virtual Bool	Load(const Char* fileName) override;

	virtual Bool	IsValid() const override;

	virtual const Byte*		Data() const override;
};