#include "Image.h"
#include "cmath"
#include <gl/glew.h>
#include <MathUtils.h>

Neo::Image::~Image() {
	if( m_Id != 0 ) {
		glDeleteTextures( 1, &m_Id );
		m_Id = 0;
	}
}

void Neo::Image::Bind() const {
	assert( m_Id );
	glBindTexture( GL_TEXTURE_2D, m_Id );

	GLenum error = glGetError();

	assert( !error);
}

void Neo::Image::Unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum error = glGetError();

	assert(!error);
}

Bool Neo::Image::UploadData( const ImageLoader& loader ) {
	if( !m_Id ) {
		glGenTextures( 1, &m_Id );
		int err = glGetError();
		if (err) {
			const Char* erMsg = (Char*)glewGetErrorString(err);
			assert(0);
		}
	}

	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	assert( !glGetError() );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	assert( !glGetError() );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	assert( !glGetError() );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	assert( !glGetError() );

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	assert( !glGetError() );

	assert( loader.IsValid() );
	//assert( IsPowerOfTwo(loader.ImageWidth()) );
	//assert( IsPowerOfTwo(loader.ImageHeight()) );
	auto w = MathUtils::NearestPowerOfTwo(loader.ImageWidth());
	auto h = MathUtils::NearestPowerOfTwo(loader.ImageHeight());

	GLenum format = loader.BytesPerPixel() <= 3 ? GL_RGB: GL_RGBA;
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, format, GL_UNSIGNED_BYTE, loader.Data() );
	assert( !glGetError() );

	return true;
}