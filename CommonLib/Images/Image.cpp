#include "Image.h"

#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

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

#include <math.h>

UInt32 Neo::Image::NearestPowerOfTwo(UInt32 val) {
	Float32 pos = ceilf((Float32)log2(val));//  (ceiling of log n with base 2)
	return (UInt32)powf(2, pos);
}

Int32 Neo::Image::NearestPowerOfTwo(Int32 val) {
	Float32 pos = ceilf((Float32)log2(val));//  (ceiling of log n with base 2)
	return (Int32)powf(2, pos);
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

	GLenum format = loader.BytesPerPixel() <= 3 ? GL_RGB: GL_RGBA;
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, loader.ImageWidth(), loader.ImageHeight(), 0, format, GL_UNSIGNED_BYTE, loader.Data() );
	assert( !glGetError() );

	return true;
}