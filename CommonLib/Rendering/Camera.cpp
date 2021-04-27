#include "Camera.h"
#include "Math\MathUtils.h"
#include <windows.h>
#include <gl/GL.h>

#include <glm/gtc/type_ptr.hpp>

Camera::Camera() : m_Rotation(glm::identity<TRotation>()) {
}

Camera::Camera(const glm::mat3x3& rot, const glm::vec3& pos) : Camera() {
	Position( pos );
	m_Rotation = rot;
}

void Camera::Update(Float32 deltaTime) {

}

void Camera::LinkTransform() const {
	glMultMatrixf(glm::value_ptr(ToMat4x4()));
}

void Camera::Translate(const glm::vec3& delta) {
	m_Position += delta;
}

void Camera::Rotate(const TRotation& delta) {
	m_Rotation = delta * m_Rotation;
}

void Camera::Rotate(Float32 pitch, Float32 yaw, Float32 roll) {
	Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	//m_Rotation.Rotate(yaw, pitch, roll);
}

void Camera::RotateAround(const TRotation& delta, const glm::vec3& pt) {
	Translate(-pt);
	Rotate(delta);
	Translate(pt);
}

void Camera::RotateAround(Float32 pitch, Float32 yaw, Float32 roll, const glm::vec3& pt) {
	Translate(-pt);
	Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	Translate(pt);
}

glm::vec3 Camera::Forward() const {
	return glm::forward<glm::vec3>() * Rotation();
}

glm::mat4x4 Camera::ToMat4x4() const {

	glm::mat4x4 t = glm::mat4x4(m_Rotation);
	t = glm::translate(t, m_Position);

	/*glm::vec3 pos = -(m_Position * Rotation());
	glm::vec3 focal = pos - Forward() * 10.0f;
	glm::vec3 up = glm::up<glm::vec3>();
	glm::mat4x4 t = MathUtils::CreateAxisAlong(pos, focal, up);*/

	return t;
}