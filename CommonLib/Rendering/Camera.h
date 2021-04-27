#pragma once

#include "System/Typedefs.h"

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/euler_angles.hpp>

class ICamera {
public:
	typedef glm::quat	TRotation;
	typedef glm::vec3	TPosition;
	
protected:
	ICamera() {}

public:
	virtual void						Update(Float32 deltaTime) = 0;
	virtual void						LinkTransform() const = 0;

	ABSTRACT_GETSET(TPosition, Position)
	ABSTRACT_GETSET(TRotation, Rotation)

	virtual void						Translate(const TPosition& delta) = 0;

	virtual void						Rotate(const TRotation& delta) = 0;
	virtual void						Rotate(Float32 pitch, Float32 yaw, Float32 roll) = 0;
	virtual void						RotateAround(const TRotation& delta, const TPosition& pt) = 0;
	virtual void						RotateAround(Float32 pitch, Float32 yaw, Float32 roll, const TPosition& pt) = 0;

	virtual TPosition					Forward() const = 0;

	virtual glm::mat4x4					ToMat4x4() const = 0;
};

class ACameraDecorator : public ICamera {
	INHERITEDCLASS_TYPEDEFS(ACameraDecorator, ICamera);

protected:
	ICamera*	m_Decoratee;

public:
	virtual void						Update(Float32 deltaTime) override {
		assert(m_Decoratee);
		m_Decoratee->Update(deltaTime);
	}

	virtual void						LinkTransform() const override {
		assert(m_Decoratee);
		m_Decoratee->LinkTransform();
	}

	virtual void						Position(typename Param<TPosition>::Type pos) override {
		assert(m_Decoratee);
		m_Decoratee->Position(pos);
	}

	virtual typename Param<TPosition>::Type			Position() const override {
		assert(m_Decoratee);
		return m_Decoratee->Position();
	}

	virtual void						Rotation(typename Param<TRotation>::Type rot) override {
		assert(m_Decoratee);
		m_Decoratee->Rotation(rot);
	}

	virtual typename Param<TRotation>::Type Rotation() const override {
		assert(m_Decoratee);
		return m_Decoratee->Rotation();
	}

	virtual void						Translate(const TPosition& delta) override {
		assert(m_Decoratee);
		m_Decoratee->Translate(delta);
	}

	virtual void						Rotate(const TRotation& delta) override {
		assert(m_Decoratee);
		m_Decoratee->Rotate(delta);
	}

	virtual void						Rotate(Float32 pitch, Float32 yaw, Float32 roll) override {
		assert(m_Decoratee);
		m_Decoratee->Rotate(pitch, yaw, roll);
	}

	virtual void						RotateAround(const TRotation& delta, const TPosition& pt) override {
		assert(m_Decoratee);
		m_Decoratee->RotateAround(delta, pt);
	}

	virtual void						RotateAround(Float32 pitch, Float32 yaw, Float32 roll, const TPosition& pt) override {
	}

	virtual TPosition					Forward() const override {
		assert(m_Decoratee);
		return m_Decoratee->Forward();
	}

	virtual glm::mat4x4					ToMat4x4() const override {
		assert(m_Decoratee);
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
	INHERITEDCLASS_TYPEDEFS(Camera, ICamera);

protected:
	TRotation		m_Rotation;
	TPosition		m_Position;

public:
	Camera();

	Camera(const glm::mat3x3& rot, const TPosition& pos);

	virtual void		Update(Float32 deltaTime) override;
	virtual void		LinkTransform() const;

	DECLARE_GETSET(Position)
	DECLARE_GETSET(Rotation)

	virtual void		Translate(const TPosition& delta) override;

	virtual void		Rotate(const TRotation& delta) override;
	virtual void		Rotate(Float32 pitch, Float32 yaw, Float32 roll) override;
	virtual void		RotateAround(const TRotation& delta, const TPosition& pt) override;
	virtual void		RotateAround(Float32 pitch, Float32 yaw, Float32 roll, const TPosition& pt) override;

	virtual glm::vec3					Forward() const override;

	virtual glm::mat4x4		ToMat4x4() const override;
};

class CameraInterpolator : public ACameraDecorator {
	INHERITEDCLASS_TYPEDEFS(CameraInterpolator, ACameraDecorator);

protected:
	TRotation		m_TargetRotation;
	TPosition		m_TargetPosition;

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

	virtual void						Update(Float32 deltaTime) override {
		m_Decoratee->Position(m_Decoratee->Position() + (m_TargetPosition - m_Decoratee->Position()) * deltaTime * 10.0f);

		TRotation deltaRot = (m_TargetRotation * glm::inverse(m_Decoratee->Rotation()));
		deltaRot = glm::mix(glm::identity<TRotation>(), deltaRot, deltaTime * 12.0f);

		m_Decoratee->Rotate(deltaRot);
	}

	virtual void						Position(typename Param<TPosition>::Type pos) override {
		TSuper::Position(pos);
		m_TargetPosition = pos;
	}

	virtual void						Translate(const TPosition& localDelta) override {
		m_TargetPosition += localDelta * m_Decoratee->Rotation();
	}

	virtual void						Rotation(typename Param<TRotation>::Type rot) override {
		m_TargetRotation = rot;
		TSuper::Rotation(rot);
	}

	virtual void						Rotate(const TRotation& delta) override {
		m_TargetRotation = delta * m_TargetRotation;
	}

	virtual void						Rotate(Float32 pitch, Float32 yaw, Float32 roll) override {
		Rotate(glm::eulerAngleXYZ(pitch, yaw, roll));
	}

	virtual void						RotateAround(const TRotation& delta, const TPosition& pt) override {
		m_Decoratee->RotateAround(delta, pt);

		TPosition localPos = m_TargetPosition - pt;
		m_TargetRotation = delta * m_TargetRotation;
		m_TargetPosition = localPos * m_TargetRotation;
	}

	virtual void						RotateAround(Float32 pitch, Float32 yaw, Float32 roll, const TPosition& pt) override {
		RotateAround(glm::eulerAngleXYZ(pitch, yaw, roll), pt);
	}
};