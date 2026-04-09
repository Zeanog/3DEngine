#include "VertexBufferObject.h"

AVertexBufferObject::AVertexBufferObject() {
	assert(m_VBO <= 0);
	glGenBuffers(1, &m_VBO);
}

AVertexBufferObject::~AVertexBufferObject() {
	glDeleteBuffers(1, &m_VBO);
	m_VBO = 0;
}