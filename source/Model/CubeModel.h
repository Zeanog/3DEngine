#pragma once
#include "IModel.h"

/** 
*	A simple cube model that is easy to render
*/
class CubeModel : public IModel
{
public:
	// Methods
	CubeModel(float side);

	virtual void	render( const ShaderProgram_GLSL& program ) override;

protected:
	// Fields
	float m_side;
};

