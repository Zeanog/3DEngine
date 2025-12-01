#include "Mesh.h"

#include <windows.h>
#include <gl/gl.h>
#include "ModelLoaders/MeshLoader_FBX.h"
#include "Rendering/Joint.h"

Neo::Mesh::Mesh() {
}

Neo::Mesh::Mesh(const VertexBuffer& vb, const IndexBuffer& ib) {
	m_VertexBuffer = vb;
	m_IndexBuffer = ib;
}

Int32 Neo::Mesh::NumMaterials() const {
	return (Int32)m_Materials.Length();
}

Bool Neo::Mesh::RenderMaterial(int index) const {
	if (index < 0 || index >= (int)m_Materials.Length()) {
		return false;
	}

	//TODO: Handle multiple textures per material
	const AMaterial* matSlot = m_Materials[index];

	//One pass for each channel
	for (UInt32 ix = 0; ix < matSlot->Channels.Length(); ++ix)
	{
		matSlot->Channels[ix]->Bind();

		for(UInt32 iy = 0; iy < matSlot->Ranges.Length(); ++iy) {
			auto range = matSlot->Ranges[iy];
			if(range.VertCount == 0) {
				continue;
			}
			const UInt32*	indexStart = &m_IndexBuffer[range.StartIndex];
			UInt32			indexCount = range.VertCount;
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexStart);
			assert(!glGetError());
		}

		matSlot->Channels[ix]->Unbind();
	}
	return true;
}

Bool Neo::Mesh::RenderMaterial(int index, const List<StaticString>& channels) const {
	const AMaterial* matSlot = m_Materials[index];

	for (UInt32 ix = 0; ix < channels.Length(); ++ix) {
		auto channel = matSlot->ChannelMap[channels[ix]];
		channel->Bind();
	}

	for (UInt32 iy = 0; iy < matSlot->Ranges.Length(); ++iy) {
		auto range = matSlot->Ranges[iy];
		if (range.VertCount == 0) {
			continue;
		}
		const UInt32*	indexStart = &m_IndexBuffer[range.StartIndex];
		UInt32			indexCount = range.VertCount;
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexStart);
		assert(!glGetError());
	}

	for (UInt32 ix = 0; ix < channels.Length(); ++ix) {
		auto channel = matSlot->ChannelMap[channels[ix]];
		channel->Unbind();
	}

	return true;
}

Bool Neo::Mesh::Render() const {
	const UInt32* ibStart = &m_IndexBuffer[0];
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
	if (!keyFrame) {
		return false;
	}

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
	m_Materials = loader.Materials();

	m_Bounds += m_VertexBuffer.Positions();

	verify(m_Skeleton.UploadData(loader));

	auto& animNames = loader.AnimNames();
	m_AnimNames = animNames;//TODO: Find a more efficient way to do this

	auto& clips = loader.AnimationClips();
	m_AnimationClips = clips;//TODO: Find a more efficient way to do this

	return true;
}