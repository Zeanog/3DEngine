#pragma once

#include "Rendering/Camera.h"

class CameraInterpolator : public ACameraDecorator {
	INHERITED_CLASS_TYPEDEFS(CameraInterpolator, ACameraDecorator)

protected:
	TRotation		m_TargetRotation{};
	TPosition		m_TargetPosition{};

	Float32			m_TranslationSpeed = 10.0f;
	Float32			m_RotationSpeed = 12.0f;

public:
	CameraInterpolator() : TSuper(new Camera()), m_TargetPosition(), m_TargetRotation(glm::identity<TRotation>()) {

	}

	CameraInterpolator(ICamera* decoratee) : TSuper(decoratee), m_TargetPosition(), m_TargetRotation(glm::identity<TRotation>()) {
	}

	virtual void						Update(Float32 deltaTime) override {
		if (m_TargetPosition != m_Decoratee->Position()) {
			m_Decoratee->Position(m_Decoratee->Position() + (m_TargetPosition - m_Decoratee->Position()) * deltaTime * m_TranslationSpeed);
		}

		if (m_TargetRotation != m_Decoratee->Rotation()) {
			TRotation deltaRot = (m_TargetRotation * glm::inverse(m_Decoratee->Rotation()));
			deltaRot = glm::mix(glm::identity<TRotation>(), deltaRot, deltaTime * m_RotationSpeed);
			m_Decoratee->Rotate(deltaRot);
		}
	}

	virtual void						Position(typename Param<TPosition>::Type pos) override {
		TSuper::Position(pos);
		m_TargetPosition = pos;
	}

	virtual void						Translate(const TPosition& delta) override {
		m_TargetPosition += delta;
	}

	virtual void						TranslateRelative(const TPosition& delta) override {
		m_TargetPosition += delta * m_Decoratee->Rotation();
		
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
		m_TargetPosition = pt + localPos * m_TargetRotation;
	}

	virtual void						RotateAround(Float32 pitch, Float32 yaw, Float32 roll, const TPosition& pt) override {
		RotateAround(glm::eulerAngleXYZ(pitch, yaw, roll), pt);
	}
};
