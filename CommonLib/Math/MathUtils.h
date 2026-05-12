#pragma once

#include "System/Typedefs.h"

#include <string>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace glm {
	template<typename T>
	inline T	up();

	template<>
	inline vec3	up() {
		return vec3(0.0f, 1.0f, 0.0f);
	}

	template<typename T>
	inline T	forward();

	template<>
	inline vec3	forward() {
		return vec3(0.0f, 0.0f, 1.0f);
	}

	template<>
	inline vec4	forward() {
		return vec4(0.0f, 0.0f, 1.0f, 0.0f);
	}

	template<typename T>
	inline T	right();

	template<>
	inline vec3	right() {
		return vec3(1.0f, 0.0f, 0.0f);
	}
};

template<typename TValue, typename TIndex>
constexpr TValue SetBit(TIndex bitIndex) {
	return (TValue)(1 << bitIndex);
}

template<typename TValue, typename TIndex>
constexpr Bool BitIsSet(TValue val, TIndex bitIndex) {
	return ((val >> bitIndex) & 0x1) != 0;
}

template<typename TValue>
constexpr Bool BitsAreSet(TValue val, TValue bits) {
	return (val & bits) == bits;
}

class MathUtils {
public:
	static Float32		PI;
	static Float32		Epsilon;

public:
	static glm::mat4x4	CreateAxisAlong_Local( const glm::vec3& forward, const glm::vec3& localPos, const glm::vec3& up );
	static glm::mat4x4	CreateAxisAlong_World(const glm::vec3& forward, const glm::vec3& wordlPos, const glm::vec3& up);
	static glm::mat4x4	CreateAxisAlong( const glm::vec3& eye, const glm::vec3& focalPt, const glm::vec3& up );
	static glm::mat4x4	CreateAxisAlong( const glm::vec3& forward, const glm::vec3& up );
	
	static Float32		Sqrt( Float32 val );

	static constexpr Float32	MilliSec2Sec(UInt64 millsec) {
		return millsec / 1000.0f;
	}

	static constexpr Float32	MilliSec2Sec(UInt32 millsec) {
		return millsec / 1000.0f;
	}

	static constexpr Float32	MilliSec2Sec(Int32 millsec) {
		return millsec / 1000.0f;
	}

	static constexpr Int32	Sec2MilliSec(Float32 sec) {
		return (Int32)(sec * 1000.0f);
	}

	static constexpr Float32			Radians2Deg(Float32 radians) {
		return radians * (180.0f / PI);
	}

	static constexpr Float32			Deg2Radians(Float32 degrees) {
		return degrees * (PI / 180.0f);
	}

	template<typename _TValue>
	static constexpr _TValue	Min(_TValue lhs, _TValue rhs) {
		return lhs < rhs ? lhs : rhs;
	}

	template<typename _TValue>
	static constexpr _TValue	Max(_TValue lhs, _TValue rhs) {
		return lhs > rhs ? lhs : rhs;
	}

	template<typename _TValue>
	static constexpr _TValue	Abs(_TValue val ) {
		return val < 0 ? -val : val;
	}

	static constexpr Bool		IsPowerOfTwo(UInt32 val) {
		return val && (val & (val - 1)) == 0;
	}

	static constexpr Bool		IsPowerOfTwo(Int32 val) {
		return val && (val & (val - 1)) == 0;
	}

	static UInt32	NearestPowerOfTwo(UInt32 val);

	static char		ToChar(Int32 num);
	static UInt32	ToNumber(Char ch);
	static UInt32	NumDigits(Int32 val, Int32 base);
	static void		ConvertTo(Int32 base, Int32 inputNum, std::string& outNum);
	static void		Convert(const Char* pNumber, Int32 srcBase, Int32 dstBase, std::string& outConvertedNum);
	static char*	Convert(const Char* pNumber, Int32 srcBase, Int32 dstBase);
};