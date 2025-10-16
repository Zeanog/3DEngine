#include <windows.h>
#include "GLApplication.h"
#include "Model/SphereModel.h"
#include "Model/CubeModel.h"
#include "Model/PlaneModel.h"

#include "System/Typedefs.h"
#include "System/Input/VirtualKey.h"
#include "Images/ImageManager.h"
#include "System/Audio/Loaders/SoundManager.h"
#include "Lighting/Light.h"
#include "Rendering/Model.h"

#include "System/Input/InputSystem.h"
#include "System/Audio/AudioSystem.h"
#include "System\Audio\SubmixVoice.h"
#include "System/Configuration.h"
#include "Math/MathUtils.h"
#include "Lighting/LightPool.h"
#include "System/Win32/Window.h"

Int64		xMousePrev = 0;
Int64		yMousePrev = 0;

CComModule	_module;

/**
*	Initialize our GL application
*/
bool GLApplication::Initialize(HWND hwnd, int width, int height)
{
	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) return false;

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
	SetSize(width, height);

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

	verify( Singleton<AudioSystem>::GetInstance()->Init() );

	LoadAssets();

	m_PrevTick = (decltype(m_PrevTick))GetTickCount64();

	OnRenderShadows.AddListener(this, &TSelf::RenderModelShadows);

	return true;
}

GLApplication::~GLApplication() {
	CoUninitialize();
}


void	GLApplication::OnKeyboardChanged(Param<KeyboardState>::Type keyboardState) {
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

	if (keyboardState.KeyWasPressed(DIK_F1)) {
		showDeferredRendering();
	}
	else if (keyboardState.KeyWasPressed(DIK_F2)) {
		showRenderTargets();
	}

	if (keyboardState.KeyWasPressed(DIK_R)) {
		Singleton<ImageManager>::GetInstance()->ReloadAll();
		Singleton<AudioSystem>::GetInstance()->ReloadAssets();
	}

	if (keyboardState.KeyWasPressed(DIK_F3) ) {
		//OutputDebugString(String::Format("F3: Open?: %d\n", Singleton<DebugConsole>::GetInstance()->IsOpen()));
		//TODO: Find a way to let us close the debug console with out closing the entire program
		if (!Singleton<DebugConsole>::GetInstance()->IsOpen()) {
			verify(Singleton<DebugConsole>::GetInstance()->Open());
			//SetFocus(m_hWnd);
		}
		else {
			Singleton<DebugConsole>::GetInstance()->Close();
		}
	}

	if (keyboardState.KeyWasPressed(DIK_F4)) {
		if (!Singleton<AudioSystem>::GetInstance()->IsEffectEnabled("Music", 0)) {
			Singleton<AudioSystem>::GetInstance()->EnableEffect("Music", 0);
		}
		else {
			Singleton<AudioSystem>::GetInstance()->DisableEffect("Music", 0);
		}
	}
}

/**
*	Set window's size
*/
void GLApplication::SetSize(int w, int h)
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

void GLApplication::Update()
{
	m_PrevTick = m_CurrentTime;
	m_CurrentTime = (decltype(m_CurrentTime))GetTickCount64();
	UInt64 delta = m_CurrentTime - m_PrevTick;
	Float32 time = MathUtils::MilliSec2Sec(delta);

	m_DeltaTime = MathUtils::Min(time, 1.0f / 30.0f);

	Singleton<InputSystem>::GetInstance()->Poll();

	m_Camera.Update(m_DeltaTime);

	if (m_PrevModels.size() >= 1) {
		m_PrevModels[0]->Rotate(m_DeltaTime, m_DeltaTime, 0);
	}

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

void GLApplication::Render()
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
void GLApplication::Release()
{
	//Shutdown the AudioSystem before destroying assets
	Singleton<AudioSystem>::GetInstance()->Destroy();
	ReleaseAssets();

	wglMakeCurrent(m_hDC, 0);
	wglDeleteContext(m_hRC);

	ReleaseDC(m_hWnd, m_hDC);

	CoUninitialize();
}

#include "Rendering/ModelLoaders/MeshManager.h"
#include <System/Audio/SourceVoice.h>
#include <System/Audio/Sound.h>
#include "System\Audio\ReverbParameters.h"

#include "System/Reflector.h"
#include "System/JsonSerializer.h"

/**
*	Load all the required assets
*/
void GLApplication::LoadAssets()
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
	m->LoadImage(StaticString("data/125881.tga"));
	m->Position(2, 2.5f, 0);

	/*m = new CubeModel(1);
	m_PrevModels.push_back( m );
	m->loadImage( StaticString("data/box.raw") );
	m->setPosition(-2,2.5f,0);
	m->setRotation( 0.0f, 0.0f, 0.0f );*/

	m = new PlaneModel(5);
	m_PrevModels.push_back(m);
	m->LoadImage(StaticString("data/DragonsDogma.tga"));
	m->Position(0, 0, 0);

	m = new PlaneModel(5);
	m_PrevModels.push_back(m);
	m->LoadImage(StaticString("data/DragonsDogma.tga"));
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

	rapidjson::Document	doc;
	verify(rapidjson::LoadFrom("Data/Scene.json", doc));

	assert(doc["Model"].IsObject());
	auto& docVal = doc["Model"];
	StaticString modelPath = docVal.FindMember("Path")->value.GetString();
	model = new Model(modelPath.CStr());
	model->Position(glm::vec3(-2, 2.5f, 0));
	m_Models.push_back(model);

	Singleton<AudioSystem>::GetInstance()->AddCategory("Music", 1, 44100);
	Singleton<AudioSystem>::GetInstance()->AddCategory("Fx", 1, 44100);

	auto numEffects = Singleton<AudioSystem>::GetInstance()->LoadEffects("Data/TestReverb.json", "Music");

	SourceVoice* selectedVoice{};

	auto& musicValue = doc["Music"];
	FOREACH(iter, musicValue) {
		StaticString musicPath( iter->GetString() );
		auto musicDuration = Singleton<AudioSystem>::GetInstance()->Play(musicPath, "Music", selectedVoice);
		//selectedVoice->Volume(0.1f);
	}

	assert(doc["TestSound"].IsObject());
	StaticString soundPath( doc["TestSound"].FindMember("Path")->value.GetString() );

	auto fxDuration = Singleton<AudioSystem>::GetInstance()->Play(soundPath, "Fx", selectedVoice);

	m_Camera.Position(glm::vec3(0.0f, -4.0f, -10.0f));
}

#include "Shaders/ShaderProgramManager.h"
#include "Shaders/ShaderManager_Vertex.h"
#include "Shaders/ShaderManager_Fragment.h"

void GLApplication::ReleaseAssets()
{
	DeletePtr(m_multipleRenderTarget);
	DeletePtr(m_deferredRendering);

	Destroy(m_PrevModels);
	Destroy(m_Models);
	Destroy(m_Lights);

	Singleton<ImageManager>::GetInstance()->Shutdown();
	Singleton<SoundManager>::GetInstance()->Shutdown();
	Singleton<MeshManager>::GetInstance()->Shutdown();

	Singleton<ShaderManager_Vertex>::GetInstance()->Shutdown();
	Singleton<ShaderManager_Fragment>::GetInstance()->Shutdown();
	Singleton<ShaderProgramManager>::GetInstance()->Shutdown();
}