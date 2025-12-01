#include "ShaderProgram_GLSL.h"
#include "ShaderManager_Vertex.h"
#include "ShaderManager_Fragment.h"
#include "ShaderProgramManager.h"

#include <gl/gl.h>
#include <gl/glu.h>

#include <glm/gtc/type_ptr.hpp>

ShaderProgram_GLSL::ShaderProgram_GLSL() {
}

Bool	ShaderProgram_GLSL::Create(const StaticString& vsPath, const StaticString& fsPath, const Char* header) {
	m_Handle = glCreateProgramObjectARB();
	assert(!glGetError());

	m_pVertexShader = Singleton<ShaderManager_Vertex>::GetInstance()->Get(vsPath, header);
	if (!m_pVertexShader) {
		return false;
	}

	m_pFragmentShader = Singleton<ShaderManager_Fragment>::GetInstance()->Get(fsPath, header);
	if (!m_pFragmentShader) {
		return false;
	}

	glAttachObjectARB(m_Handle, m_pVertexShader->GetHandle());
	assert(!glGetError());

	glAttachObjectARB(m_Handle, m_pFragmentShader->GetHandle());
	assert(!glGetError());

	glLinkProgramARB(m_Handle);
	assert(!glGetError());

	verify(EnumerateUniforms(m_RequiredChannels, 1U, GL_SAMPLER_2D));

	return true;
}

Bool ShaderProgram_GLSL::LinkUniform(const Char* name, Float32 val) const {
	assert(m_Handle);

	Int32 loc = glGetUniformLocation(m_Handle, name);
	glUniform1f(loc, val);
	Int32 errorCode = glGetError();
	if (errorCode) {
		const Char* errorMsg = glErrorString(errorCode);
		return false;
	}
	return true;
}

Bool ShaderProgram_GLSL::LinkUniform( const Char* name, const Float32* mat ) const {
	assert( m_Handle );

	Int32 loc = glGetUniformLocation( m_Handle, name );
	glUniformMatrix4fvARB( loc, 1, false, mat );
	Int32 errorCode = glGetError();
	if( errorCode ) {
		const Char* errorMsg = glErrorString( errorCode );
		return false;
	}
	return true;
}

Bool ShaderProgram_GLSL::LinkUniform( const Char* name, const glm::mat3& mat ) const {
	assert( m_Handle );

	Int32 loc = glGetUniformLocation( m_Handle, name );
	glUniformMatrix3fvARB( loc, 1, false, glm::value_ptr(mat));
	Int32 errorCode = glGetError();
	if( errorCode ) {
		const Char* errorMsg = glErrorString( errorCode );
		return false;
	}
	return true;
}

Bool ShaderProgram_GLSL::LinkUniform( const Char* name, const glm::mat4& mat ) const {
	assert( m_Handle );

	Int32 loc = glGetUniformLocation( m_Handle, name );
	glUniformMatrix4fvARB( loc, 1, false, glm::value_ptr(mat));
	Int32 errorCode = glGetError();
	if( errorCode ) {
		const Char* errorMsg = glErrorString( errorCode );
		return false;
	}
	return true;
}

Bool ShaderProgram_GLSL::LinkUniform(const Char* name, const glm::vec3& vec) const {
	assert(m_Handle);

	Int32 loc = glGetUniformLocation(m_Handle, name);
	glUniform3f(loc, vec[0], vec[1], vec[2]);
	Int32 errorCode = glGetError();
	if (errorCode) {
		const Char* errorMsg = glErrorString(errorCode);
		return false;
	}
	return true;
}

Bool ShaderProgram_GLSL::LinkUniform(const Char* name, const glm::vec4& vec) const {
	assert(m_Handle);

	Int32 loc = glGetUniformLocation(m_Handle, name);
	glUniform4f(loc, vec[0], vec[1], vec[2], vec[3]);
	Int32 errorCode = glGetError();
	if (errorCode) {
		const Char* errorMsg = glErrorString(errorCode);
		return false;
	}
	return true;
}

Bool ShaderProgram_GLSL::LinkUniform( const Char* name, Int32 val ) const {
	assert( m_Handle );

	Int32 loc = glGetUniformLocation( m_Handle, name );
	Int32 errorCode = glGetError();
	if (errorCode) {
		const Char* errorMsg = glErrorString(errorCode);
		return false;
	}
	glUniform1iARB( loc, val );
	errorCode = glGetError();
	if( errorCode ) {
		const Char* errorMsg = glErrorString( errorCode );
		return false;
	}
	return true;
}