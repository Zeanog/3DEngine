#pragma once

#include "Shaders/ShaderProgram_GLSL.h"

#include <windows.h>
#include "DeferredRendering/DeferredRendering.h"
#include "Rendering/FrameBufferObject.h"

#include "Rendering/Camera.h"
#include "System/Input/InputSystem.h"
#include "System/DebugConsole.h"
#include "Images/ImageManager.h"
#include "Math/Bounds.h"

class IModel;
class Model;

class GLApplication {
	CLASS_TYPEDEFS(GLApplication);

protected:
	Functor<void, TYPELIST_1(Mouse::State)>		OnMouseChangedFunctor;
	void	OnMouseChanged(ParamType<Mouse::State>::Type mouseState) {
		if (mouseState.Btns[0].IsSet() && (mouseState.DeltaX != 0 || mouseState.DeltaY != 0)) {
			RotateCamera(mouseState.DeltaX, mouseState.DeltaY);
		}
	}

	Functor<void, TYPELIST_1(KeyboardState)>		OnKeyboardChangedFunctor;
	void	OnKeyboardChanged(ParamType<KeyboardState>::Type keyboardState);

public:
	GLApplication() {
	}

	// Methods
	bool	initialize(HWND hwnd, int w, int h);
	virtual ~GLApplication();
	void	setSize(int w, int h);
	void	update();
	void	render();
	void	release();

	void    StartRotateCamera(Int64 x, Int64 y);
	void    StopRotateCamera(Int64 x, Int64 y);
	void    RotateCamera(Int64 x, Int64 y);

	void	TranslateCamera(Float32 x, Float32 y, Float32 z);

	void	showDeferredRendering(){ m_state = 0; }
	void	showRenderTargets(){ m_state = 1; }

private:
	// Methods
	void	loadAssets();
	void	releaseAssets();

	CameraInterpolator				m_Camera;

	// Fields
	std::vector<IModel*> m_PrevModels;
	Neo::Bounds			m_Bounds;
	std::vector<Model*> m_Models;



	DeferredRendering*	m_deferredRendering;
	FrameBufferObject*	m_multipleRenderTarget;

	std::vector<ALight*>		m_Lights;
	ShaderProgram_GLSL 	m_RenderModelProgram;

	ShaderProgram_GLSL 	m_LightingProgram_Directional;
	ShaderProgram_GLSL 	m_LightingProgram_Spot;
	ShaderProgram_GLSL 	m_LightingProgram_Point;

	ShaderProgram_GLSL 	m_RenderModel_UnlitProgram;
	ShaderProgram_GLSL	m_ShadowMapGenerationProgram;

	int					m_windowWidth;
	int					m_windowHeight;

	HGLRC				m_hRC;	// Rendering's context
	HDC					m_hDC;	// Device's context
	HWND				m_hWnd; // Window's handle

	float				m_DeltaTime;
	unsigned int		m_CurrentTime;
	unsigned int		m_PrevTick;
	unsigned char		m_state; // 0 - Normal render, 1 - Show render targets
};