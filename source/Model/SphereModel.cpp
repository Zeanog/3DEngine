#include "SphereModel.h"
#include <math.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

/** 
*	Construct the sphere
*/
SphereModel::SphereModel(float radius, unsigned int meshPrecision)
{	
	m_radius = radius;
	m_meshPrecision = meshPrecision;
	m_Bounds *= radius;
}

/** 
*	Render
*/
void SphereModel::render( const ShaderProgram_GLSL& program )
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(glm::identity<glm::mat4x4>());
	localTransform = glm::translate(localTransform, m_Position);
	localTransform = localTransform * glm::mat4x4(m_Rotation);
	glMultMatrixf(glm::value_ptr(localTransform));

	program.StartUsing();

	if( m_Image ) {
		m_Image->Bind();
	}

 	GLUquadricObj *sphere = gluNewQuadric();
	gluQuadricTexture(sphere, true);
	gluSphere(sphere, m_radius, m_meshPrecision, m_meshPrecision);
	gluDeleteQuadric(sphere);

	if (m_Image) {
		m_Image->Unbind();
	}

	program.StopUsing();
	
	glPopMatrix();
}

