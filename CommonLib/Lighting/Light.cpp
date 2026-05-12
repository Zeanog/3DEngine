#include "Light.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "LightPool.h"

#include <glm/gtc/type_ptr.hpp>

ALight::ALight() {
	m_ShadowFBO = NULL;
}

ALight::~ALight() {
}

glm::mat4	Light_Directional::m_CachedProjectionMatrix(glm::identity<glm::mat4>());

void Light_Directional::PreShadowRender() {
	//Directional Light Shadow Camera
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//Needs to be the size of all geometry affect by light
	glOrtho(-10, 10.0, -10.0, 10.0, 0.2, 500.0);

	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(m_CachedProjectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Light_Directional::PostShadowRender() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Light_Directional::RenderShadows(const Neo::Bounds& worldBounds, const Functor<void>& perLightShadowRenderHandler) {
	Light_Directional::PreShadowRender();

	FOREACH (iter, *Singleton<DirectionalLightPool>::GetInstance()) {
		if (!(*iter)->CastsShadows()) {
			continue;
		}

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glm::mat4 mat = glm::lookAt(worldBounds.GetCenter() - (*iter)->Direction() * 10.0f, worldBounds.GetCenter(), glm::up<glm::vec3>());
		glMultMatrixf(glm::value_ptr(mat));

		(*iter)->SetShadowMapAsTarget();
		perLightShadowRenderHandler();
		(*iter)->UnsetShadowMapAsTarget();

		glPopMatrix();
	}

	Light_Directional::PostShadowRender();
}

Light_Directional::Light_Directional() {
	Singleton<DirectionalLightPool>::GetInstance()->Add( this );
}

Light_Directional::Light_Directional( Float32 xDir, Float32 yDir, Float32 zDir ) {
	m_Direction[0] = xDir;
	m_Direction[1] = yDir;
	m_Direction[2] = zDir;
	Singleton<DirectionalLightPool>::GetInstance()->Add( this );
}

Light_Directional::Light_Directional(const glm::vec3& dir) {
	m_Direction = dir;
	Singleton<DirectionalLightPool>::GetInstance()->Add(this);
}

Light_Directional::~Light_Directional() {
	Singleton<DirectionalLightPool>::GetInstance()->Remove( this );
}

glm::mat4 Light_Directional::Transform() const {
	return MathUtils::CreateAxisAlong(m_Direction, glm::up<glm::vec3>());
}

const IRenderTarget* Light_Directional::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform("vLightDir", m_Direction * glm::inverse(camera.Rotation())));
	if(!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(3, program, bounds, camera);
}

glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);
const IRenderTarget* Light_Directional::LinkShadowMapTo(UInt32 location, const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	glm::mat4 depthViewMatrix = glm::lookAt(bounds.GetCenter() - m_Direction * 10.0f, bounds.GetCenter(), glm::up<glm::vec3>());
	glm::mat4 mvp = biasMatrix * m_CachedProjectionMatrix * depthViewMatrix;
	glm::mat4 cameraMat;

	camera.ToMat4x4(cameraMat);
	mvp = mvp * glm::inverse(cameraMat);
	
	verify( program.LinkUniform("mDepthMVP", mvp) );
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, location);
}

void Light_Directional::DebugRender(ShaderProgram_GLSL& program, const glm::mat4& transform)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(glm::value_ptr(transform));

	program.StartUsing();

	program.LinkUniform("vColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	GLUquadricObj *qObj = gluNewQuadric();
	gluQuadricTexture(qObj, true);
	//gluSphere(sphere, 0.25f, 64, 64);
	float height2WidthRatio = 1.0f / 5.0f;
	float height = 2;
	float width = height * height2WidthRatio;
	gluCylinder(qObj, width, 0, height, 64, 10);
	gluDeleteQuadric(qObj);

	program.StopUsing();

	glPopMatrix();
}

Light_Point::Light_Point() {
	Singleton<PointLightPool>::GetInstance()->Add( this );
}

Light_Point::Light_Point( const glm::vec3& org ) {
	m_Origin = org;

	Singleton<PointLightPool>::GetInstance()->Add( this );
}

Light_Point::Light_Point( Float32 x, Float32 y, Float32 z ) {
	m_Origin[ 0 ] = x;
	m_Origin[ 1 ] = y;
	m_Origin[ 2 ] = z;

	Singleton<PointLightPool>::GetInstance()->Add( this );
}

Light_Point::~Light_Point() {
	Singleton<PointLightPool>::GetInstance()->Remove( this );
}

