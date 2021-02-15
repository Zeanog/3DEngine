#include "Camera.h"

#include <windows.h>
#include <gl/GL.h>

#include <glm/gtc/type_ptr.hpp>

Camera::Camera() : m_Rotation(glm::identity<TRotation>()) {
}

Camera::Camera(const glm::mat3x3& rot, const glm::vec3& pos) : Camera() {
	GlobalPosition( pos );
	m_Rotation = rot;
}

void Camera::Update(Float32 deltaTime) {

}

void Camera::LinkTransform() const {
	glMultMatrixf(glm::value_ptr(ToMat4x4()));
}

void Camera::LocalPosition(const glm::vec3& pos) {
	m_Position = pos;
}

void Camera::GlobalPosition(const glm::vec3& pos) {
	m_Position = pos;
}

void Camera::Translate(const glm::vec3& delta) {
	m_Position += delta;
}

void Camera::Rotation(const TRotation& rot) {
	m_Rotation = rot;
}

void Camera::Rotate(const TRotation& delta) {
	m_Rotation = delta * m_Rotation;
}

void Camera::Rotate(Float32 pitch, Float32 yaw, Float32 roll) {
	Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	//m_Rotation.Rotate(yaw, pitch, roll);
}