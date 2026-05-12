#include "VertexBuffer.h"
#include <fbxsdk/core/base/fbxstringlist.h>

Neo::VertexBuffer::VertexBuffer() {
}

Bool Neo::VertexBuffer::Bind(VertexAttributes attribs) const {
	if( (attribs & VertexAttributes::TexCoordsAttrib0) != 0 ) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		assert(!glGetError());

		assert(m_TextureCoordinates.Size() > 0);
		auto& texCoordList = m_TextureCoordinates.Begin()->second;
		auto coordDimension = sizeof(decltype(m_TextureCoordinates)::TValue::TData) / sizeof(Float32);
		glTexCoordPointer(coordDimension, GL_FLOAT, 0, &texCoordList[0]);
		assert(!glGetError());
	}

	if ((attribs & VertexAttributes::TexCoordsAttrib1) != 0) {
		assert(0);
		/*glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		assert(!glGetError());

		assert(m_TextureCoordinates.Size() > 1);
		auto&& iter = m_TextureCoordinates.Begin();
		auto& texCoordList = (++iter)->second;
		auto coordDimension = sizeof(decltype(m_TextureCoordinates)::TValue::TElement) / sizeof(Float32);
		glTexCoordPointer(coordDimension, GL_FLOAT, 0, &texCoordList[0]);
		assert(!glGetError());*/
	}

	if ((attribs & VertexAttributes::TexCoordsAttrib2) != 0) {
		assert(0);
		/*glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		assert(!glGetError());

		assert(m_TextureCoordinates.Size() > 1);
		auto&& iter = m_TextureCoordinates.Begin();
		auto& texCoordList = (++iter)->second;
		texCoordList = (++iter)->second;
		auto coordDimension = sizeof(decltype(m_TextureCoordinates)::TValue::TElement) / sizeof(Float32);
		glTexCoordPointer(coordDimension, GL_FLOAT, 0, &texCoordList[0]);
		assert(!glGetError());*/
	}

	if ((attribs & VertexAttributes::NormalAttrib) != 0) {
		glEnableClientState(GL_NORMAL_ARRAY);
		assert(!glGetError());

		assert(m_Normals.Length() > 0);
		glNormalPointer(GL_FLOAT, 0, &m_Normals[0]);
		assert(!glGetError());
	}

	if ((attribs & VertexAttributes::TangentAttrib) != 0) {
		assert(0);
		/*glEnableClientState(GL_NORMAL_ARRAY);
		assert(!glGetError());

		assert(m_Normals.Length() > 0);
		glNormalPointer(GL_FLOAT, 0, &m_Normals[0]);
		assert(!glGetError());*/
	}
	
	if ((attribs & VertexAttributes::BiTangentAttrib) != 0) {
		assert(0);
		/*glEnableClientState(GL_NORMAL_ARRAY);
		assert(!glGetError());

		assert(m_Normals.Length() > 0);
		glNormalPointer(GL_FLOAT, 0, &m_Normals[0]);
		assert(!glGetError());*/
	}

	if ((attribs & VertexAttributes::PositionAttrib) != 0) {
		glEnableClientState(GL_VERTEX_ARRAY);
		assert(!glGetError());

		assert(m_Positions.Length() > 0);
		auto coordDimension = sizeof(decltype(m_Positions)::TData) / sizeof(Float32);
		glVertexPointer(coordDimension, GL_FLOAT, 0, &m_Positions[0]);
		assert(!glGetError());
	}
	
	return true;
}

void Neo::VertexBuffer::Unbind() const {
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState - TangentAttrib
	//glDisableClientState - BiTangentAttrib
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Neo::VertexBuffer::Resize(Int32 size, const fbxsdk::FbxStringList& texCoordGroups) {
	m_Positions.Resize(size);
	m_Normals.Resize(size);
	m_Tangents.Resize(size);
	m_BiTangents.Resize(size);
	for (Int32 ix = 0; ix < texCoordGroups.GetCount(); ++ix) {
		const auto texCoordName = texCoordGroups.GetItemAt(ix)->mString.Buffer();
		assert(!m_TextureCoordinates.Contains(texCoordName));
		m_TextureCoordinates.Add(texCoordName, VertexArrayObject<VertexBuffer::TTextureCoordinate>());
		m_TextureCoordinates[texCoordName].Resize(size);
	}
}