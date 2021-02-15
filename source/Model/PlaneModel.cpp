#include "PlaneModel.h"
#include <math.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

/** 
*	Construct the plane
*/
PlaneModel::PlaneModel(float side)
	: m_side(side)
{
	m_Bounds *= side;
}

/** 
*	Render
*/
void PlaneModel::render( const ShaderProgram_GLSL& program )
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(glm::identity<glm::mat4x4>());
	localTransform = glm::translate(localTransform, m_Position);
	localTransform = localTransform * glm::mat4x4(m_Rotation);
	localTransform = glm::scale(localTransform, glm::vec3(m_side));
	glMultMatrixf(glm::value_ptr(localTransform));

	program.StartUsing();

	if( m_Image ) {
		m_Image->Bind();
	}

	//program.LinkUniform(StaticString("tDiffuse"), 0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); 
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f,  0.0f, -1.0f);

	glTexCoord2f(0.0f, 0.0f); 
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f,  0.0f,  1.0f);

	glTexCoord2f(1.0f, 0.0f); 
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f( 1.0f,  0.0f,  1.0f);

	glTexCoord2f(1.0f, 1.0f); 
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f( 1.0f,  0.0f, -1.0f);
	glEnd();

	program.StopUsing();

	if (m_Image) {
		m_Image->Unbind();
	}

	glPopMatrix();
}

