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
	CLASS_TYPEDEFS( ALight )

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
			m_ShadowFBO = new FrameBufferObject_Depth(1024, 768);//ALLOCATING THIS HERE IS REALLY DUMB BUT IT WILL DO FOR NOW
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

	virtual 	const IRenderTarget*				LinkShadowMapTo(UInt32 location, const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const = 0;

	void				DebugRenderMap(float xPos, float yPos, float renderWidth, float renderHeight) const {
		m_ShadowFBO->showTexture("tShadowMap", renderWidth, renderHeight, xPos, yPos);
	}

	virtual void DebugRender(ShaderProgram_GLSL& program, const glm::mat4& transform)
	{
		
	}

protected:
	FrameBufferObject* m_ShadowFBO;

public:
	ALight();

	virtual ~ALight();

	virtual const IRenderTarget*	LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const = 0;
};

class Light_Directional : public ALight {
	INHERITED_CLASS_TYPEDEFS( Light_Directional, ALight )

public:
	static void	PreShadowRender();
	static void	PostShadowRender();
	static void RenderShadows(const Neo::Bounds& worldBounds, const Functor<void>& perLightRenderHandler);

protected:
	static glm::mat4	m_CachedProjectionMatrix;

	glm::vec3			m_Direction{};

public:
	Light_Directional();
	Light_Directional( Float32 xDir, Float32 yDir, Float32 zDir );
	Light_Directional(const glm::vec3& dir);
	virtual ~Light_Directional();

	DECLARE_GETSET(Direction)

	glm::mat4				Transform() const;

	virtual const IRenderTarget*		LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	virtual const IRenderTarget*		LinkShadowMapTo(UInt32 location, const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;

	virtual void DebugRender(ShaderProgram_GLSL& program, const glm::mat4& transform) override;
};

class Light_Point : public ALight {
	INHERITED_CLASS_TYPEDEFS( Light_Point, ALight )

public:

protected:
	glm::vec3	m_Origin{};
	float		m_ConstantAttenuation{};
	float		m_LinearAttenuation{};
	float		m_QuadraticAttenuation{};

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
	DECLARE_GETSET(ConstantAttenuation)
	DECLARE_GETSET(LinearAttenuation)
	DECLARE_GETSET(QuadraticAttenuation)

	virtual const IRenderTarget*		LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	virtual 	const IRenderTarget*	LinkShadowMapTo(UInt32 location, const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
};

class Light_Spot : public ALight {
	INHERITED_CLASS_TYPEDEFS( Light_Spot, ALight)

public:
	static void RenderShadows(const Functor<void>& perLightRenderHandler, const ICamera& camera);

protected:
	glm::mat4	m_ProjectionMatrix{};

	glm::mat4	m_Transform{glm::identity<glm::mat4>()};
	glm::vec3*	m_Position = reinterpret_cast<glm::vec3*>(&m_Transform[3]);
	glm::vec3*	m_Direction = reinterpret_cast<glm::vec3*>(&m_Transform[2]);

	Float32		m_CosCutoff = 0.0f;

	Float32		m_FOV = 0.0f;
	Float32		m_AspectRatio = 0.0f;
	Float32		m_Distance = 100.0f;

protected:
	void	PreShadowRender();
	void	PostShadowRender();

public:
	Light_Spot();
	Light_Spot( const glm::vec3& org, const glm::mat4& rot, Float32 fov, Float32 aspectRatio );
	virtual ~Light_Spot();

	DECLARE_GETSET(Transform)
	DECLARE_GETSET(AspectRatio)
	DECLARE_GETSET(ProjectionMatrix)
	DECLARE_GETSET(Distance)

	void FOV(Float32 fov) {
		m_FOV = fov;
		m_CosCutoff = std::cos(FOV() * 0.5f);
	}

	Float32 FOV() const {
		return m_FOV;
	}

	const glm::vec3&	Position() const {
		return *m_Position;
	}

	void		Position(const glm::vec3& pt) {
		*m_Position = glm::vec4(pt, 1.0f);
	}

	const glm::vec3&	Direction() const {
		return *m_Direction;
	}

	virtual const IRenderTarget*	LinkTo(const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	virtual const IRenderTarget*	LinkShadowMapTo(UInt32 location, const ShaderProgram_GLSL& program, const Neo::Bounds& bounds, const ICamera& camera) const override;
	void							DebugRender(ShaderProgram_GLSL& program, const glm::mat4& transform);

	//AOB: Then remove ToMat4x4 and just use Transform getter
	virtual const glm::mat4&		AsCameraTransform() const;
};