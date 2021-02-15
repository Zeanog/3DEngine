#include "IModel.h"

#include "Images/ImageManager.h"
#include "Shaders/ShaderProgram_GLSL.h"

bool IModel::loadImage( const StaticString& textureName ) {
	m_Image = Singleton<ImageManager>::GetInstance()->Get( textureName );
	return m_Image != NULL;
}

void IModel::Position(const glm::vec3& pos) {
	m_Position = pos;
}

void IModel::Translate(const glm::vec3& delta) {
	m_Position += delta;
}

void IModel::Rotation(const TRotation& rot) {
	m_Rotation = rot;
}

void IModel::Rotate(const TRotation& delta) {
	m_Rotation = delta * m_Rotation;
}

void IModel::Rotate(Float32 pitch, Float32 yaw, Float32 roll) {
	Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	//m_Rotation.Rotate(yaw, pitch, roll);
}