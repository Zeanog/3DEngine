#pragma once

#include "Mesh.h"
#include "Images/Image.h"
#include "System/IJsonSerializable.h"

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
	const AnimationClip* m_Clip;

	Float32			m_Time;

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
		if (m_Time < 0.0f) {
			return;
		}
		m_Time += deltaTime;
		if (m_Time >= m_Clip->Duration()) {
			m_Time = 0.0f;
		}
	}

	const AnimKeyFrame*	GetCurrentFrame() const {
		Int32 currentFrame = (Int32)(m_Time * m_Clip->FrameRate());
		return m_Clip->GetFrame(currentFrame);
	}
};

class Model {
public:
	typedef glm::quat	TRotation;

protected:
	TRotation		m_Rotation;
	glm::vec3		m_Position;

	AnimationPlayer	m_DebugAnimPlayer;

	const Neo::Mesh* m_Mesh;

public:
	DECLARE_GETSET(Position)
	DECLARE_GETSET(Rotation)

	Model() : m_Rotation(glm::identity<TRotation>())
	{
	}

	Model(const StaticString& meshPath);

	virtual ~Model();

	virtual void	Render(const ShaderProgram_GLSL& program) const;
	virtual void	RenderJoints( Float32 deltaTime );

	Bool			UploadData(const AModelLoader& loader);

	DECLARE_GETSET(Mesh);
	/*void			AddImage(const Neo::Image* img) {
		m_Images.Add(img);
	}*/

	Neo::Bounds	GetBounds() const {
		Neo::Bounds b( m_Mesh->GetBounds() );
		return b.Shift(m_Position);
	}

protected:


};