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
	const Shader_Vertex_GLSL*		m_pVertexShader{}; // Vertex shader handle
	const Shader_Fragment_GLSL*		m_pFragmentShader{}; // Fragment shader handle
	GLhandleARB						m_Handle = -1; // Shader handle

	DEFINE_MEMBER_EX(Bool, IsInUse)

public:
	ShaderProgram_GLSL();

	virtual ~ShaderProgram_GLSL() {
	}

	Bool	IsValid() const {
		return m_pVertexShader && m_pFragmentShader && m_Handle != -1;
	}

	Bool	Create(const StaticString& vsPath, const StaticString& fsPath, const Char* header);

	Bool	LinkUniform(const Char* name, Float32 val) const;
	Bool	LinkUniform( const Char* name, const Float32* mat ) const;
	Bool	LinkUniform( const Char* name, const glm::mat3& mat ) const;
	Bool	LinkUniform( const Char* name, const glm::mat4& mat ) const;
	Bool	LinkUniform(const Char* name, const glm::vec3& vec) const;
	Bool	LinkUniform(const Char* name, const glm::vec4& vec) const;
	Bool	LinkUniform( const Char* name, Int32 val ) const;

	template< class TMetaContainer >
	Bool	LinkUniform( const Char* name, const Float32* vec, UInt32 size ) const;

	template<>
	Bool	LinkUniform<Float32>( const Char* name, const Float32* list, UInt32 count ) const {
		assert( m_Handle );

		Int32 loc = glGetUniformLocation( m_Handle, name );
		glUniform1fv( loc, count, list );
		Int32 errorCode = glGetError();
		if( errorCode ) {
			const Char* errorMsg = glErrorString( errorCode );
			return false;
		}
		return true;
	}

	void	StartUsing() {
		glUseProgramObjectARB( m_Handle );
		int error = glGetError();
		if (error) {
			const Char* str = glErrorString(error);
			int ix = 0;
		}
		IsInUse(true);
	}

	void	StopUsing() {
		glUseProgramObjectARB( 0 );
		assert(!glGetError());
		IsInUse(false);
	}

	operator GLhandleARB() {
		return m_Handle;
	}

	template<typename... TGLenums>
	void	EnumerateUniforms(List<StaticString>& outUniforms, TGLenums... enums) const {
		static constexpr UInt32 NumEnums = sizeof...(enums);

		Int32 maxNameLength = -1;
		glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
		
		Char*	nameBuffer = STACK_ALLOC(Char, maxNameLength);
		Int32	nameLength = -1;
		Int32	size = -1;
		GLenum	type;

		Int32 numUniforms = -1;
		glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &numUniforms);

		for (auto i = 0; i < numUniforms; i++) {
			glGetActiveUniform(m_Handle, (GLuint)i, maxNameLength, &nameLength, &size, &type, nameBuffer);
			if (!nameLength) {
				continue;
			}

			if (NumEnums > 0) {//TODO: If no enums are provided then return all uniforms
				for (auto typeToCheck : { enums... }) {
					if (type == typeToCheck) {
						outUniforms.Add(StaticString(nameBuffer));
					}
				}
			}
			else {
				outUniforms.Add(StaticString(nameBuffer));
			}
		}
	}
};