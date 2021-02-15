#pragma once

#include "System/Typedefs.h"
#include "Math/Matrix.h"

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/euler_angles.hpp>

class ICamera {
public:
	typedef glm::quat	TRotation;
	
protected:
	ICamera() {}

public:
	virtual void						Update(Float32 deltaTime) = 0;
	virtual void						LinkTransform() const = 0;

	virtual void						LocalPosition(const glm::vec3& pos) = 0;
	virtual void						LocalPosition(Float32 x, Float32 y, Float32 z) = 0;
	virtual const glm::vec3&			LocalPosition() const = 0;
	virtual glm::vec3&					LocalPosition() = 0;

	virtual void						GlobalPosition(const glm::vec3& pos) = 0;
	virtual void						GlobalPosition(Float32 x, Float32 y, Float32 z) = 0;
	virtual const glm::vec3&			GlobalPosition() const = 0;
	virtual glm::vec3&					GlobalPosition() = 0;

	virtual void						Translate(const glm::vec3& delta) = 0;

	virtual void						Rotation(const TRotation& rot) = 0;
	virtual const TRotation&			Rotation() const = 0;
	virtual TRotation&					Rotation() = 0;

	virtual void						Rotate(const TRotation& delta) = 0;
	virtual void						Rotate(Float32 pitch, Float32 yaw, Float32 roll) = 0;

	virtual glm::vec3					Forward() const = 0;

	virtual glm::mat4x4					ToMat4x4() const = 0;
};

class ACameraDecorator : public ICamera {
	INHERITEDCLASS_TYPEDEFS(ACameraDecorator, ICamera);

protected:
	ICamera*	m_Decoratee;

public:
	virtual void						Update(Float32 deltaTime) {
		m_Decoratee->Update(deltaTime);
	}

	virtual void						LinkTransform() const {
		m_Decoratee->LinkTransform();
	}

	virtual void						LocalPosition(const glm::vec3& pos) {
		m_Decoratee->LocalPosition(pos);
	}

	virtual void						LocalPosition(Float32 x, Float32 y, Float32 z) {
		m_Decoratee->LocalPosition(x, y, z);
	}

	virtual const glm::vec3&			LocalPosition() const {
		return m_Decoratee->LocalPosition();
	}

	virtual glm::vec3&					LocalPosition() {
		return m_Decoratee->LocalPosition();
	}

	virtual void						GlobalPosition(const glm::vec3& pos) {
		m_Decoratee->GlobalPosition(pos);
	}

	virtual void						GlobalPosition(Float32 x, Float32 y, Float32 z) {
		m_Decoratee->GlobalPosition(x, y, z);
	}

	virtual const glm::vec3&			GlobalPosition() const {
		return m_Decoratee->GlobalPosition();
	}

	virtual glm::vec3&					GlobalPosition() {
		return m_Decoratee->GlobalPosition();
	}

	virtual void						Translate(const glm::vec3& delta) {
		m_Decoratee->Translate(delta);
	}

	virtual void						Rotation(const TRotation& rot) {
		m_Decoratee->Rotation(rot);
	}

	virtual const TRotation&			Rotation() const {
		return m_Decoratee->Rotation();
	}

	virtual TRotation&					Rotation() {
		return m_Decoratee->Rotation();
	}

	virtual void						Rotate(const TRotation& delta) {
		m_Decoratee->Rotate(delta);
	}

	virtual void						Rotate(Float32 pitch, Float32 yaw, Float32 roll) {
		m_Decoratee->Rotate(pitch, yaw, roll);
	}

	virtual glm::vec3					Forward() const override {
		return m_Decoratee->Forward();
	}

	virtual glm::mat4x4					ToMat4x4() const {
		return m_Decoratee->ToMat4x4();
	}

	virtual void						Dispose() {
		DeletePtr(m_Decoratee);
	}

protected:
	ACameraDecorator(ICamera* decoratee) {
		m_Decoratee = decoratee;
	}
};

class Camera : public ICamera {
	CLASS_TYPEDEFS(Camera);

protected:
	TRotation		m_Rotation;
	glm::vec3		m_Position;

public:
	Camera();

	Camera(const glm::mat3x3& rot, const glm::vec3& pos);

	virtual void		Update(Float32 deltaTime);
	virtual void		LinkTransform() const;

	virtual void		LocalPosition(const glm::vec3& pos);
	virtual void		LocalPosition(Float32 x, Float32 y, Float32 z) {
		m_Position[0] = x;
		m_Position[1] = y;
		m_Position[2] = z;
	}
	virtual const glm::vec3& LocalPosition() const {
		return m_Position;
	}
	virtual glm::vec3& LocalPosition() {
		return m_Position;
	}

	virtual void		GlobalPosition(const glm::vec3& pos);
	virtual void		GlobalPosition(Float32 x, Float32 y, Float32 z) {
		m_Position[0] = x;
		m_Position[1] = y;
		m_Position[2] = z;
	}
	virtual const glm::vec3& GlobalPosition() const {
		return m_Position;
	}
	virtual glm::vec3& GlobalPosition() {
		return m_Position;
	}
	virtual void		Translate(const glm::vec3& delta);

