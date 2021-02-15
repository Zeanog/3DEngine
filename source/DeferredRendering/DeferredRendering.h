#pragma once
#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "../Model/GLSLShaderData.h"
#include "Rendering/FrameBufferObject.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "Math/Bounds.h"
#include <glm/vec3.hpp>

class ALight;
class ICamera;

/**
*	This object is used to render a big screen sized quad with the deferred rendering shader applied on it.
*/
class DeferredRendering
{
public:
	// Ctors/Dtors
	DeferredRendering(int width, int height, FrameBufferObject* srcFBO);

	
	void	PreRender();
	void	PostRender();
	void	Render( const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera, const ALight* light);

	void	showTexture(const StaticString& texName, float fSizeX, float fSizeY, float x, float y) const;

private:
	FrameBufferObject*	m_pSrcFBO; // A pointer to the FBO render texture that contains diffuse, normals and positions

	unsigned int		m_width; // width
	unsigned int		m_height; // height
};
