#include "FrameBufferObject.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include <exception>

void RenderTexture::LinkTo(GLenum textureUnitIndex) const {
	m_LinkedTextureUnit = textureUnitIndex;

	glActiveTextureARB(GL_TEXTURE0_ARB + m_LinkedTextureUnit);
	glEnable(GL_TEXTURE_2D);

	Bind();
}

void RenderTexture::Unlink() const {
	Unbind();

	glActiveTextureARB(GL_TEXTURE0_ARB + m_LinkedTextureUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	m_LinkedTextureUnit = -1;
}

/**
*	Create the FBO render texture initializing all the stuff that we need
*/
FrameBufferObject::FrameBufferObject(int _dWidth, int _dHeight)
{	
	// Save extensions
	m_Width  = _dWidth;
	m_Height = _dHeight;

	// Generate the OGL resources for what we need
	glGenFramebuffersEXT(1, &m_hFBO);
}

/**
*	Destructor
*/
FrameBufferObject::~FrameBufferObject(){
	//glDeleteRenderbuffersEXT(1, &m_depthBuffer);

	glDeleteFramebuffersEXT(1, &m_hFBO);

	m_KeyToAttachmentMap.Clear();
}

/**
*	Start rendering to the texture
*	Both color and depth buffers are cleared.
*/
void FrameBufferObject::SetAsTarget() {
	// Bind our FBO and set the viewport to the proper size
	Bind();
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,m_Width, m_Height);

	// Clear the render targets
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	glDrawBuffers(m_TextureAttachments.Length(), &m_TextureAttachments[0]);
}

/**
*	Stop rendering to this texture.
*/
void FrameBufferObject::UnsetAsTarget(){
	// Stop acquiring and unbind the FBO
	Unbind();
	glPopAttrib();
}

const RenderTarget* FrameBufferObject::LinkTargetTo(const StaticString& key, const ShaderProgram_GLSL& program, GLenum textureUnitIndex) const {
	const RenderTarget* rt = (*this)[key];
	rt->LinkTo(textureUnitIndex);
	verify(program.LinkUniform(key, (int)textureUnitIndex));
	return rt;
}

/**
*	Show the texture to screen. It is just for debug purposes.
*/
void FrameBufferObject::showTexture(const StaticString& texName, float fSizeX, float fSizeY, float x, float y) const {
	const RenderTarget* rt = (*this)[texName];
	assert(rt);

	//Projection setup
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,m_Width,0,m_Height,0.1f,2);	

	//Model setup
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	rt->Bind();

	// Render the quad
	glTranslatef(x,-y,-1.0);

	glColor3f(1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f( 0, 1 );
	glVertex3f(    0.0f,  (float) m_Height, 0.0f);
	glTexCoord2f( 0, 0 );
	glVertex3f(    0.0f,   m_Height-fSizeY, 0.0f);
	glTexCoord2f( 1, 0 );
	glVertex3f(   fSizeX,  m_Height-fSizeY, 0.0f);
	glTexCoord2f( 1, 1 );
	glVertex3f(   fSizeX, (float) m_Height, 0.0f);
	glEnd();

	rt->Unbind();

	//Reset to the matrices	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void FrameBufferObject_Depth::SetAsTarget() {
	Bind();
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_Width, m_Height);

	// Clear the render target
	glClear(GL_DEPTH_BUFFER_BIT);

	// Specify that we need no colours
	glDrawBuffer(GL_NONE);
}