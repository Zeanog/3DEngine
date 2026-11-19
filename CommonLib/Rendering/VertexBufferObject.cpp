#include "VertexBufferObject.h"

AVertexBufferObject::AVertexBufferObject() {
	
}

AVertexBufferObject::~AVertexBufferObject() {
	if (m_VBO > 0) {
		glDeleteBuffers(1, &m_VBO);
		assert(!glGetError());
		m_VBO = 0;
	}
}

void AVertexBufferObject::UploadData(UInt32 location) {//TODO: Bad name as we dont upload anything
	assert(m_VBO == 0);
	glGenBuffers(1, &m_VBO);
	assert(!glGetError());
}