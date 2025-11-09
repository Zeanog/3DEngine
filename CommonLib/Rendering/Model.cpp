#include "Model.h"

#include "Math/MathUtils.h"
#include "Images/ImageManager.h"
#include "ModelLoaders/MeshManager.h"
#include "Shaders/ShaderProgram_GLSL.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>

Model::Model(const StaticString& meshPath) : m_Rotation(glm::identity<TRotation>()), m_Position(glm::zero<glm::vec3>())
{
	m_Mesh = Singleton<MeshManager>::GetInstance()->Get(meshPath);

	if (m_Mesh->NumClips() > 0) {
		m_DebugAnimPlayer.Start(m_Mesh->GetClip(0));
	}
}

Model::~Model() {
}

void Model::Render(ShaderProgram_GLSL& program) const {
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
		m_Mesh->RenderMaterial(ix);//TODO: Handle unique programs per material
	}

	m_Mesh->PostRender();

	program.StopUsing();

	glPopMatrix();
}

void Model::RenderJoints(ShaderProgram_GLSL& program, Float32 deltaTime) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(glm::identity<glm::mat4x4>());
	localTransform = glm::translate(localTransform, m_Position);
	localTransform = localTransform * glm::mat4x4(m_Rotation);
	glMultMatrixf(glm::value_ptr(localTransform));
	assert(!glGetError());

	m_DebugAnimPlayer.Update(deltaTime);

	program.StartUsing();
	program.LinkUniform("vColor", glm::vec4(0.0f, 1.0f, 0.5f, 1.0f));

	m_Mesh->RenderJoints(m_DebugAnimPlayer.GetCurrentFrame());

	program.StopUsing();

	glPopMatrix();
}

#include "Rendering/ModelLoaders/ModelLoader.h"
Bool Model::UploadData(ModelLoader& loader) {
	m_Mesh = loader.Mesh();
	m_InvertedNormals = loader.InvertNormals();
	m_ShaderProgram = loader.GetShaderProgram();
	m_RequiredChannels = loader.GetRequiredChannels();
	m_ShadowProgram = loader.GetShadowPrograms();
	return true;
}