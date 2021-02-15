#include "Mesh.h"

#include <windows.h>
#include <gl/gl.h>

#include "ModelLoaders/ModelLoader_FBX.h"
#include "Rendering/Joint.h"

#include <glm/gtx/transform.hpp>

void Neo::Mesh::MaterialSlot_ColorChannel::Bind() const {
	TSuper::Bind();
}

void Neo::Mesh::MaterialSlot_ColorChannel::Unbind() const {
	TSuper::Unbind();
}

Neo::Mesh::Mesh() {
}

Neo::Mesh::Mesh(const VertexBuffer& vb, const IndexBuffer& ib) {
	m_VertexBuffer = vb;
	m_IndexBuffer = ib;
}

Int32 Neo::Mesh::NumMaterials() const {
	return (Int32)m_MaterialSlots.Length();
}

Bool Neo::Mesh::RenderMaterial(int index) const {
	if (index < 0 || index >= (int)m_MaterialSlots.Length()) {
		return false;
	}

	const AMaterialSlot* matSlot = m_MaterialSlots[index];

	matSlot->Bind();

	const UInt32*	ibStart = &m_IndexBuffer[matSlot->Index];
	Int32			indexCount = matSlot->PolyCount * 3;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, ibStart);
	assert(!glGetError());

	matSlot->Unbind();

	return true;
}

Bool Neo::Mesh::Render() const {
	const UInt32*	ibStart = &m_IndexBuffer[0];
	Int32			indexCount = m_IndexBuffer.Count();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, ibStart);
	assert(!glGetError());
	return true;
}

Bool Neo::Mesh::RenderJoints() const {
	AnimationClip* clip = m_AnimationClips[0];
	AnimKeyFrame* keyFrame = clip->GetFrame(0);

	return RenderJoints(keyFrame);
}

Bool Neo::Mesh::RenderJoints(const AnimKeyFrame* keyFrame) const {
	glm::mat4 transform;
	for (UInt32 ix = 0; ix < m_Skeleton.NumJoints(); ++ix) {
		transform = keyFrame->GetGlobalTransform(ix, m_Skeleton);
		Joint::Render(transform, 0.1f, 64);
	}

	return true;
}

void Neo::Mesh::PreRender(VertexBuffer::VertexAttributes attribs) const {
	m_VertexBuffer.Bind(attribs);
	assert(!glGetError());
}

void Neo::Mesh::PostRender() const {
	m_VertexBuffer.Unbind();
	assert(!glGetError());
}

Bool Neo::Mesh::UploadData(const AMeshLoader& loader) {
	m_VertexBuffer = loader.VB();
	m_IndexBuffer = loader.IB();
	m_MaterialSlots = loader.MaterialSlots();

	m_Bounds += m_VertexBuffer.Positions();
	
	verify( m_Skeleton.UploadData(loader) );
	for (UInt32 ix = 0; ix < loader.AnimationClips().Length(); ++ix) {
		m_AnimationClips.Add(loader.AnimationClips()[ix]);
	}
	
	return true;
}