const IRenderTarget* Light_Point::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform("vLightPos", m_Origin * glm::inverse(camera.Rotation())));
	verify(program.LinkUniform("fConstantAttenuation", m_ConstantAttenuation));
	verify(program.LinkUniform("fLinearAttenuation", m_LinearAttenuation));
	verify(program.LinkUniform("fQuadraticAttenuation", m_QuadraticAttenuation));

	if (!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(3, program, bounds, camera);
}

const IRenderTarget* Light_Point::LinkShadowMapTo(UInt32 location, const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	//program.LinkUniform("mDepthMVP", MathUtils::CreateAxisAlong());
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, location);
}

void Light_Spot::PreShadowRender() {
	float height = 768.0f;

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, (int)(m_AspectRatio * height), (int)height);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	/*m_ProjectionMatrix = glm::perspective(
		glm::radians(FOV()),
		m_AspectRatio,
		0.1f,
		5.0f
	);*/

	////glOrtho(-10.0, 10.0, -10.0, 10.0, 1.0f, 25.0);
	////gluPerspective(MathUtils::Radians2Deg(m_FOV), m_AspectRatio, 1, 25.0);
	//gluPerspective(60.0f, m_AspectRatio, 1, 25.0);
	////glFrustum(-10, 10, -10, 10, 0.1f, 25.0f);

	//glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(m_ProjectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Light_Spot::PostShadowRender() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

Light_Spot::Light_Spot() {
	Singleton<SpotLightPool>::GetInstance()->Add( this );
}

void Light_Spot::RenderShadows(const Functor<void>& perLightShadowRenderHandler, const ICamera& camera) {
	FOREACH(iter, *Singleton<SpotLightPool>::GetInstance()) {
		if (!(*iter)->CastsShadows()) {
			continue;
		}

		(*iter)->PreShadowRender();

		//glm::mat4 m = (*iter)->AsCameraTransform();
		glm::mat4 m = glm::lookAt((*iter)->Position(), (*iter)->Position() + (*iter)->Direction() * (*iter)->Distance(), glm::up<glm::vec3>());
		glMultMatrixf(glm::value_ptr(m));

		(*iter)->SetShadowMapAsTarget();
		perLightShadowRenderHandler();
		(*iter)->UnsetShadowMapAsTarget();

		(*iter)->PostShadowRender();
	}
}

Light_Spot::Light_Spot( const glm::vec3& org, const glm::mat4& rot, Float32 fovInRadians, Float32 aspectRatio ) {
	m_Transform = rot;
	m_Transform[3] = glm::vec4(org, 1.0f);
	FOV(fovInRadians);
	m_AspectRatio = aspectRatio;
	Singleton<SpotLightPool>::GetInstance()->Add( this );
}

Light_Spot::~Light_Spot() {
	Singleton<SpotLightPool>::GetInstance()->Remove( this );
}

const IRenderTarget* Light_Spot::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform("fLightCosCutoff", m_CosCutoff));

	glm::vec3 offset(Position() + camera.Position());
	offset = offset * glm::inverse(camera.Rotation());
	verify(program.LinkUniform("vLightPos", offset));
	verify(program.LinkUniform("vLightDirection", Direction()));
	verify(program.LinkUniform("vEyePos", -camera.Position()));
	
	if (!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(3, program, bounds, camera);
}

const IRenderTarget* Light_Spot::LinkShadowMapTo(UInt32 location, const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	glm::mat4 spotView = glm::lookAt(Position(), Position() + Direction(), glm::up<glm::vec3>());
	glm::mat4 spotProj = glm::perspective(FOV(), AspectRatio(), 0.1f, m_Distance);
	glm::mat4 mvp = spotProj * spotView;

	verify(program.LinkUniform("mDepthMVP", mvp));
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, location);
}

const glm::mat4& Light_Spot::AsCameraTransform() const {
	return m_Transform;
}

void Light_Spot::DebugRender(ShaderProgram_GLSL& program, const glm::mat4& transform) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(glm::value_ptr(transform));

	program.StartUsing();

	program.LinkUniform("vColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	GLUquadricObj* qObj = gluNewQuadric();
	gluQuadricTexture(qObj, true);
	float height = 2.0f;
	float width = std::tanf(FOV() * 0.5f) * height * 2.0f;
	gluCylinder(qObj, 0, width, height, 64, 10);
	gluDeleteQuadric(qObj);

	program.StopUsing();

	glPopMatrix();
}