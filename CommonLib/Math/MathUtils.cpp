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

UInt32 MathUtils::NearestPowerOfTwo(UInt32 val) {
	Float32 pos = ceilf((Float32)log2(val));//  (ceiling of log n with base 2)
	return (UInt32)powf(2, pos);
}

Int32 MathUtils::NearestPowerOfTwo(Int32 val) {
	Float32 pos = ceilf((Float32)log2(val));//  (ceiling of log n with base 2)
	return (Int32)powf(2, pos);
}

Char	MathUtils::ToChar(int num) {
	if (num <= 9) {
		return (Char)(num + '0');
	}
	else {
		return (Char)((num - 10) + 'A');
	}
}

Int32     MathUtils::ToNumber(Char ch) {
	if (ch <= '9') {
		return ch - '0';
	} else if(ch >= 'a' && ch <= 'z') {
		return (ch - 'a') + 10;
	}

	return (ch - 'A') + 10;
}

Int32		MathUtils::NumDigits(Int32 val, Int32 base) {
	return (int)(log2(val) / log2(base)) + 1;
}

void	MathUtils::ConvertTo(Int32 dstBase, Int32 inputNum, std::string& outDstNum) {
	int numDigits = NumDigits(inputNum, dstBase);

	outDstNum.resize(numDigits);
	for (int ix = numDigits - 1; ix >= 0; --ix) {
		outDstNum[ix] = ToChar(inputNum % dstBase);
		inputNum /= dstBase;
	}
}

void MathUtils::Convert(const Char* str, Int32 srcBase, Int32 dstBase, std::string& outNum) {
	if (!str || !str[0]) {
		outNum = "";
		return;
	}

	int val = 0;
	int index = 0;
	while (true) {
		char ch = str[index++];
		if (ch == '\0') {//Null terminator
			break;
		}

		val *= srcBase;
		val += ToNumber(ch);
	}

	ConvertTo(dstBase, val, outNum);
}

Char* MathUtils::Convert(const Char* pNumber, Int32 srcBase, Int32 dstBase) {
	static constexpr Int32 numBuffers = 5;
	static constexpr Int32 bufferLength = 256;
	static Char	buffers[numBuffers][bufferLength];
	static Int32	currentBufferIndex = -1;

	currentBufferIndex = (currentBufferIndex + 1) % numBuffers;
	Char* buffer = buffers[currentBufferIndex];

	static std::string convertedNum;//This makes this non thread-safe
	Convert(pNumber, srcBase, dstBase, convertedNum);
	auto error = strcpy_s(buffer, bufferLength, convertedNum.c_str());
	return buffer;
}
