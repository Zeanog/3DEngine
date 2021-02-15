#include "pch.h"

#include "System\MathUtils.h"

#include <math.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

Float32 MathUtils::PI = 3.145926f;

glm::mat4x4 MathUtils::CreateAxisAlong_Local( const glm::vec3& worldForward, const glm::vec3& localPos, const glm::vec3& up ) {
	glm::mat4x4 mat( CreateAxisAlong(worldForward, up) );
	return glm::translate(mat, localPos);
}

glm::mat4x4 MathUtils::CreateAxisAlong_World(const glm::vec3& worldForward, const glm::vec3& worldPos, const glm::vec3& up) {
	glm::mat4x4 mat(CreateAxisAlong(worldForward, up));
	mat[3] = glm::vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	return mat;
}

glm::mat3x3 MathUtils::CreateAxisAlong( const glm::vec3& forward, const glm::vec3& up ) {
	glm::vec3 zDir = glm::normalize(forward);
	glm::vec3 xDir = glm::cross( up, zDir );
	float length = glm::length(xDir);
	if (length <= 0.001f) {
		glm::vec3 f = glm::forward<glm::vec3>();
		xDir = glm::cross(f, zDir);
	}
	xDir = glm::normalize(xDir);

	glm::vec3 yDir = glm::cross(zDir, xDir);
	yDir = glm::normalize(yDir);

	return glm::mat3x3( xDir, yDir, zDir );
}

Float32	MathUtils::Sqrt( Float32 val ) {
	return sqrtf( val );
}