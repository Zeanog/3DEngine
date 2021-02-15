#include "VertexBuffer.h"

#include "Shaders/ShaderProgram_GLSL.h"

VertexBuffer::VertexBuffer() {
}

Bool VertexBuffer::Bind(VertexAttributes attribs) const {
	if( (attribs & VertexAttributes::TexCoordsAttrib) != 0 ) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		assert(!glGetError());

		assert(m_TextureCoordinates.Length() > 0);
		glTexCoordPointer(2, GL_FLOAT, 0, &m_TextureCoordinates[0]);
		assert(!glGetError());
	}

	if ((attribs & VertexAttributes::NormalAttrib) != 0) {
		glEnableClientState(GL_NORMAL_ARRAY);
		assert(!glGetError());

		assert(m_Normals.Length() > 0);
		glNormalPointer(GL_FLOAT, 0, &m_Normals[0]);
		assert(!glGetError());
	}

	if ((attribs & VertexAttributes::PositionAttrib) != 0) {
		glEnableClientState(GL_VERTEX_ARRAY);
		assert(!glGetError());

		assert(m_Positions.Length() > 0);
		glVertexPointer(3, GL_FLOAT, 0, &m_Positions[0]);
		assert(!glGetError());
	}
	
	return true;
}

void VertexBuffer::Unbind() const {
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}