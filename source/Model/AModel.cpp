#include "AModel.h"

#include "Images/ImageManager.h"
#include "Shaders/ShaderProgram_GLSL.h"

bool AModel::LoadImage( const StaticString& textureName ) {
	m_Image = Singleton<ImageManager>::GetInstance()->Get( textureName );
	return m_Image != NULL;
}

void AModel::Position(const glm::vec3& pos) {
	m_Position = pos;
}

void AModel::Translate(const glm::vec3& delta) {
	m_Position += delta;
}

void AModel::Rotation(const TRotation& rot) {
	m_Rotation = rot;
}

void AModel::Rotate(const TRotation& delta) {
	m_Rotation = delta * m_Rotation;
}

void AModel::Rotate(Float32 pitch, Float32 yaw, Float32 roll) {
	Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	//m_Rotation.Rotate(yaw, pitch, roll);
}