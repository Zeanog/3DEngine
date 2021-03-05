#pragma once

#include "System/StaticString.h"
#include "Math/Bounds.h"
#include "System/Functors/Functor.h"

#include "Rendering/Camera.h"
#include "Rendering/FrameBufferObject.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class ShaderProgram_GLSL;
class Camera;

class ALight {
	CLASS_TYPEDEFS( ALight );

protected:
	virtual void		InitShadowMap() {
		m_ShadowFBO->Bind();
		m_ShadowFBO->AddTarget("tShadowMap", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		m_ShadowFBO->Unbind();
	}

public:
	Bool				CastsShadows() const {
		return m_ShadowFBO != NULL;
	}

	void				CastsShadows( Bool castsShadows ) {
		if (CastsShadows() && !castsShadows) {
			DeletePtr(m_ShadowFBO);
		}
		else if( !CastsShadows() && castsShadows ) {
			m_ShadowFBO = new FrameBufferObject_Depth(1024, 768);
			InitShadowMap();
		}
	}

	void SetShadowMapAsTarget() {
		if (CastsShadows()) {
			m_ShadowFBO->SetAsTarget();
		}
	}

	void UnsetShadowMapAsTarget() {
		if (CastsShadows()) {
			m_ShadowFBO->UnsetAsTarget();
		}
	}

	virtual 	const RenderTarget*				LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const = 0;

	void				DebugRenderMap(float xPos, float yPos, float renderWidth, float renderHeight) const {
		m_ShadowFBO->showTexture("tShadowMap", renderWidth, renderHeight, xPos, yPos);
	}

	virtual void DebugRender(const ShaderProgram_GLSL& program, const glm::mat4& transform)
	{
		
	}

protected:
	FrameBufferObject*			m_ShadowFBO;

public:
	ALight();

	virtual ~ALight();

	virtual const RenderTarget*	LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const = 0;
};

class Light_Directional : public ALight {
	INHERITEDCLASS_TYPEDEFS( Light_Directional, ALight );

public:
	static void	PreShadowRender();
	static void	PostShadowRender();
	static void RenderShadows(const Neo::Bounds& worldBounds, const Functor<void>& perLightRenderHandler);

protected:
	static glm::mat4	m_CachedProjectionMatrix;

	glm::vec3			m_Direction;

public:
	Light_Directional();
	Light_Directional( Float32 xDir, Float32 yDir, Float32 zDir );
	Light_Directional(const glm::vec3& dir);
	virtual ~Light_Directional();

	DECLARE_GETSET(Direction)

	glm::mat4		ToMat4x4() const;

	virtual const RenderTarget*		LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	virtual const RenderTarget*		LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;

	virtual void DebugRender(const ShaderProgram_GLSL& program, const glm::mat4& transform) override;
};

class Light_Point : public ALight {
	INHERITEDCLASS_TYPEDEFS( Light_Point, ALight );

public:

protected:
	glm::vec3	m_Origin;

protected:
	virtual void		InitShadowMap() override {
		m_ShadowFBO->Bind();
		m_ShadowFBO->AddTarget("tShadowMap0", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		//m_ShadowFBO->AddTarget("tShadowMap1", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		//m_ShadowFBO->AddTarget("tShadowMap2", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		//m_ShadowFBO->AddTarget("tShadowMap3", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		//m_ShadowFBO->AddTarget("tShadowMap4", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		//m_ShadowFBO->AddTarget("tShadowMap5", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		m_ShadowFBO->Unbind();
	}

public:
	Light_Point();
	virtual ~Light_Point();

	Light_Point( const glm::vec3& org );

	Light_Point( Float32 x, Float32 y, Float32 z );

	DECLARE_GETSET(Origin)

	virtual const RenderTarget*	LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	virtual 	const RenderTarget*				LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
};

class Light_Spot : public ALight {
	INHERITEDCLASS_TYPEDEFS( Light_Spot, ALight);

public:
	static void RenderShadows(const Functor<void>& perLightRenderHandler);

protected:
	glm::mat4	m_ProjectionMatrix;

	glm::mat4	m_Transform;

	float		m_ConstantAttenuation;
	float		m_LinearAttenuation;
	float		m_QuadraticAttenuation;
	float		m_Exponent;
	float		m_CosCutoff;

	Float32		m_FOV;
	Float32		m_AspectRatio;

protected:
	void	PreShadowRender();
	void	PostShadowRender();

	virtual void		InitShadowMap() override {
		m_ShadowFBO->Bind();
		m_ShadowFBO->AddTarget("tShadowMap", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
		m_ShadowFBO->Unbind();
	}

public:
	Light_Spot();
	Light_Spot( const glm::vec3& org, const glm::mat4& rot, Float32 fov, Float32 aspectRatio );
	virtual ~Light_Spot();

	DECLARE_GETSET(Transform)
	DECLARE_GETSET(ConstantAttenuation)
	DECLARE_GETSET(LinearAttenuation)
	DECLARE_GETSET(QuadraticAttenuation)
	DECLARE_GETSET(Exponent)
	DECLARE_GETSET(AspectRatio)
	DECLARE_GETSET(ProjectionMatrix)

	void FOV(Float32 fov) {
		m_FOV = fov;
		m_CosCutoff = std::cos(FOV() * 0.5f);
	}

	Float32 FOV() const {
		return m_FOV;
	}

	glm::vec3	Position() const {
		return m_Transform[3];
	}

	void		Position(const glm::vec3& pt) {
		m_Transform[3] = glm::vec4(pt, 1.0f);
	}

	glm::vec3	Direction() const {
		return m_Transform[2];
	}

	/*void	Direction(const glm::vec3& dir) {
		m_Transform[2] = glm::vec4(dir, 1.0f);
	}*/

	virtual const RenderTarget*			LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	virtual 	const RenderTarget*		LinkShadowMapTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	void								DebugRender(const ShaderProgram_GLSL& program, const glm::mat4& transform);

	glm::mat4		ToMat4x4() const;
};