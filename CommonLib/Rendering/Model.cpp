#include "Model.h"

#include "Math/MathUtils.h"
#include "Images/ImageManager.h"
#include "ModelLoaders/MeshManager.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "System/JsonSerializer.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>

Model::Model(const StaticString& meshPath) : m_Rotation(glm::identity<TRotation>()), m_Position(glm::zero<glm::vec3>())
{
	m_Mesh = Singleton<MeshManager>::GetInstance()->Get(meshPath);

	//m_DebugAnimPlayer.Start(m_Mesh->GetClip(0));
}

Model::~Model() {
}

void Model::Render(const ShaderProgram_GLSL& program) const {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	program.StartUsing();

	glm::mat4x4 localTransform(glm::identity<glm::mat4x4>());
	localTransform = glm::translate(localTransform, m_Position);
	localTransform = localTransform * glm::mat4x4(m_Rotation);
	glMultMatrixf(glm::value_ptr(localTransform));
	assert(!glGetError());

	m_Mesh->PreRender((VertexBuffer::VertexAttributes)(VertexBuffer::VertexAttributes::PositionAttrib | VertexBuffer::VertexAttributes::NormalAttrib | VertexBuffer::VertexAttributes::TexCoordsAttrib));

	for (int ix = 0; ix < m_Mesh->NumMaterials(); ++ix) {
		m_Mesh->RenderMaterial(ix);
	}

	m_Mesh->PostRender();

	program.StopUsing();

	glPopMatrix();
}

void Model::RenderJoints(Float32 deltaTime) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(glm::identity<glm::mat4x4>());
	localTransform = glm::translate(localTransform, m_Position);
	localTransform = localTransform * glm::mat4x4(m_Rotation);
	glMultMatrixf(glm::value_ptr(localTransform));
	assert(!glGetError());

	m_DebugAnimPlayer.Update(deltaTime);

	//m_Mesh->PreRender((VertexBuffer::VertexAttributes)(VertexBuffer::VertexAttributes::PositionAttrib | VertexBuffer::VertexAttributes::NormalAttrib | VertexBuffer::VertexAttributes::TexCoordsAttrib));

	//m_Mesh->Render();
	m_Mesh->RenderJoints(m_DebugAnimPlayer.GetCurrentFrame());

	//m_Mesh->PostRender();

	glPopMatrix();
}

Bool Model::UploadData(const AModelLoader& loader) {
	return true;
}