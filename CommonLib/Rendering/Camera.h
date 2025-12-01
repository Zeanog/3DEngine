#pragma once

#include "System/Typedefs.h"
#include "System\Functors\ParamType.h"

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
	INHERITEDCLASS_TYPEDEFS(ACameraDecorator, ICamera)

protected:
	ICamera*	m_Decoratee;

public:
	virtual ~ACameraDecorator() {
		DeletePtr(m_Decoratee);
	}

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

protected:
	ACameraDecorator(ICamera* decoratee) {
		m_Decoratee = decoratee;
	}
};

class Camera : public ICamera {
	INHERITEDCLASS_TYPEDEFS(Camera, ICamera)

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

	virtual glm::vec3		Forward() const override;

	virtual glm::mat4x4		ToMat4x4() const override;
};