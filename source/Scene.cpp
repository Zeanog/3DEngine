#include "Scene.h"

#include "Rendering/Model.h"
#include "Lighting/Light.h"
#include "Lighting/LightPool.h"
#include "System/Clock.h"

#include "glm/vec4.hpp"

void Scene::Update() {
	/*if (m_PrevModels.size() >= 1) {
		m_PrevModels[0]->Rotate(m_DeltaTime, m_DeltaTime, 0);
	}*/

	/*if (m_PrevModels.size() >= 2) {
		m_PrevModels[1]->addRotation(0, time, time);
	}*/

	//Just for debugging
	DirectionalLightPool::Iterator dirIter = Singleton<DirectionalLightPool>::GetInstance()->Begin();
	if (dirIter != Singleton<DirectionalLightPool>::GetInstance()->End()) {
		glm::vec3 currAngles;
		currAngles.x = -90.0f;
		currAngles.y = 45.0f * std::sin(Singleton<SystemClock>::GetInstance()->Time());
		currAngles.z = 0.0f;
		glm::vec3 dir = glm::forward<glm::vec4>() * glm::quat(glm::vec3(MathUtils::Deg2Radians(currAngles.x), MathUtils::Deg2Radians(currAngles.y), MathUtils::Deg2Radians(currAngles.z)));
		(*dirIter)->Direction(dir);
	}

	SpotLightPool::Iterator spotIter = Singleton<SpotLightPool>::GetInstance()->Begin();
	if (spotIter != Singleton<SpotLightPool>::GetInstance()->End()) {
		glm::vec3 currAngles;
		currAngles.x = MathUtils::Deg2Radians(90.0f);
		currAngles.y = MathUtils::Deg2Radians(30.0f * std::sin(Singleton<SystemClock>::GetInstance()->Time()));
		currAngles.z = MathUtils::Deg2Radians(0.0f);
		glm::mat4 t = glm::eulerAngleXYZ(currAngles.x, currAngles.y, currAngles.z);
		t[3] = glm::vec4((*spotIter)->Position(), 1.0f);
		(*spotIter)->Transform(t);
	}

	//(*iter)->Position(glm::vec3(0.0f, 10.0f, 0.0f) + 3.0f * glm::vec3(std::sin(MathUtils::MilliSec2Sec(m_CurrentTime)), 0.0f, 0.0f));

	if (m_Models.size() >= 1) {
		m_Models[0]->Rotation(glm::quat(glm::vec3(0, Singleton<SystemClock>::GetInstance()->DeltaTime(), 0)) * m_Models[0]->Rotation());
	}

	if (m_Models.size() >= 2) {
		m_Models[1]->Rotation(glm::quat(glm::vec3(0, Singleton<SystemClock>::GetInstance()->DeltaTime(), 0)) * m_Models[1]->Rotation());
	}

	/*for (UInt32 i = 0; i < m_PrevModels.size(); ++i) {
		m_Bounds += m_PrevModels[i]->GetBounds();
	}*/

	for (UInt32 i = 0; i < m_Models.size(); ++i) {
		m_Bounds += m_Models[i]->GetBounds();
	}
}

void RenderModels() {
	for (UInt32 i = 0; i < m_Models.size(); ++i) {
		m_Models[i]->Render(m_RenderModelProgram);
	}

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
}

void Scene::Render() {
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

	for (UInt32 i = 0; i < m_Models.size(); ++i) {
		m_Models[i]->Render(m_RenderModelProgram);
	}

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

		DirectionalLightPool::Iterator iter = Singleton<DirectionalLightPool>::GetInstance()->Begin();
		//SpotLightPool::Iterator iter = Singleton<SpotLightPool>::GetInstance()->Begin();
		(*iter)->DebugRenderMap(512, 384, 512, 384);
	}

	SwapBuffers(m_hDC);
}