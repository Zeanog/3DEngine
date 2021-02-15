#pragma once

#include "Images/Image.h"
#include "Images/ImageLoader_TGA.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "Math/Bounds.h"

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <string>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/euler_angles.hpp>

/** 
*	Every renderable object inherits from this interface for simplicity
*/
class IModel
{
public:
	typedef glm::quat	TRotation;

public:
	// Methods
	IModel() : m_Rotation(glm::identity<TRotation>()), m_Bounds(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f))
	{
		
	}
	virtual ~IModel(){}

	virtual bool	loadImage(const StaticString& textureName);
	virtual void		Position(const glm::vec3& pos);
	virtual void		Position(Float32 x, Float32 y, Float32 z) {
		m_Position[0] = x;
		m_Position[1] = y;
		m_Position[2] = z;
	}
	virtual const glm::vec3& Position() const {
		return m_Position;
	}
	virtual glm::vec3& Position() {
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
	virtual void	render( const ShaderProgram_GLSL& program ) = 0;

	Neo::Bounds	GetBounds() const {
		return m_Bounds.Shift(m_Position);
	}

protected:
	TRotation		m_Rotation;
	glm::vec3		m_Position;

	Neo::Bounds		m_Bounds;

	const Neo::Image*	m_Image;
};

