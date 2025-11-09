#pragma once
#include "AModel.h"

/** 
*	A simple cube model that is easy to render
*/
class CubeModel : public AModel
{
public:
	// Methods
	CubeModel(float side);

	virtual void	render( ShaderProgram_GLSL& program ) override;

protected:
	// Fields
	float m_side;
};

