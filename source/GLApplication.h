#pragma once

#include "Shaders/ShaderProgram_GLSL.h"

#include <windows.h>
#include "DeferredRendering/DeferredRendering.h"
#include "Rendering/FrameBufferObject.h"

#include "Rendering/CameraInterpolator.h"
#include "System/Input/InputSystem.h"
#include "System/DebugConsole.h"
#include "Images/ImageManager.h"
#include "Math/Bounds.h"
#include "System\Functors\Functor.h"

class AModel;
class Model;

class GLApplication {
	CLASS_TYPEDEFS(GLApplication)

protected:
	void	OnMouseChanged(Param<Neo::Mouse::State>::Type mouseState) {
		if (mouseState.Btns[0].IsSet() /*&& (mouseState.DeltaX != 0 || mouseState.DeltaY != 0)*/) {
			RotateCamera(mouseState.DeltaX, mouseState.DeltaY);
		}
	}

	void	OnKeyboardChanged(Param<KeyboardState>::Type keyboardState);

public:
	GLApplication() {
	}

	// Methods
	bool	Initialize(HWND hwnd, int w, int h);
	virtual ~GLApplication();
	void	SetSize(int w, int h);
	void	Update();
	void	Render();
	void	Release();

	void    RotateCamera(Int64 x, Int64 y);
	void	TranslateCamera(Float32 x, Float32 y, Float32 z);

	void	showDeferredRendering(){ m_state = 0; }
	void	showRenderTargets(){ m_state = 1; }

protected:
	// Methods
	void	LoadAssets();
	void	ReleaseAssets();

	void	RenderModels(ShaderProgram_GLSL& program);
	void	RenderModelShadows() {
		RenderModels(m_ShadowMapGenerationProgram);
	}

	CameraInterpolator		m_Camera;

	// Fields
	List<AModel*>		m_PrevTypeModels{};
	Neo::Bounds			m_Bounds{};
	List<Model*>		m_Models{};

	Functor<void>		OnRenderShadows;

	DeferredRendering* m_deferredRendering{};
	FrameBufferObject* m_multipleRenderTarget{};

	List<ALight*>		m_Lights{};
	ShaderProgram_GLSL 	m_RenderModelProgram;

	ShaderProgram_GLSL 	m_LightingProgram_Directional;
	ShaderProgram_GLSL 	m_LightingProgram_Spot;
	ShaderProgram_GLSL 	m_LightingProgram_Point;

	ShaderProgram_GLSL 	m_RenderModel_UnlitProgram;
	ShaderProgram_GLSL	m_ShadowMapGenerationProgram;

	int					m_windowWidth{};
	int					m_windowHeight{};

	HGLRC				m_hRC{};	// Rendering's context
	HDC					m_hDC{};	// Device's context
	HWND				m_hWnd{}; // Window's handle

	float				m_DeltaTime{};
	UInt64				m_CurrentTime{};
	decltype(m_CurrentTime)	m_PrevTick{};
	unsigned char		m_state{}; // 0 - Normal render, 1 - Show render targets
};