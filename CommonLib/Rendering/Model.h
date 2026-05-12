#pragma once

#include "Mesh.h"
#include "Images/Image.h"
#include "Shaders/ShaderProgram_GLSL.h"

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/euler_angles.hpp>

class ShaderProgram_GLSL;
class AModelLoader;
class GeometricComponent;

class AnimationPlayer {
public:

protected:
	const AnimationClip* m_Clip = NULL;

	Float32			m_Time = -1.0f;

public:
	AnimationPlayer() {
		m_Time = -1.0f;
	}

	void		Start(const AnimationClip* clip) {
		m_Clip = clip;
		m_Time = 0.f;
	}

	void		Stop() {
		m_Time = -1.0f;
	}

	void		Update(Float32 deltaTime) {
		if (!m_Clip || m_Time < 0.0f) {
			return;
		}
		m_Time += deltaTime;
		if (m_Time >= m_Clip->Duration()) {
			m_Time = 0.0f;
		}
	}

	const AnimKeyFrame* GetCurrentFrame() const {
		if (!m_Clip || m_Time < 0.0f) {
			return nullptr;
		}
		Int32 currentFrame = (Int32)(m_Time * m_Clip->FrameRate());//TODO: Interpolate between frames instead of just snapping to the current frame
		return m_Clip->GetFrame(currentFrame);
	}

	const AnimKeyFrame* operator[](UInt32 frameIndex) const {
		return m_Clip->GetFrame(frameIndex);
	}
};

class Model {
public:
	typedef glm::quat	TRotation;
	typedef glm::vec3	TPosition;
	typedef glm::vec3	TScale;

protected:
	glm::mat4x4	m_Transform{ glm::identity<glm::mat4x4>() };
	TPosition*	m_Position = reinterpret_cast<glm::vec3*>(&m_Transform[3]);
	TScale		m_Scale{ glm::vec3(0.01f) };
	
	AnimationPlayer	m_DebugAnimPlayer;

	const Neo::Mesh* m_Mesh{};

	const ShaderProgram_GLSL* m_ShaderProgram{};

	const ShaderProgram_GLSL* m_ShadowProgram{};

public:
	void	Position(const TPosition& pos) {
		*m_Position = pos;
	}
	TPosition&	Position() {
		return *m_Position;
	}
	const TPosition& Position() const {
		return *m_Position;
	}
	void Rotation(const TRotation& rot) {
		glm::mat4 rotMat = glm::mat4_cast(rot);
		Rotation(rotMat);
	}
	void Rotation(const glm::mat4x4& rotMat) {
		for (UInt32 ix = 0; ix < 3; ++ix) {
			m_Transform[ix].x = rotMat[ix].x;
			m_Transform[ix].y = rotMat[ix].y;
			m_Transform[ix].z = rotMat[ix].z;
		}
	}
	void Rotation(const glm::mat3x3& rotMat) {
		for(UInt32 ix = 0; ix < 3; ++ix) {
			m_Transform[ix].x = rotMat[ix].x;
			m_Transform[ix].y = rotMat[ix].y;
			m_Transform[ix].z = rotMat[ix].z;
		}
	}
	void	Rotate(const TRotation& rot) {
		Rotate(glm::mat4_cast(rot));
	}
	void	Rotate(const glm::mat4x4& rot) {
		auto pos = Position();
		m_Transform = rot * m_Transform;
		Position(pos);
	}

	DECLARE_GETSET(Scale)

	Model()
	{
	}

	Model(const StaticString& meshPath);

	virtual ~Model();

	virtual void	Render(ShaderProgram_GLSL& program) const;
	virtual void	RenderJoints(ShaderProgram_GLSL& program, Float32 deltaTime);

	Bool			UploadData(class ModelLoader& loader);

	DECLARE_GETSET(Mesh)

	Neo::Bounds	GetBounds() const {
		assert(m_Mesh);
		Neo::Bounds b(m_Mesh->GetBounds());
		return b.Shift(*m_Position);
	}

protected:


};