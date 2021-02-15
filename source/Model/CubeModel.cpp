#include "CubeModel.h"
#include <math.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
*	Construct the cube
*/
CubeModel::CubeModel(float side)
	: m_side(side)
{
	m_Bounds *= side;
}

/**
*	Render
*/
void CubeModel::render(const ShaderProgram_GLSL& program)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(glm::identity<glm::mat4x4>());
	localTransform = glm::translate(localTransform, m_Position);
	localTransform = localTransform * glm::mat4x4(m_Rotation);
	glMultMatrixf(glm::value_ptr(localTransform));

	program.StartUsing();

	if (m_Image) {
		m_Image->Bind();
	}

	//erify( program.LinkUniform(StaticString("tDiffuse"), 0) );

	glBegin(GL_QUADS);
	// Front Face
	glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	// Top Face
	glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, -1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, -1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, -1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, -1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glTexCoord2f(1.0f, 0.0f); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	glTexCoord2f(0.0f, 0.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	if (m_Image) {
		m_Image->Unbind();
	}

	program.StopUsing();

	glPopMatrix();
}

