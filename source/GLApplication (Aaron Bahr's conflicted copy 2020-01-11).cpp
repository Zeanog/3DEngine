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
#include "Rendering/MeshManager.h"

#include "System/Input/InputSystem.h"

#include "System/Win32/Window.h"
#include "System/Flash/SWF.h"

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

	if (!(m_hdc = GetDC(m_hWnd)))
		return FALSE;

	if (!(pixelFormat = ChoosePixelFormat(m_hdc, &pfd)))
		return FALSE;

	if (!SetPixelFormat(m_hdc, pixelFormat, &pfd))
		return FALSE;

	if (!(m_hrc = wglCreateContext(m_hdc)))
		return FALSE;

	if (!wglMakeCurrent(m_hdc, m_hrc))
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

	OnMouseChangedFunctor.Set(this, &TSelf::OnMouseChanged);
	Singleton<InputSystem>::GetInstance()->GetMouse()->OnChanged += OnMouseChangedFunctor;

	OnKeyboardChangedFunctor.Set(this, &TSelf::OnKeyboardChanged);
	Singleton<InputSystem>::GetInstance()->GetKeyboard()->OnKeydown += OnKeyboardChangedFunctor;

	loadAssets();

	m_lastTick = GetTickCount();

	m_Camera.Position(0.0f, -4.0f, -10.0f);

	CoInitialize(NULL);

	return true;
}

GLApplication::~GLApplication() {
	CoUninitialize();
}

FlashEngine flashEngine;
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
		Singleton<MeshManager>::GetInstance()->ReloadAll();
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

#include "Math/MathUtils.h"

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

	if (m_Models.size() >= 1) {
		m_Models[0]->Rotate(0, m_DeltaTime, m_DeltaTime);
	}
}

#include "Lighting/LightPool.h"

#define CAST_SHADOWS 0

void GLApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

#if CAST_SHADOWS
	//Directional Light Shadow Camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10.0, 10.0, -10.0, 10.0, 0.2, 100.0);
	//

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	Float32	lightTransform[16];
	for (DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin(), end = Singleton<DirectionalLightPool>::GetInstance()->End(); iter != end; ++iter) {
		if (!(*iter)->CastsShadows()) {
			continue;
		}
		//lightTransform = MathUtils::CreateAxisAlong(-(*iter)->GetDirection(), Vector<3>::Zero(), Vector<3>::Up());

		glPushMatrix();
		glLoadIdentity();

		glRotatef(90, 1, 0, 0);
		glTranslatef(cameraPosition[0], cameraPosition[1], cameraPosition[2]);

		(*iter)->AttachShadowMap();
		for (int i = 0; i < m_Models.size(); ++i) {
			m_Models[i]->Render(m_ShadowMapGenerationProgram);
		}
		(*iter)->DetachShadowMap();

		glPopMatrix();
	}

	glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)m_windowWidth / (GLfloat)m_windowHeight, 0.1f, 100.0f);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_Camera.LinkTransform();

	m_RenderModelProgram.StartUsing();

	// Render our geometry into the FBO
	m_multipleRenderTarget->start();
	for (int i = 0; i < m_PrevModels.size(); ++i) {
		m_PrevModels[i]->render(m_RenderModelProgram);
	}

	for (int i = 0; i < m_Models.size(); ++i) {
		m_Models[i]->Render(m_RenderModelProgram);
	}
	m_multipleRenderTarget->stop();

	m_RenderModelProgram.StopUsing();

	// Render to the screen
	if (m_state == 0)
	{
		for (DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin(), end = Singleton<DirectionalLightPool>::GetInstance()->End(); iter != end; ++iter) {
			m_deferredRendering->Render(m_CompositeProgram, m_Camera.Position(), *iter);
		}
	}
	else if (m_state == 1)
	{
		m_multipleRenderTarget->showTexture(0, 512, 384, 0);
		m_multipleRenderTarget->showTexture(1, 512, 384, 512);
		m_multipleRenderTarget->showTexture(2, 512, 384, 0, 384);
		//(*iter)->DebugRender(m_multipleRenderTarget->Width(), m_multipleRenderTarget->Height(), 512, 384, 512, 384);
	}

	SwapBuffers(m_hdc);
}

/**
*	Release all the GL resources we have allocated
*/
void GLApplication::release()
{
	releaseAssets();

	wglMakeCurrent(m_hdc, 0);
	wglDeleteContext(m_hrc);

	ReleaseDC(m_hWnd, m_hdc);
}

/**
*	Load all the required assets
*/
void GLApplication::loadAssets()
{
	m_state = 0;

	m_multipleRenderTarget = new FBORenderTexture(m_windowWidth, m_windowHeight);
	m_deferredRendering = new DeferredRendering(m_windowWidth, m_windowHeight, m_multipleRenderTarget, m_CompositeProgram);

	verify(m_RenderModelProgram.Create("data/deferredShading.vert", "data/deferredShading.frag"));

	List<StaticString>	uniformNames;
	m_RenderModelProgram.EnumerateUniforms(uniformNames);

	verify(m_ShadowMapGenerationProgram.Create(StaticString("data/shadowMap.vert"), StaticString("data/shadowMap.frag")));

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

	//m_Lights.push_back(new Light_Point(0.0f, 0.0f, 50.0f, 0.1f));
	Vector<3> dir = Vector<3>(0.0f, -1.0f, 0.0f);
	dir.Normalize();
	ALight* light = new Light_Directional(dir);
	//light->CastsShadows(true);
	m_Lights.push_back(light);
	//m_Lights.push_back(new Light_Spot(Vector<3>(0.0f, 100.0f, 0.0f), Vector<3>(0.0f, -1.0f, 0.0f), 1.0f));

	Model* model = new Model();
	model->ReadFrom("data/Cube.model");
	model->Position(-2, 2.5f, 0);
	m_Models.push_back(model);

	ShadowMap::BuildBiasMatrix();
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