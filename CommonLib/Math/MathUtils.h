#pragma once

#include "System/Typedefs.h"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace glm {
	template<typename T>
	inline T	up() {
	}

	template<>
	inline vec3	up() {
		return vec3(0.0f, 1.0f, 0.0f);
	}

	template<typename T>
	inline T	forward() {
	}

	template<>
	inline vec3	forward() {
		return vec3(0.0f, 0.0f, 1.0f);
	}

	template<typename T>
	inline T	right() {
	}

	template<>
	inline vec3	right() {
		return vec3(1.0f, 0.0f, 0.0f);
	}
};

class MathUtils {
public:
	static Float32		PI;

public:
	static glm::mat4x4	CreateAxisAlong_Local( const glm::vec3& forward, const glm::vec3& localPos, const glm::vec3& up );
	static glm::mat4x4	CreateAxisAlong_World(const glm::vec3& forward, const glm::vec3& wordlPos, const glm::vec3& up);
	static glm::mat4x4	CreateAxisAlong( const glm::vec3& forward, const glm::vec3& up );

	static Float32	Sqrt( Float32 val );

	static Float32	MilliSec2Sec(Int32 millsec) {
		return millsec / 1000.0f;
	}

	static Int32	Sec2MilliSec(Float32 sec) {
		return (Int32)(sec * 1000.0f);
	}

	static Float32			Radians2Deg(Float32 radians) {
		return radians * (180.0f / PI);
	}

	static Float32			Deg2Radians(Float32 degrees) {
		return degrees * (PI / 180.0f);
	}

	template<typename _TValue>
	static _TValue	Min(_TValue lhs, _TValue rhs) {
		return lhs < rhs ? lhs : rhs;
	}

	template<typename _TValue>
	static _TValue	Max(_TValue lhs, _TValue rhs) {
		return lhs > rhs ? lhs : rhs;
	}

	template<typename _TValue>
	static _TValue	Abs(_TValue val ) {
		return val < 0 ? -val : val;
	}
};