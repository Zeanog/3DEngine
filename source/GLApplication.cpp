#include <windows.h>
#include "GLApplication.h"
#include "Model/SphereModel.h"
#include "Model/CubeModel.h"
#include "Model/PlaneModel.h"

#include "System/Typedefs.h"
#include "System/Input/VirtualKey.h"
#include "Images/ImageManager.h"
#include "Lighting/Light.h"
#include "Rendering/Model.h"

#include "System/Input/InputSystem.h"

#include "System/Configuration.h"

#include "System/Win32/Window.h"
#include <glm/vec3.hpp>

#include "Math/MathUtils.h"
#include "Lighting/LightPool.h"
#include <glm/gtc/type_ptr.hpp>

Int64		xMousePrev = 0;
Int64		yMousePrev = 0;

CComModule	_module;

/**
*	Initialize our GL application
*/
bool GLApplication::initialize(HWND hwnd, int width, int height)
{
	GLuint pixelFormat;
	m_windowHeight = height;
	m_windowWidth = width;

	m_hWnd = hwnd;

	static	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	if (!(m_hDC = GetDC(m_hWnd)))
		return FALSE;

	if (!(pixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
		return FALSE;

	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))
		return FALSE;

	if (!(m_hRC = wglCreateContext(m_hDC)))
		return FALSE;

	if (!wglMakeCurrent(m_hDC, m_hRC))
		return FALSE;

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	setSize(width, height);

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLenum err = glewInit();
	if (GLEW_OK != err)
		return false;

	Singleton<InputSystem>::GetInstance()->Init(m_hWnd);

	OnMouseChangedFunctor.AddListener(this, &TSelf::OnMouseChanged);
	Singleton<InputSystem>::GetInstance()->GetMouse()->OnChanged.AddListener( OnMouseChangedFunctor );

	OnKeyboardChangedFunctor.AddListener(this, &TSelf::OnKeyboardChanged);
	Singleton<InputSystem>::GetInstance()->GetKeyboard()->OnKeydown.AddListener( OnKeyboardChangedFunctor );

	loadAssets();

	m_lastTick = GetTickCount();

	CoInitialize(NULL);

	return true;
}

GLApplication::~GLApplication() {
	CoUninitialize();
}


void	GLApplication::OnKeyboardChanged(ParamType<KeyboardState>::Type keyboardState) {
	if (keyboardState.KeyIsDown(DIK_W)) {
		TranslateCamera(0.0f, 0.0f, 1.0f);
	}
	else if (keyboardState.KeyIsDown(DIK_S)) {
		TranslateCamera(0.0f, 0.0f, -1.0f);
	}

	if (keyboardState.KeyIsDown(DIK_A)) {
		TranslateCamera(1.0f, 0.0f, 0.0f);
	}
	else if (keyboardState.KeyIsDown(DIK_D)) {
		TranslateCamera(-1.0f, 0.0f, 0.0f);
	}

	if (keyboardState.KeyIsDown(DIK_SPACE)) {
		TranslateCamera(0.0f, -1.0f, 0.0f);
	}
	else if (keyboardState.KeyIsDown(DIK_C)) {
		TranslateCamera(0.0f, 1.0f, 0.0f);
	}

	if (keyboardState.KeyIsDown(DIK_F1)) {
		showDeferredRendering();
	}
	else if (keyboardState.KeyIsDown(DIK_F2)) {
		showRenderTargets();
	}

	if (keyboardState.KeyIsDown(DIK_R)) {
		Singleton<ImageManager>::GetInstance()->ReloadAll();
	}

	if (keyboardState.KeyIsDown(DIK_F3)) {
		if (!Singleton<DebugConsole>::GetInstance()->IsOpen()) {
			verify(Singleton<DebugConsole>::GetInstance()->Open());
		}
		else {
			Singleton<DebugConsole>::GetInstance()->Close();
		}
	}

	if (keyboardState.KeyIsDown(DIK_F4)) {
		//Create SWF Window here
		//flashEngine.AttachTo(m_hWnd);
		//flashEngine.PlayMovie("Data/Car-speakers-590x90.swf");
	}
}

/**
*	Set window's size
*/
void GLApplication::setSize(int w, int h)
{
	m_windowWidth = w;
	m_windowHeight = h;

	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)m_windowWidth / (GLfloat)m_windowHeight, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool		lButtonState = false;
void GLApplication::StartRotateCamera(Int64 x, Int64 y) {
	xMousePrev = x;
	yMousePrev = y;

	lButtonState = true;
}

void GLApplication::StopRotateCamera(Int64 x, Int64 y) {
	lButtonState = false;
}

void GLApplication::RotateCamera(Int64 deltaX, Int64 deltaY) {
	m_Camera.Rotate(deltaY * m_DeltaTime * 1.0f, deltaX * m_DeltaTime * 1.0f, 0.0f);
}

void GLApplication::TranslateCamera(Float32 x, Float32 y, Float32 z) {
	m_Camera.Translate(glm::vec3(x, y, z) * m_DeltaTime * 5.0f);
}

/**
*	Update
*/

