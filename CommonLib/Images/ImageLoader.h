#pragma once

#include "System/Typedefs.h"
#include "System/StaticString.h"
#include "System/File.h"

class ImageLoader {
	CLASS_TYPEDEFS( ImageLoader );

protected:
	UInt32			m_Width;
	UInt32			m_Height;
	UInt32			m_BytesPerPixel;

public:
	DECLARE_GETSET(BytesPerPixel)

public:
	ImageLoader() {
		m_Width = 0;
		m_Height = 0;
		m_BytesPerPixel = 0;
	}

	virtual Bool	Load( const Char* fileName ) = 0;

	virtual Bool	IsValid() const = 0;

	virtual const Byte*		Data() const = 0;

	virtual UInt32	ImageHeight() const {
		return m_Height;
	}

	virtual UInt32	ImageWidth() const {
		return m_Width;
	}

	virtual Bool	Load( const StaticString& fileName ) {
		return Load( fileName.CStr() );
	}

	virtual Bool	Load( const String& fileName ) {
		return Load( fileName.CStr() );
	}

	virtual void	Clear() {
		m_Width = 0;
		m_Height = 0;
		m_BytesPerPixel = 0;
	}
};