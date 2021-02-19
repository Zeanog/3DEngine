#include "Light.h"
#include "../Shaders/ShaderProgram_GLSL.h"
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
	glOrtho(-7.0, 7.0, -7.0, 7.0, 0.2, 500.0);

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

glm::mat4 Light_Directional::ToMat4x4() const {
	return MathUtils::CreateAxisAlong(m_Direction, glm::up<glm::vec3>());
}

const RenderTarget* Light_Directional::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform(StaticString("vLightDir"), m_Direction * glm::inverse(camera.Rotation())));
	if(!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(program, bounds, camera);
}

glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);
const RenderTarget* Light_Directional::LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	glm::mat4 depthViewMatrix = glm::lookAt(bounds.GetCenter() - m_Direction * 10.0f, bounds.GetCenter(), glm::up<glm::vec3>());
	glm::mat4 mvp = biasMatrix * m_CachedProjectionMatrix * depthViewMatrix;
	
	mvp = mvp * glm::inverse(camera.ToMat4x4());
	
	verify( program.LinkUniform("mDepthMVP", mvp) );
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, 3);
}

void Light_Directional::DebugRender(const ShaderProgram_GLSL& program, const glm::mat4& transform)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(glm::value_ptr(transform));

	program.StartUsing();

	program.LinkUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

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

Light_Point::Light_Point( const glm::vec3& org, Float32 radius ) {
	m_Origin = org;
	m_Radius = radius;

	Singleton<PointLightPool>::GetInstance()->Add( this );
}

Light_Point::Light_Point( Float32 x, Float32 y, Float32 z, Float32 radius ) {
	m_Origin[ 0 ] = x;
	m_Origin[ 1 ] = y;
	m_Origin[ 2 ] = z;
	m_Radius = radius;

	Singleton<PointLightPool>::GetInstance()->Add( this );
}

Light_Point::~Light_Point() {
	Singleton<PointLightPool>::GetInstance()->Remove( this );
}

const RenderTarget* Light_Point::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform(StaticString("vLightPos"), m_Origin));
	verify(program.LinkUniform(StaticString("fLightRadius"), m_Radius));
	if (!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(program, bounds, camera);
}

const RenderTarget* Light_Point::LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	//program.LinkUniform("mDepthMVP", MathUtils::CreateAxisAlong());
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, 3);
}

glm::mat4	Light_Spot::m_CachedProjectionMatrix(glm::identity<glm::mat4>());

void Light_Spot::PreShadowRender() {
	//Directional Light Shadow Camera
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//Needs to be the size of all geometry affect by light
	glOrtho(-10.0, 10.0, -10.0, 10.0, 0.2, 100.0);

	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(m_CachedProjectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Light_Spot::PostShadowRender() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Light_Spot::Light_Spot() {
	m_Radius = 0.0f;
	Singleton<SpotLightPool>::GetInstance()->Add( this );
}

Light_Spot::Light_Spot( const glm::vec3& org, const glm::vec3& dir, Float32 radius ) {
	m_Origin = org;
	m_Direction = dir;
	m_Radius = radius;
	Singleton<SpotLightPool>::GetInstance()->Add( this );
}

Light_Spot::~Light_Spot() {
	Singleton<SpotLightPool>::GetInstance()->Remove( this );
}

const RenderTarget* Light_Spot::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform(StaticString("vLightDir"), m_Direction * glm::inverse(camera.Rotation())));
	verify(program.LinkUniform(StaticString("vEyeDir"), camera.Forward()));
	return TSuper::LinkTo(program, bounds, camera);
}

const RenderTarget* Light_Spot::LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	//program.LinkUniform("mDepthMVP", MathUtils::CreateAxisAlong());
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, 3);
}