void GLApplication::update()
{
	int delta = GetTickCount() - m_lastTick;
	float time = delta * 0.001f;

	m_lastTick = GetTickCount();

	m_DeltaTime = MathUtils::Min(time, 1.0f / 30.0f);

	Singleton<InputSystem>::GetInstance()->Poll();

	m_Camera.Update(m_DeltaTime);

	if (m_PrevModels.size() >= 1) {
		m_PrevModels[0]->Rotate(m_DeltaTime, m_DeltaTime, 0);
	}

	/*if (m_PrevModels.size() >= 2) {
		m_PrevModels[1]->addRotation(0, time, time);
	}*/

	//DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin();
	//glm::mat4 rot = glm::eulerAngleXYZ(MathUtils::Deg2Radians(-1.0f * m_DeltaTime), 0.0f, 0.0f) * (*iter)->ToMat4x4();
	//(*iter)->Direction(glm::normalize((*iter)->Direction() + glm::vec3(0.1f, 0.0f, 0.0f) * m_DeltaTime) );


	if (m_Models.size() >= 1) {
		m_Models[0]->Rotate(0, m_DeltaTime, 0);
	}

	if (m_Models.size() >= 2) {
		m_Models[1]->Rotate(0, m_DeltaTime, 0);
	}

	for (UInt32 i = 0; i < m_PrevModels.size(); ++i) {
		m_Bounds += m_PrevModels[i]->GetBounds();
	}
	
	for (UInt32 i = 0; i < m_Models.size(); ++i) {
		m_Bounds += m_Models[i]->GetBounds();
	}
}

#define CAST_SHADOWS 1

void GLApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#if CAST_SHADOWS
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glDisable(GL_BLEND);

	Light_Directional::PreShadowRender();

	for (DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin(), end = Singleton<DirectionalLightPool>::GetInstance()->End(); iter != end; ++iter) {
		if (!(*iter)->CastsShadows()) {
			continue;
		}
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glm::mat4x4 lightTransform = (*iter)->ToMat4x4_Local(m_Bounds.GetCenter(), 20.0f);
		glMultMatrixf(glm::value_ptr(lightTransform));

		(*iter)->SetShadowMapAsTarget();
		for (UInt32 i = 0; i < m_PrevModels.size(); ++i) {
			m_PrevModels[i]->render(m_ShadowMapGenerationProgram);
		}

		for (UInt32 i = 0; i < m_Models.size(); ++i) {
			m_Models[i]->Render(m_ShadowMapGenerationProgram);
		}
		(*iter)->UnsetShadowMapAsTarget();

		glPopMatrix();
	}

	Light_Directional::PostShadowRender();

	glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)m_windowWidth / (GLfloat)m_windowHeight, 0.1f, 100.0f);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_Camera.LinkTransform();

	// Render our geometry into the FBO
	m_multipleRenderTarget->SetAsTarget();
	for (UInt32 i = 0; i < m_PrevModels.size(); ++i) {
		m_PrevModels[i]->render(m_RenderModelProgram);
	}

	for (DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin(), end = Singleton<DirectionalLightPool>::GetInstance()->End(); iter != end; ++iter) {
		(*iter)->DebugRender(m_RenderModel_UnlitProgram, (*iter)->ToMat4x4_World(m_Bounds.GetCenter(), 20.0f));
	}

	for (UInt32 i = 0; i < m_Models.size(); ++i) {
		m_Models[i]->Render(m_RenderModelProgram);
		//m_Models[i]->RenderJoints(m_DeltaTime);
	}

	m_multipleRenderTarget->UnsetAsTarget();

	// Render to the screen
	if (m_state == 0)
	{
		m_deferredRendering->PreRender();
		
		for (DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin(), end = Singleton<DirectionalLightPool>::GetInstance()->End(); iter != end; ++iter) {
			m_LightingProgram_Directional.StartUsing();
			m_deferredRendering->Render(m_LightingProgram_Directional, m_Bounds, m_Camera, *iter);
			m_LightingProgram_Directional.StopUsing();
		}

		for (SpotLightPool::Iterator iter = Singleton<SpotLightPool>::GetInstance()->Begin(), end = Singleton<SpotLightPool>::GetInstance()->End(); iter != end; ++iter) {
			m_LightingProgram_Spot.StartUsing();
			m_deferredRendering->Render(m_LightingProgram_Spot, m_Bounds, m_Camera, *iter);
			m_LightingProgram_Spot.StopUsing();
		}

		for (PointLightPool::Iterator iter = Singleton<PointLightPool>::GetInstance()->Begin(), end = Singleton<PointLightPool>::GetInstance()->End(); iter != end; ++iter) {
			m_LightingProgram_Point.StartUsing();
			m_deferredRendering->Render(m_LightingProgram_Point, m_Bounds, m_Camera, *iter);
			m_LightingProgram_Point.StopUsing();
		}
		m_deferredRendering->PostRender();
	}
	else if (m_state == 1)
	{
		m_deferredRendering->showTexture("tDiffuse", 512, 384, 0, 0);
		m_deferredRendering->showTexture("tPositions", 512, 384, 512, 0);
		m_deferredRendering->showTexture("tNormals", 512, 384, 0, 384);
	
		DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin();
		(*iter)->DebugRenderMap(512, 384, 512, 384);
	}

	SwapBuffers(m_hDC);
}

