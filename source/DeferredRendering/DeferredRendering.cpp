#include "Rendering/FrameBufferObject.h"
#include "DeferredRendering.h"
#include "Lighting/LightPool.h"
#include "Rendering/Camera.h"
#include <exception>

/**
*	Create the deferred rendering object. I have hardcoded the shader's name here.
*/
DeferredRendering::DeferredRendering(int _dWidth, int _dHeight, FrameBufferObject* fboRenderTexture)
	: m_pSrcFBO(fboRenderTexture)
	, m_width(_dWidth)
	, m_height(_dHeight)
{	
}

void DeferredRendering::PreRender() {
	//Projection setup
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, m_width, 0, m_height, 0.1f, 2);

	//Model setup
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, -1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // clear light accumulation buffer to black here

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE); // additive blending
}

void DeferredRendering::PostRender() {
	glDisable(GL_BLEND);

	//Reset to the matrices	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

/**
*	Render the big quad with the deferred rendering shader on it
*/
void DeferredRendering::Render( const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera, const ALight* light ) 
{
	const RenderTarget* diffuse = m_pSrcFBO->LinkTargetTo(StaticString("tDiffuse"), program, 0);
	const RenderTarget* positions = m_pSrcFBO->LinkTargetTo(StaticString("tPositions"), program, 1);
	const RenderTarget* normals = m_pSrcFBO->LinkTargetTo(StaticString("tNormals"), program, 2);

	const RenderTarget* shadow = light->LinkTo(program, bounds, camera);

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );

		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( (Float32)m_width, 0.0f, 0.0f );

		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f( (Float32)m_width, (Float32)m_height, 0.0f );

		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f( 0.0f, (Float32)m_height, 0.0f );
	glEnd();

	if (shadow) {
		shadow->Unlink();
		shadow = NULL;
	}

	diffuse->Unlink();
	positions->Unlink();
	normals->Unlink();
}

void DeferredRendering::showTexture(const StaticString& texName, float fSizeX, float fSizeY, float x, float y) const {
	if ((*m_pSrcFBO)[texName] != NULL) {
		m_pSrcFBO->showTexture(texName, fSizeX, fSizeY, x, y);
		return;
	}
}