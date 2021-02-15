#pragma once
#include "IModel.h"

/** 
*	A plane model that is easy to render
*/
class PlaneModel : public IModel
{
public:
	// Methods
	PlaneModel(float side);

	virtual void	render( const ShaderProgram_GLSL& program ) override;

protected:
	// Fields
	float m_side;
};

