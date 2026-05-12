#pragma once

#include "System/Typedefs.h"
#include "System/Map.h"
#include "System/List.h"
#include "System/StaticString.h"

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

class ShaderProgram_GLSL;

class IRenderTarget {
	CLASS_TYPEDEFS(IRenderTarget)

protected:
	GLuint			m_hTarget; // The diffuse render target

public:
	virtual void			Bind() const = 0;
	virtual void			Unbind() const = 0;
	virtual void			LinkTo(GLenum textureUnitIndex) const = 0;
	virtual void			Unlink() const = 0;
	virtual Bool			IsLinked() const = 0;
};

class RenderTarget : public IRenderTarget {
	INHERITED_CLASS_TYPEDEFS(RenderTarget, IRenderTarget)

public:
	RenderTarget() {
		m_hTarget = 0;
	}

	virtual ~RenderTarget() {
		if (m_hTarget) {
			glDeleteRenderbuffersEXT(1, &m_hTarget);
			m_hTarget = 0;
		}
	}

	GLuint			hTarget() const {
		return m_hTarget;
	}

	void			hTarget( GLuint h ) {
		m_hTarget = h;
	}

	virtual Bool			Create(int width, int height, GLuint attachmentId, GLuint storageType) {
		if (!m_hTarget) {
			glGenRenderbuffersEXT(1, &m_hTarget);
			//assert(!glGetError());
			int errorCode = glGetError();
			if (errorCode) {
				const GLubyte* error = gluErrorString(errorCode);
				return false;
			}
		}

		Bind();

		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, storageType, width, height);
		assert(!glGetError());

		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachmentId, GL_RENDERBUFFER_EXT, m_hTarget);
		assert(!glGetError());

		Unbind();

		return true;
	}

	virtual void			Bind() const override {
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_hTarget);
		assert(!glGetError());
	}

	virtual void			Unbind() const override {
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		assert(!glGetError());
	}

	virtual void			LinkTo(GLenum textureUnitIndex) const override {
	}

	virtual void			Unlink() const override {
	}

	virtual Bool			IsLinked() const override {
		return false;
	}
};

class RenderTexture : public IRenderTarget {
	INHERITED_CLASS_TYPEDEFS(RenderTexture, IRenderTarget)

protected:
	mutable GLenum	m_LinkedTextureUnit;

public:
	RenderTexture() {
		m_LinkedTextureUnit = -1;
	}

	virtual ~RenderTexture() {
		if (m_hTarget) {
			glDeleteTextures(1, &m_hTarget);
			m_hTarget = 0;
		}
	}

	virtual Bool			Create(int width, int height, GLuint attachmentId, GLuint storageType, GLenum format, GLenum type) {
		glGenTextures(1, &m_hTarget);
		assert(!glGetError());

		Bind();

		glTexImage2D(GL_TEXTURE_2D, 0, storageType, width, height, 0, format, type, NULL);
		assert(!glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachmentId, GL_TEXTURE_2D, m_hTarget, 0);
		assert(!glGetError());

		Unbind();

		return true;
	}

	virtual void			Bind() const override {
		glBindTexture(GL_TEXTURE_2D, m_hTarget);
		assert(!glGetError());
	}

	virtual void			Unbind() const override {
		glBindTexture(GL_TEXTURE_2D, 0);
		assert(!glGetError());
	}

	virtual void			LinkTo(GLenum textureUnitIndex) const override;
	virtual void			Unlink() const override;
	virtual Bool			IsLinked() const override {
		return m_LinkedTextureUnit >= 0;
	}
};

class FrameBufferObject {
	CLASS_TYPEDEFS(FrameBufferObject)

public:
	// Ctors/Dtors
	FrameBufferObject(int width, int height);
	virtual ~FrameBufferObject();

	// Methods
	virtual void	SetAsTarget();
	virtual void	UnsetAsTarget();
	void	showTexture(const StaticString& texName, float fSizeX, float fSizeY, float x, float y) const;

	UInt32	Width() const {
		return m_Width;
	}

	UInt32	Height() const {
		return m_Height;
	}

	IRenderTarget*	operator[](const StaticString& key) {
		GLuint attachmentId = m_KeyToAttachmentMap[key];
		return m_AttachmentTargetMap[attachmentId];
	}

	const IRenderTarget*	operator[](const StaticString& key) const {
		GLuint attachmentId = m_KeyToAttachmentMap[key];
		return m_AttachmentTargetMap[attachmentId];
	}

	Bool		BindTexture(const StaticString& key) const {
		const IRenderTarget* rt = (*this)[key];
		if (!rt) {
			return false;
		}
		rt->Bind();
		return true;
	}

	Bool		UnbindTexture(const StaticString& key) const {
		const IRenderTarget* rt = (*this)[key];
		if (!rt) {
			return false;
		}
		rt->Unbind();
		return true;
	}

	Bool		AddTarget(const StaticString& key, GLuint attachmentId, GLuint storageType, GLenum format, GLenum type) {
		RenderTexture* rt = new RenderTexture();
		rt->Create(m_Width, m_Height, attachmentId, storageType, format, type);

		m_KeyToAttachmentMap.Add(key, attachmentId);
		m_TextureAttachments.Add(attachmentId);
		m_AttachmentTargetMap.Add(attachmentId, rt);

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		return status == GL_FRAMEBUFFER_COMPLETE_EXT;
	}

	Bool		AddTarget(const StaticString& key, GLuint attachmentId, GLuint storageType) {
		RenderTarget* rt = new RenderTarget();
		rt->Create(m_Width, m_Height, attachmentId, storageType);

		m_KeyToAttachmentMap.Add(key, attachmentId);
		m_AttachmentTargetMap.Add(attachmentId, rt);

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		return status == GL_FRAMEBUFFER_COMPLETE_EXT;
	}

	const IRenderTarget*			LinkTargetTo(const StaticString& key, const ShaderProgram_GLSL& program, GLenum textureUnitIndex) const;

	void		Bind() const {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_hFBO);
	}

	void		Unbind() const {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

protected:
	Map<StaticString, GLuint>		m_KeyToAttachmentMap;
	List<GLuint>					m_TextureAttachments;
	Map<GLuint, IRenderTarget*>		m_AttachmentTargetMap;

	// Variables
	GLuint			m_hFBO;

	unsigned int	m_Width;
	unsigned int	m_Height;
};

class FrameBufferObject_Depth : public FrameBufferObject {
	INHERITED_CLASS_TYPEDEFS(FrameBufferObject_Depth, FrameBufferObject)

public:
	FrameBufferObject_Depth(int width, int height) : TSuper(width, height) {}

	virtual void	SetAsTarget() override;
};
