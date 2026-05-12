#pragma once

#include "Rendering/Camera.h"

class FlyingCamera : public ACameraDecorator {
	INHERITED_CLASS_TYPEDEFS(FlyingCamera, ACameraDecorator)

protected:
	DEFINE_MEMBER_EX(glm::vec3, TranslationSpeed)//Reasonable Value Magnitude: 10
	DEFINE_MEMBER_EX(glm::vec3, RotationSpeed)//Reasonable Value Magnitude: 12
	
public:
	FlyingCamera() : TSelf(new Camera()) {
	}

	FlyingCamera(ICamera* decoratee) : TSuper(decoratee) {
	}
	
	virtual void						Update(Float32 deltaTime) override {
		m_Decoratee->Translate(deltaTime * m_TranslationSpeed);
		m_Decoratee->Rotate(deltaTime * m_RotationSpeed);
		
		TSuper::Update(deltaTime);
	}
};