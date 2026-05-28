#include "Model.h"

#include "Math/MathUtils.h"
#include "Images/ImageManager.h"
#include "ModelLoaders/MeshManager.h"
#include "Shaders/ShaderProgram_GLSL.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>

Model::Model(const StaticString& meshPath)
{
	/*m_Mesh = Singleton<MeshManager>::GetInstance()->Get(meshPath);

	if (m_Mesh->NumClips() > 0) {
		m_DebugAnimPlayer.Start(m_Mesh->GetClip(0));
	}*/
}

Model::~Model() {
}

void Model::Render(ShaderProgram_GLSL& program) const {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(m_Transform);
	localTransform = glm::scale(localTransform, Scale());
	glMultMatrixf(glm::value_ptr(localTransform));
	assert(!glGetError());

	for (auto ix = 0; ix < m_Mesh->NumMaterials(); ++ix) {
		program.StartUsing();

		m_Mesh->PreRender((Neo::VertexBuffer::VertexAttributes)(Neo::VertexBuffer::VertexAttributes::PositionAttrib | Neo::VertexBuffer::VertexAttributes::NormalAttrib | Neo::VertexBuffer::VertexAttributes::TexCoordsAttrib0));
		m_Mesh->RenderMaterial(ix, program.RequiredChannels());
		m_Mesh->PostRender();

		program.StopUsing();
	}

	glPopMatrix();
}

void Model::RenderJoints(ShaderProgram_GLSL& program, Float32 deltaTime) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(m_Transform);
	localTransform = glm::scale(localTransform, Scale());
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
	if (m_Mesh->NumClips() > 0) {
		m_DebugAnimPlayer.Start(m_Mesh->GetClip(0));
	}
	m_ShaderProgram = loader.GetShaderProgram();
	m_ShadowProgram = loader.GetShadowPrograms();
	return true;
}