/**
*	Release all the GL resources we have allocated
*/
void GLApplication::release()
{
	releaseAssets();

	wglMakeCurrent(m_hDC, 0);
	wglDeleteContext(m_hRC);

	ReleaseDC(m_hWnd, m_hDC);
}

#include "Rendering/ModelLoaders/MeshManager.h"

/**
*	Load all the required assets
*/
void GLApplication::loadAssets()
{
	m_state = 0;

	m_multipleRenderTarget = new FrameBufferObject(m_windowWidth, m_windowHeight);
	m_multipleRenderTarget->Bind();
	m_multipleRenderTarget->AddTarget("tDiffuse", GL_COLOR_ATTACHMENT0_EXT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_multipleRenderTarget->AddTarget("tPositions", GL_COLOR_ATTACHMENT1_EXT, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT);
	m_multipleRenderTarget->AddTarget("tNormals", GL_COLOR_ATTACHMENT2_EXT, GL_RGBA16F_ARB, GL_RGBA, GL_FLOAT);
	m_multipleRenderTarget->AddTarget("Depth", GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT24);
	m_multipleRenderTarget->Unbind();

	m_deferredRendering = new DeferredRendering(m_windowWidth, m_windowHeight, m_multipleRenderTarget);

	verify(m_LightingProgram_Directional.Create("data/deferredLighting.vert", "data/deferredLighting_Directional.frag", NULL));
	verify(m_LightingProgram_Spot.Create("data/deferredLighting.vert", "data/deferredLighting_Spot.frag", NULL));
	verify(m_LightingProgram_Point.Create("data/deferredLighting.vert", "data/deferredLighting_Point.frag", NULL));
	
	verify(m_RenderModel_UnlitProgram.Create("data/deferredShading.vert", "data/deferredShading_Unlit.frag", NULL));
	verify(m_RenderModelProgram.Create("data/deferredShading.vert", "data/deferredShading.frag", NULL));

	IModel* m = NULL;

	m = new SphereModel(1, 64);
	m_PrevModels.push_back(m);
	m->loadImage(StaticString("data/125881.tga"));
	m->Position(2, 2.5f, 0);

	/*m = new CubeModel(1);
	m_PrevModels.push_back( m );
	m->loadImage( StaticString("data/box.raw") );
	m->setPosition(-2,2.5f,0);
	m->setRotation( 0.0f, 0.0f, 0.0f );*/

	m = new PlaneModel(5);
	m_PrevModels.push_back(m);
	m->loadImage(StaticString("data/DragonsDogma.tga"));
	m->Position(0, 0, 0);

	m = new PlaneModel(5);
	m_PrevModels.push_back(m);
	m->loadImage(StaticString("data/DragonsDogma.tga"));
	m->Position(7, 2.5, 0);
	m->Rotate(0.0f, 0.0f, 3.14f / 3.0f);

	ALight* light = NULL;
	glm::vec3 dir = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
	light = new Light_Directional(dir);
#if CAST_SHADOWS
	light->CastsShadows(true);
#endif
	m_Lights.push_back(light);

	//light = new Light_Point(lm::vec3(10.0f, 0.0f, 0.0f), 30.0f );
#if CAST_SHADOWS
	//light->CastsShadows(true);
#endif
	//m_Lights.push_back(light);

	//light = new Light_Spot(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), 30.0f);
#if CAST_SHADOWS
	//light->CastsShadows(true);
#endif
	//m_Lights.push_back(light);

	Model* model = NULL;
	//model = new Model("Data/Cube.mesh");
	//model->Position(-2, 2.5f, 0);
	//m_Models.push_back(model);

	Configuration config;

	config.LoadFrom("Data/Scene.ini");
	String modelPath;
	config.GetValue("Model", "Path", modelPath);

	model = new Model(modelPath.CStr());
	model->GlobalPosition(-2, 2.5f, 0);
	m_Models.push_back(model);

	m_Camera.GlobalPosition(0.0f, -4.0f, -10.0f);
	//m_Camera.Rotation(glm::eulerAngleXYZ(0.0f, MathUtils::Deg2Radians(90.0f), 0.0f));
}

#include "Shaders/ShaderProgramManager.h"
#include "Shaders/ShaderManager_Vertex.h"
#include "Shaders/ShaderManager_Fragment.h"

void GLApplication::releaseAssets()
{
	DeletePtr(m_multipleRenderTarget);
	DeletePtr(m_deferredRendering);

	Destroy(m_PrevModels);
	Destroy(m_Models);
	Destroy(m_Lights);

	Singleton<ImageManager>::GetInstance()->Shutdown();
	Singleton<MeshManager>::GetInstance()->Shutdown();

	Singleton<ShaderManager_Vertex>::GetInstance()->Shutdown();
	Singleton<ShaderManager_Fragment>::GetInstance()->Shutdown();
	Singleton<ShaderProgramManager>::GetInstance()->Shutdown();
}