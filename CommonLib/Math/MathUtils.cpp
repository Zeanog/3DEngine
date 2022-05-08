#include "Math/MathUtils.h"

#include <math.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

Float32 MathUtils::PI = 3.145926f;
Float32 MathUtils::Epsilon = 0.001f;

glm::mat4x4 MathUtils::CreateAxisAlong_Local( const glm::vec3& worldForward, const glm::vec3& localPos, const glm::vec3& up ) {
	glm::mat4x4 mat( CreateAxisAlong(worldForward, up) );
	mat = glm::translate(mat, localPos);
	return mat;
}

glm::mat4x4 MathUtils::CreateAxisAlong_World(const glm::vec3& worldForward, const glm::vec3& worldPos, const glm::vec3& up) {
	glm::mat4x4 mat(CreateAxisAlong(worldForward, up));
	mat[3] = glm::vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	return mat;
}

glm::mat4x4 MathUtils::CreateAxisAlong( const glm::vec3& forward, const glm::vec3& up ) {
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
	//return glm::lookAt(glm::zero<glm::vec3>(), forward * 10.0f, up);
}

glm::mat4x4 MathUtils::CreateAxisAlong(const glm::vec3& eye, const glm::vec3& focalPt, const glm::vec3& up) {
	glm::vec3 dir = glm::normalize(focalPt - eye);
	glm::vec3 dirXup = glm::cross(dir, up);
	if (glm::length(dirXup) > MathUtils::Epsilon) {
		return glm::lookAt(eye, focalPt, up);
	}

	return glm::lookAt(eye, focalPt, up);
}

Float32	MathUtils::Sqrt( Float32 val ) {
	return sqrtf( val );
}