	virtual void		Rotation(const TRotation& rot);
	virtual const TRotation& Rotation() const {
		return m_Rotation;
	}
	virtual TRotation& Rotation() {
		return m_Rotation;
	}
	virtual void		Rotate(const TRotation& delta);
	virtual void		Rotate(Float32 pitch, Float32 yaw, Float32 roll);

	virtual glm::vec3					Forward() const override {
		const float x2 = 2.0f * m_Rotation.x;
		const float y2 = 2.0f * m_Rotation.y;
		const float z2 = 2.0f * m_Rotation.z;
		const float x2w = x2 * m_Rotation.w;
		const float y2w = y2 * m_Rotation.w;
		const float x2x = x2 * m_Rotation.x;
		const float z2x = z2 * m_Rotation.x;
		const float y2y = y2 * m_Rotation.y;
		const float z2y = z2 * m_Rotation.y;
		return glm::normalize(glm::vec3(z2x + y2w, z2y - x2w, 1.0f - (x2x + y2y)));
	}

	virtual glm::mat4x4		ToMat4x4() const {
		glm::mat4x4 localTransform = (glm::mat4x4)m_Rotation;
		
		localTransform = glm::translate(localTransform, m_Position);

		return localTransform;
	}
};

class CameraInterpolator : public ACameraDecorator {
	INHERITEDCLASS_TYPEDEFS(CameraInterpolator, ACameraDecorator);

protected:
	TRotation		m_TargetRotation;
	glm::vec3		m_TargetPosition;

public:
	CameraInterpolator() : TSuper(new Camera()), m_TargetRotation(glm::identity<TRotation>()) {

	}

	CameraInterpolator(ICamera* decoratee) : TSuper(decoratee) {
	}

	virtual ~CameraInterpolator() {
		DeletePtr(m_Decoratee);
	}

	//static glm::quat RotateTowards(glm::quat q1, glm::quat q2, float maxAngle) {
	//	if (maxAngle < 0.001f) {
	//		// No rotation allowed. Prevent dividing by 0 later.
	//		return q1;
	//	}

	//	float cosTheta = dot(q1, q2);

	//	// q1 and q2 are already equal.
	//	// Force q2 just to be sure
	//	if (cosTheta > 0.9999f) {
	//		return q2;
	//	}

	//	// Avoid taking the long path around the sphere
	//	if (cosTheta < 0) {
	//		q1 = q1 * -1.0f;
	//		cosTheta *= -1.0f;
	//	}

	//	float angle = acos(cosTheta);

	//	// If there is only a 2&deg; difference, and we are allowed 5&deg;,
	//	// then we arrived.
	//	if (angle < maxAngle) {
	//		return q2;
	//	}

	//	float fT = maxAngle / angle;
	//	angle = maxAngle;

	//	glm::quat res = (sin((1.0f - fT) * angle) * q1 + sin(fT * angle) * q2) / sin(angle);
	//	res = normalize(res);
	//	return res;
	//}

	virtual void						Update(Float32 deltaTime) {
		m_Decoratee->GlobalPosition(m_Decoratee->GlobalPosition() + (m_TargetPosition - m_Decoratee->GlobalPosition()) * deltaTime * 10.0f);

		TRotation deltaRot = (m_TargetRotation * glm::inverse(m_Decoratee->Rotation()));
		deltaRot = glm::mix(glm::identity<TRotation>(), deltaRot, deltaTime * 12.0f);

		m_Decoratee->Rotate(deltaRot);
	}

	virtual const glm::vec3& LocalPosition() const {
		return m_Decoratee->LocalPosition();
	}
	virtual glm::vec3& Position() {
		return m_Decoratee->LocalPosition();
	}

	virtual const glm::vec3& GlobalPosition() const {
		return m_Decoratee->GlobalPosition();
	}
	virtual glm::vec3& GlobalPosition() {
		return m_Decoratee->GlobalPosition();
	}
	virtual const TRotation& Rotation() const {
		return m_Decoratee->Rotation();
	}
	virtual TRotation& Rotation() {
		return m_Decoratee->Rotation();
	}

	virtual void						LocalPosition(const glm::vec3& pos) {
		TSuper::LocalPosition(pos);
		m_TargetPosition = pos;
	}

	virtual void						LocalPosition(Float32 x, Float32 y, Float32 z) {
		TSuper::LocalPosition(x, y, z);
		m_TargetPosition[0] = x;
		m_TargetPosition[1] = y;
		m_TargetPosition[2] = z;
	}

	virtual void						GlobalPosition(const glm::vec3& pos) {
		TSuper::GlobalPosition(pos);
		m_TargetPosition = pos;
	}

	virtual void						GlobalPosition(Float32 x, Float32 y, Float32 z) {
		TSuper::GlobalPosition(x, y, z);
		m_TargetPosition[0] = x;
		m_TargetPosition[1] = y;
		m_TargetPosition[2] = z;
	}

	virtual void						Translate(const glm::vec3& localDelta) {
		m_TargetPosition += localDelta * m_Decoratee->Rotation();
	}

	virtual void						Rotation(const TRotation& rot) {
		m_TargetRotation = rot;
		TSuper::Rotation(rot);
	}

	virtual void						Rotate(const TRotation& delta) {
		m_TargetRotation = delta * m_TargetRotation;
	}

	virtual void						Rotate(Float32 pitch, Float32 yaw, Float32 roll) {
		Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	}
};