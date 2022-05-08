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

	m_PrevTick = GetTickCount();

	OnRenderShadows.AddListener(this, &TSelf::RenderModelShadows);

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

	gluPerspective(60.0f, (GLfloat)m_windowWidth / (GLfloat)m_windowHeight, 0.1f, 100.0f);

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

void GLApplication::update()
{
	m_PrevTick = m_CurrentTime;
	m_CurrentTime = GetTickCount();
	int delta = m_CurrentTime - m_PrevTick;
	float time = delta * 0.001f;

	m_DeltaTime = MathUtils::Min(time, 1.0f / 30.0f);

	Singleton<InputSystem>::GetInstance()->Poll();

	m_Camera.Update(m_DeltaTime);

	if (m_PrevModels.size() >= 1) {
		m_PrevModels[0]->Rotate(m_DeltaTime, m_DeltaTime, 0);
	}

	/*if (m_PrevModels.size() >= 2) {
		m_PrevModels[1]->addRotation(0, time, time);
	}*/

	//Just for debugging
	/*DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin();
	glm::vec3 currAngles;
	currAngles.x = -90.0f;
	currAngles.y = 45.0f * std::sin(MathUtils::MilliSec2Sec(m_CurrentTime));
	currAngles.z = 0.0f;
	glm::vec3 dir = glm::forward<glm::vec4>() * glm::quat(glm::vec3(MathUtils::Deg2Radians(currAngles.x), MathUtils::Deg2Radians(currAngles.y), MathUtils::Deg2Radians(currAngles.z)));
	(*iter)->Direction(dir);*/

	SpotLightPool::Iterator iter = Singleton<SpotLightPool>::GetInstance()->Begin();
	glm::vec3 currAngles;
	currAngles.x = MathUtils::Deg2Radians(90.0f);
	currAngles.y = MathUtils::Deg2Radians(30.0f * std::sin(MathUtils::MilliSec2Sec(m_CurrentTime)));
	currAngles.z = MathUtils::Deg2Radians(0.0f);
	glm::mat4 t = glm::eulerAngleXYZ(currAngles.x, currAngles.y, currAngles.z);
	t[3] = glm::vec4((*iter)->Position(), 1.0f);
	(*iter)->Transform(t);

	//(*iter)->Position(glm::vec3(0.0f, 10.0f, 0.0f) + 3.0f * glm::vec3(std::sin(MathUtils::MilliSec2Sec(m_CurrentTime)), 0.0f, 0.0f));

	if (m_Models.size() >= 1) {
		m_Models[0]->Rotation( glm::quat(glm::vec3(0, m_DeltaTime, 0)) * m_Models[0]->Rotation());
	}

	if (m_Models.size() >= 2) {
		m_Models[1]->Rotation(glm::quat(glm::vec3(0, m_DeltaTime, 0)) * m_Models[1]->Rotation());
	}

	for (UInt32 i = 0; i < m_PrevModels.size(); ++i) {
		m_Bounds += m_PrevModels[i]->GetBounds();
	}
	
	for (UInt32 i = 0; i < m_Models.size(); ++i) {
		m_Bounds += m_Models[i]->GetBounds();
	}
}

#define CAST_SHADOWS 1

void GLApplication::RenderModels(const ShaderProgram_GLSL& program) {
	for (UInt32 i = 0; i < m_PrevModels.size(); ++i) {
		m_PrevModels[i]->render(program);
	}

	for (UInt32 i = 0; i < m_Models.size(); ++i) {
		m_Models[i]->Render(program);
	}
}

void GLApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#if CAST_SHADOWS
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glDisable(GL_BLEND);

	Light_Directional::RenderShadows(m_Bounds, OnRenderShadows);
	Light_Spot::RenderShadows(OnRenderShadows, m_Camera);
	
	glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0f, (GLfloat)m_windowWidth / (GLfloat)m_windowHeight, 1.0f, 25.0f);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_Camera.LinkTransform();

	m_multipleRenderTarget->SetAsTarget();
	RenderModels(m_RenderModelProgram);

#if _DEBUG
	FOREACH(iter, *Singleton<DirectionalLightPool>::GetInstance()) {
		glm::mat4 mat = (*iter)->Transform();
		mat[3] = glm::vec4(m_Bounds.GetCenter() - (*iter)->Direction() * 10.0f, 1.0f);
		(*iter)->DebugRender(m_RenderModel_UnlitProgram, mat);
	}
	FOREACH(iter, *Singleton<SpotLightPool>::GetInstance()) {
		(*iter)->DebugRender(m_RenderModel_UnlitProgram, (*iter)->Transform());
	}
#endif

	m_multipleRenderTarget->UnsetAsTarget();

	// Render to the screen
	if (m_state == 0)
	{
		m_deferredRendering->PreRender();
		FOREACH(iter, *Singleton<DirectionalLightPool>::GetInstance()) {
			m_LightingProgram_Directional.StartUsing();
			m_deferredRendering->Render(m_LightingProgram_Directional, m_Bounds, m_Camera, *iter);
			m_LightingProgram_Directional.StopUsing();
		}

		FOREACH(iter, *Singleton<SpotLightPool>::GetInstance()) {
			m_LightingProgram_Spot.StartUsing();
			m_deferredRendering->Render(m_LightingProgram_Spot, m_Bounds, m_Camera, *iter);
			m_LightingProgram_Spot.StopUsing();
		}

		FOREACH(iter, *Singleton<PointLightPool>::GetInstance()) {
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
	
		//DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin();
		SpotLightPool::Iterator iter = Singleton<SpotLightPool>::GetInstance()->Begin();
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
	verify(m_ShadowMapGenerationProgram.Create("data/shadowMap.vert", "data/shadowMap.frag", NULL));

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
//
//	ALight* light = NULL;
//	glm::vec3 dir = glm::vec4(glm::forward<glm::vec3>(), 0.0f) * glm::quat(glm::vec3(MathUtils::Deg2Radians(-90), MathUtils::Deg2Radians(-45), MathUtils::Deg2Radians(0)));
//	light = new Light_Directional(dir);
//#if CAST_SHADOWS
//	light->CastsShadows(true);
//#endif
//	m_Lights.push_back(light);

	//light = new Light_Point(lm::vec3(10.0f, 0.0f, 0.0f), 30.0f );
#if CAST_SHADOWS
	//light->CastsShadows(true);
#endif
	//m_Lights.push_back(light);

	Light_Spot* spotLight = new Light_Spot(glm::vec3(2.0f, 10.0f, 0.0f), glm::eulerAngleXYZ(MathUtils::Deg2Radians(90.0f), MathUtils::Deg2Radians(45.0f), MathUtils::Deg2Radians(0.0f)), MathUtils::Deg2Radians(30.0f), (float)m_windowWidth / m_windowHeight);
	spotLight->ConstantAttenuation(2.0f);
	spotLight->LinearAttenuation(0.01f);
	spotLight->QuadraticAttenuation(0.01f);
	spotLight->Exponent(2);
#if CAST_SHADOWS
		spotLight->CastsShadows(true);
#endif
	m_Lights.push_back(spotLight);

	Model* model = NULL;
	//model = new Model("Data/Cube.mesh");
	//model->Position(-2, 2.5f, 0);
	//m_Models.push_back(model);

	Configuration config;

	config.LoadFrom("Data/Scene.ini");
	String modelPath;
	config.GetValue("Model", "Path", modelPath);

	model = new Model(modelPath.CStr());
	model->Position(glm::vec3(-2, 2.5f, 0));
	m_Models.push_back(model);

	m_Camera.Position(glm::vec3(0.0f, -4.0f, -10.0f));
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