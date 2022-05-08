#pragma once

#include "../System/StaticString.h"
#include "Shader_GLSL.h"

#include "../Rendering/ErrorHandling.h"

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class ShaderProgram_GLSL {
public:
	
protected:
	const Shader_Vertex_GLSL*		m_pVertexShader; // Vertex shader handle
	const Shader_Fragment_GLSL*		m_pFragmentShader; // Fragment shader handle
	GLhandleARB						m_Handle = -1; // Shader handle

public:
	ShaderProgram_GLSL();

	virtual ~ShaderProgram_GLSL() {
	}

	Bool	Create(const StaticString& vsPath, const StaticString& fsPath, const Char* header);

	Bool	LinkUniform(const StaticString& name, Float32 val) const;
	Bool	LinkUniform( const StaticString& name, const Float32* mat ) const;
	Bool	LinkUniform( const StaticString& name, const glm::mat3& mat ) const;
	Bool	LinkUniform( const StaticString& name, const glm::mat4& mat ) const;
	Bool	LinkUniform(const StaticString& name, const glm::vec3& vec) const;
	Bool	LinkUniform(const StaticString& name, const glm::vec4& vec) const;
	Bool	LinkUniform( const StaticString& name, Int32 val ) const;

	template< class TMetaContainer >
	Bool	LinkUniform( const StaticString& name, const Float32* vec, UInt32 size ) const;

	template<>
	Bool	LinkUniform<Float32>( const StaticString& name, const Float32* list, UInt32 count ) const {
		assert( m_Handle );

		Int32 loc = glGetUniformLocation( m_Handle, name.CStr() );
		glUniform1fv( loc, count, list );
		Int32 errorCode = glGetError();
		if( errorCode ) {
			const Char* errorMsg = glErrorString( errorCode );
			return false;
		}
		return true;
	}

	void	StartUsing() const {
		glUseProgramObjectARB( m_Handle );
		int error = glGetError();
		if (error) {
			const Char* str = glErrorString(error);
			int ix = 0;
		}
		//assert(!glGetError());
	}

	void	StopUsing() const {
		glUseProgramObjectARB( 0 );
		assert(!glGetError());
	}

	operator GLhandleARB() {
		return m_Handle;
	}

	void	EnumerateUniforms(List<StaticString>& outUniforms) {
		Int32 numUniforms = -1;
		glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &numUniforms);
		
		Char	nameBuffer[256];
		Int32	nameLength = -1;
		Int32	size = -1;
		GLenum	type;
		for (int i = 0; i < numUniforms; i++)
		{
			glGetActiveUniform(m_Handle, (GLuint)i, STATIC_ARRAY_LENGTH(nameBuffer), &nameLength, &size, &type, nameBuffer);
			if (!nameLength) {
				continue;
			}
			outUniforms.Add(StaticString(nameBuffer));
		}
	}
};