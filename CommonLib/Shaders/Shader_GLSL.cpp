#include "Shader_GLSL.h"

//#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <windows.h>

void Shader_GLSL:: GetShaderInfoLog(StaticString& msg) {
	GLint length;

	if (!m_Handle) {
		return;
	}

	glGetShaderiv(m_Handle, GL_INFO_LOG_LENGTH, &length);
	assert(!glGetError());

	if (length > 0) {
		GLchar* log = STACK_ALLOC(GLchar, length);
		glGetShaderInfoLog(m_Handle, 200, &length, log);
		msg = log;
	}
}

Bool	Shader_Vertex_GLSL::ReadFrom(const StaticString& path) {
	return ReadFrom(path, NULL);
};

Bool Shader_Vertex_GLSL::ReadFrom(const StaticString& path, const Char* header) {
	if (!m_Handle) {
		m_Handle = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		assert(!glGetError());
	}

	File file;

	if (!file.Open(path, "rb")) {
		return false;
	}

	Char* data = STACK_ALLOC(Char, file.Length() + 1);
	memset(data, 0, file.Length() + 1);

	if (!file.Read(data, file.Length())) {
		return false;
	}

	m_ShaderChunks.Clear();

	m_ShaderChunks.Add(m_Version.CStr());
	if(header && header[0]) {
		m_ShaderChunks.Add(header);
	}
	m_ShaderChunks.Add(data);

	glShaderSourceARB(m_Handle, m_ShaderChunks.Length(), &m_ShaderChunks[0], NULL);
	assert(!glGetError());

	glCompileShaderARB(m_Handle);
	assert(!glGetError());

	GLint compiled = false;
	glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &compiled);
	assert(!glGetError());

	if (!compiled) {
		StaticString log;
		GetShaderInfoLog(log);

		OutputDebugString(log.CStr());
	}

	return !!compiled;
}

Bool	Shader_Fragment_GLSL::ReadFrom(const StaticString& path) {
	return ReadFrom(path, NULL);
};

Bool Shader_Fragment_GLSL::ReadFrom(const StaticString& path, const Char* header) {
	if (!m_Handle) {
		m_Handle = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		assert(!glGetError());
	}

	File file;

	if (!file.Open(path, "rb")) {
		return false;
	}

	Char* data = STACK_ALLOC(Char, file.Length() + 1);
	memset(data, 0, file.Length() + 1);

	if (!file.Read(data, file.Length())) {
		return false;
	}

	m_ShaderChunks.Clear();

	m_ShaderChunks.Add(m_Version.CStr());
	if (header && header[0]) {
		m_ShaderChunks.Add(header);
	}
	m_ShaderChunks.Add(data);

	glShaderSourceARB(m_Handle, m_ShaderChunks.Length(), &m_ShaderChunks[0], NULL);
	assert(!glGetError());

	glCompileShaderARB(m_Handle);
	assert(!glGetError());

	GLint compiled = false;
	glGetObjectParameterivARB(m_Handle, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
	assert(!glGetError());

	if (!compiled) {
		StaticString log;
		GetShaderInfoLog(log);

		OutputDebugString(log.CStr());
	}

	return !!compiled;
}