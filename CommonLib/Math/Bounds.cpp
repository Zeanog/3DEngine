#include "Bounds.h"

#include "Math/MathUtils.h"
#include "Shaders/ShaderProgram_GLSL.h"

#include <windows.h>
#include <gl/gl.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

void Neo::Bounds::Render(const ShaderProgram_GLSL& program, const glm::vec3& offset) const {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4x4 localTransform(MathUtils::CreateAxisAlong_World(glm::forward<glm::vec3>(), offset, glm::up<glm::vec3>()));
	glMultMatrixf(glm::value_ptr(localTransform));

	glLineWidth(1);

	program.StartUsing();

	glBegin(GL_LINES);

	// Front Face
	/*glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f, 1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, -1.0f, 1.0f);
	/*glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f, 1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, -1.0f, 1.0f);
	/*glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f, 1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, 1.0f, 1.0f);
	/*glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f, 1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	/*glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, -1.0f, -1.0f);
	/*glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, 1.0f, -1.0f);
	/*glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, 1.0f, -1.0f);
	/*glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, -1.0f, -1.0f);
	// Top Face
	/*glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, 1.0f, -1.0f);
	/*glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, 1.0f, 1.0f);
	/*glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, 1.0f, 1.0f);
	/*glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	/*glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, -1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, -1.0f, -1.0f);
	/*glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, -1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, -1.0f, -1.0f);
	/*glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, -1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, -1.0f, 1.0f);
	/*glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, -1.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	/*glTexCoord2f(1.0f, 0.0f); glNormal3f(1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, -1.0f, -1.0f);
	/*glTexCoord2f(1.0f, 1.0f); glNormal3f(1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, 1.0f, -1.0f);
	/*glTexCoord2f(0.0f, 1.0f); glNormal3f(1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, 1.0f, 1.0f);
	/*glTexCoord2f(0.0f, 0.0f); glNormal3f(1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	/*glTexCoord2f(0.0f, 0.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, -1.0f, -1.0f);
	/*glTexCoord2f(1.0f, 0.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, -1.0f, 1.0f);
	/*glTexCoord2f(1.0f, 1.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, 1.0f, 1.0f);
	/*glTexCoord2f(0.0f, 1.0f); glNormal3f(-1.0f, 0.0f, 0.0f); glColor3f(1.0f, 1.0f, 1.0f);*/ glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	program.StopUsing();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}