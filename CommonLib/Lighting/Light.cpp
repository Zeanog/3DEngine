#include "Light.h"
#include "../Shaders/ShaderProgram_GLSL.h"
#include "LightPool.h"

#include <glm/gtc/type_ptr.hpp>

ALight::ALight() {
	m_ShadowFBO = NULL;
}

ALight::~ALight() {
}

glm::mat4	Light_Directional::m_CachedProjectionMatrix;

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

glm::mat4 Light_Directional::ToMat4x4_Local(const glm::vec3& focusPt, float offset) const {// AOB: This is wrong!!
	glm::mat4 mat(MathUtils::CreateAxisAlong(m_Direction, glm::up<glm::vec3>()));
	mat[3] = glm::vec4(focusPt, 1.0f) - glm::vec4(m_Direction * offset, 0.0f);
	return mat;
}

glm::mat4 Light_Directional::ToMat4x4_World( const glm::vec3& focusPt, float offset ) const {
	return MathUtils::CreateAxisAlong_World(m_Direction, focusPt - m_Direction * offset, glm::up<glm::vec3>());
}

void WorldToCamera(glm::mat4x4& matWorld) {
	matWorld = glm::inverse(matWorld);
	//matWorld[1] = -matWorld[1];
	//matWorld[3][0] = -matWorld[3][0];
	//matWorld[3][1] = -matWorld[3][1];
	//matWorld[3][2] = -matWorld[3][2];
}

glm::mat4 Light_Directional::ToMat4x4_Camera(const glm::vec3& focusPt, float offset) const {
	glm::mat4 mat = MathUtils::CreateAxisAlong_World(m_Direction, focusPt - m_Direction * offset, glm::up<glm::vec3>());
	WorldToCamera(mat);
	return mat;
}

const RenderTarget* Light_Directional::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform(StaticString("vLightDir"), m_Direction * glm::inverse(camera.Rotation())));
	if(!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(program, bounds, camera);
}

const RenderTarget* Light_Directional::LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	glm::mat4x4 mv = MathUtils::CreateAxisAlong_Local(m_Direction, bounds.GetCenter(), glm::up<glm::vec3>());
	//glm::mat4x4 mv = ToMat4x4_Local(bounds.GetCenter(), 20.0f);
	glm::mat4 mvp = m_CachedProjectionMatrix * mv;
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

	GLUquadricObj *sphere = gluNewQuadric();
	gluQuadricTexture(sphere, true);
	gluSphere(sphere, 0.25f, 64, 64);
	gluDeleteQuadric(sphere);

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

glm::mat4	Light_Spot::m_CachedProjectionMatrix;

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