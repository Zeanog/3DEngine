#include "Mesh.h"
#include "ModelLoaders/MeshLoader_FBX.h"
#include "Rendering/Joint.h"
#include <windows.h>
#include <gl/gl.h>

Neo::Mesh::Mesh() {
}

Neo::Mesh::Mesh(const Neo::VertexBuffer& vb, const Neo::IndexBuffer& ib) {
	m_VertexBuffer = vb;
	m_IndexBuffer = ib;
}

Int32 Neo::Mesh::NumMaterials() const {
	return (Int32)m_Materials.Length();
}

//Bool Neo::Mesh::RenderMaterial(int index) const {
//	if (index < 0 || index >= (int)m_Materials.Length()) {
//		return false;
//	}
//
//	//TODO: Handle multiple textures per material
//	const AMaterial* matSlot = m_Materials[index];
//
//	//One pass for each channel
//	for (UInt32 ix = 0; ix < matSlot->Channels.Length(); ++ix)
//	{
//		matSlot->Channels[ix]->Bind();
//
//		for(UInt32 iy = 0; iy < matSlot->Ranges.Length(); ++iy) {
//			auto range = matSlot->Ranges[iy];
//			if(range.IndexCount == 0) {
//				continue;
//			}
//			const UInt32*	indexStart = &m_IndexBuffer[range.StartIndex];
//			UInt32			indexCount = range.IndexCount;
//			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexStart);
//			assert(!glGetError());
//		}
//
//		matSlot->Channels[ix]->Unbind();
//	}
//	return true;
//}

Bool Neo::Mesh::RenderMaterial(int index, const ShaderProgram_GLSL& program) const {
	const AMaterial* matSlot = m_Materials[index];
	auto& requiredChannels = program.RequiredChannels();

	for (UInt32 ix = 0; ix < requiredChannels.Length(); ++ix) {
		if (!matSlot->ChannelMap.Contains(requiredChannels[ix])) {
			continue;
		}
		auto channel = matSlot->ChannelMap[requiredChannels[ix]];
		channel->Bind(ix, program);
	}

	//for (UInt32 iy = 0; iy < matSlot->Ranges.Length(); ++iy) {
		auto& range = matSlot->Range;
		assert(range.FinalIndex() < m_IndexBuffer.Count());
		const UInt32* indexStart = m_IndexBuffer.Data() + range.StartIndex;
		UInt32			indexCount = range.IndexCount;
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexStart);
		assert(!glGetError());
	//}

	for (UInt32 ix = 0; ix < requiredChannels.Length(); ++ix) {
		if (!matSlot->ChannelMap.Contains(requiredChannels[ix])) {
			continue;
		}
		auto channel = matSlot->ChannelMap[requiredChannels[ix]];
		channel->Unbind(ix, program);
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
	assert(m_Skeleton.NumJoints() == keyFrame->FrameDataCount());
	for (UInt32 ix = 0; ix < keyFrame->FrameDataCount(); ++ix) {
		transform = keyFrame->GetGlobalTransform(ix, m_Skeleton);
		Joint::Render(transform, 0.5f, 64);
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