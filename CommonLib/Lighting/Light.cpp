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

const RenderTarget* Light_Point::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform(StaticString("vLightPos"), m_Origin * glm::inverse(camera.Rotation())));
	if (!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(program, bounds, camera);
}

const RenderTarget* Light_Point::LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	//program.LinkUniform("mDepthMVP", MathUtils::CreateAxisAlong());
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, 3);
}

void Light_Spot::PreShadowRender() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//glOrtho(-10.0, 10.0, -10.0, 10.0, 0.2, 100.0);
	gluPerspective(MathUtils::Radians2Deg(m_FOV), m_AspectRatio, 1, 100.0);

	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(m_ProjectionMatrix));

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
	Singleton<SpotLightPool>::GetInstance()->Add( this );
}

void Light_Spot::RenderShadows(const Functor<void>& perLightShadowRenderHandler) {
	//glm::mat4 projectionMatrix;
	FOREACH (iter, *Singleton<SpotLightPool>::GetInstance()) {
		if (!(*iter)->CastsShadows()) {
			continue;
		}

		(*iter)->PreShadowRender();

		glm::mat4 mat = glm::lookAt((*iter)->Position(), (*iter)->Position() + (*iter)->Direction() * 30.0f, glm::forward<glm::vec3>());  //MathUtils::CreateAxisAlong((*iter)->Direction(), glm::right<glm::vec3>());
		//mat[3] = glm::vec4(-(*iter)->Position(), 1.0f);
		glMultMatrixf(glm::value_ptr(mat));

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

const RenderTarget* Light_Spot::LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	verify(program.LinkUniform(StaticString("fConstantAttenuation"), m_ConstantAttenuation));
	verify(program.LinkUniform(StaticString("fLinearAttenuation"), m_LinearAttenuation));
	verify(program.LinkUniform(StaticString("fQuadraticAttenuation"), m_QuadraticAttenuation));
	verify(program.LinkUniform(StaticString("fExponent"), m_Exponent));
	verify(program.LinkUniform(StaticString("fLightCosCutoff"), m_CosCutoff));

	glm::vec3 offset(Position() + camera.Position());
	offset = offset * glm::inverse(camera.Rotation());
	verify(program.LinkUniform(StaticString("vLightPos"), offset));
	verify(program.LinkUniform(StaticString("vLightDirection"), Direction() * glm::inverse(camera.Rotation())));
	verify(program.LinkUniform(StaticString("vEyePos"), -camera.Position()));
	
	if (!CastsShadows()) {
		return NULL;
	}

	return LinkShadowMapTo(program, bounds, camera);
}

const RenderTarget* Light_Spot::LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const {
	glm::mat4 mvp = biasMatrix * m_ProjectionMatrix * ToMat4x4();

	mvp = mvp * glm::inverse(camera.ToMat4x4());

	verify(program.LinkUniform("mDepthMVP", mvp));
	return m_ShadowFBO->LinkTargetTo("tShadowMap", program, 3);
}

glm::mat4 Light_Spot::ToMat4x4() const {
	return m_Transform;
}

void Light_Spot::DebugRender(const ShaderProgram_GLSL& program, const glm::mat4& transform)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(glm::value_ptr(transform));

	program.StartUsing();

	program.LinkUniform("vColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	GLUquadricObj* qObj = gluNewQuadric();
	gluQuadricTexture(qObj, true);
	float height = 5.0f;
	float width = std::tanf(FOV() * 0.5f) * height * 2.0f;
	gluCylinder(qObj, 0, width, height, 64, 10);
	gluDeleteQuadric(qObj);

	program.StopUsing();

	glPopMatrix();
}