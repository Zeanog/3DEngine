#pragma once

#include "System/Typedefs.h"
#include "System/File.h"
#include "System/List.h"

#include <GL/glew.h>

class Shader_GLSL {
	CLASS_TYPEDEFS(Shader_GLSL);

public:

protected:
	GLhandleARB	m_Handle;

	const StaticString	m_Version;

	List<const Char*>	m_ShaderChunks;

public:
	virtual Bool	ReadFrom( const StaticString& path ) = 0;
	virtual Bool	ReadFrom(const StaticString& path, const Char* header ) = 0;

	operator GLhandleARB() {
		return m_Handle;
	}

	GLhandleARB	GetHandle() const {
		return m_Handle;
	}

protected:
	Shader_GLSL( const StaticString& version ) : m_Version(version) {
		m_Handle = 0;
	}

	void GetShaderInfoLog(StaticString& msg);
};

class Shader_Vertex_GLSL : public Shader_GLSL {
	INHERITEDCLASS_TYPEDEFS(Shader_Vertex_GLSL, Shader_GLSL);

public:

protected:

public:
	Shader_Vertex_GLSL(const StaticString& version) : TSuper(version) {
	}

	virtual Bool	ReadFrom(const StaticString& path) override;
	virtual Bool	ReadFrom(const StaticString& path, const Char* header) override;
};

class Shader_Fragment_GLSL : public Shader_GLSL {
	INHERITEDCLASS_TYPEDEFS(Shader_Fragment_GLSL, Shader_GLSL);

public:

protected:

public:
	Shader_Fragment_GLSL(const StaticString& version) : TSuper(version) {
	}

	virtual Bool	ReadFrom( const StaticString& path ) override;
	virtual Bool	ReadFrom(const StaticString& path, const Char* header) override;
};