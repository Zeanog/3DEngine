#pragma once
#include "AModel.h"

/** 
*	A simple sphere model that is easy to render
*/
class SphereModel : public AModel
{
public:
	// Methods
	SphereModel(float radius, unsigned int meshPrecision);

	virtual void	render( ShaderProgram_GLSL& program ) override;

private:
	// Fields
	float			m_radius;
	unsigned int	m_meshPrecision;
};

