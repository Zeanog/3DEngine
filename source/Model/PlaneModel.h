#pragma once
#include "AModel.h"

/** 
*	A plane model that is easy to render
*/
class PlaneModel : public AModel
{
public:
	// Methods
	PlaneModel(float side);

	virtual void	render( ShaderProgram_GLSL& program ) override;

protected:
	// Fields
	float m_side;
};

