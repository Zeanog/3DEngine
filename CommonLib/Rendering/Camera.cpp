#include "Camera.h"
#include "Math\MathUtils.h"
#include <windows.h>
#include <gl/GL.h>

#include <glm/gtc/type_ptr.hpp>

Camera::Camera() : m_Position(), m_Rotation(glm::identity<ICamera::TRotation>()) {
}

Camera::Camera(const glm::mat3x3& rot, const ICamera::TPosition& pos) : Camera() {
	Position( pos );
	m_Rotation = rot;
}

void Camera::Update(Float32 deltaTime) {

}

void Camera::LinkTransform() const {
	//auto transform = glm::mat4x4(m_Rotation);
	glm::mat4x4 transform;
	//ToMat4x4(transform);
	transform = glm::mat4x4(m_Rotation);
	transform = glm::translate(transform, m_Position);
	glMultMatrixf(glm::value_ptr(transform));
}

void Camera::Translate(const TPosition& delta) {
	m_Position += delta;
}

void Camera::TranslateRelative(const TPosition& delta) {
	m_Position += delta * m_Rotation;
}

void Camera::Rotate(const TRotation& delta) {
	m_Rotation = delta * m_Rotation;
}

void Camera::Rotate(Float32 pitch, Float32 yaw, Float32 roll) {
	Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	//m_Rotation.Rotate(yaw, pitch, roll);
}

void Camera::RotateAround(const TRotation& delta, const TPosition& pt)
{
	Translate(-pt);
	Rotate(delta);
	Translate(pt);
}

void Camera::RotateAround(Float32 pitch, Float32 yaw, Float32 roll, const TPosition& pt) {
	Translate(-pt);
	Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	Translate(pt);
}

void Camera::Forward(TPosition& outVec) const {
	outVec = glm::forward<TPosition>() * Rotation();
}

void Camera::ToMat4x4(glm::mat4x4& outMat) const {
	/*outMat = glm::identity<glm::mat4x4>();
	outMat *= glm::translate(outMat, m_Position);
	outMat *= glm::mat4x4(m_Rotation);*/

	outMat = glm::mat4x4(m_Rotation);
	outMat = glm::translate(outMat, m_Position);
}