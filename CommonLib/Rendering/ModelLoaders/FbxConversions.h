#pragma once

#include "System/Typedefs.h"

#include <glm/mat4x4.hpp>
#include <fbxsdk.h>

class FbxConversions {
public:
	static glm::mat4& Convert(glm::mat4& to, const FbxMatrix& from) {
		for (Int32 iy = 0; iy < 4; ++iy) {
			for (Int32 ix = 0; ix < 4; ++ix) {
				to[iy][ix] = (Float32)from.Get(iy, ix);
			}
		}
		return to;
	}

	static glm::mat4& Convert(glm::mat4& to, const fbxsdk::FbxAMatrix& from) {
		for (Int32 iy = 0; iy < 4; ++iy) {
			for (Int32 ix = 0; ix < 4; ++ix) {
				to[iy][ix] = (Float32)from.Get(iy, ix);
			}
		}
		return to;
	}

	static glm::vec2& Convert(glm::vec2& to, const FbxVector2& from) {
		for (int ix = 0; ix < 2; ++ix) {
			to[ix] = (Float32)from[ix];
		}
		return to;
	}

	static glm::vec3& Convert(glm::vec3& to, const FbxVector4& from) {
		for (int ix = 0; ix < 3; ++ix) {
			to[ix] = (Float32)from[ix];
		}
		return to;
	}

	glm::vec4& Convert(glm::vec4& to, const FbxVector4& from) {
		for (int ix = 0; ix < 4; ++ix) {
			to[ix] = (Float32)from[ix];
		}
		return to;
	}

	/*static Vector<4>& Convert(Vector<4>& to, const FbxVector4& from) {
		for (int ix = 0; ix < 4; ++ix) {
			to[ix] = (Float32)from[ix];
		}
		return to;
	}

	static Vector<3>& Convert(Vector<3>& to, const FbxVector4& from) {
		for (int ix = 0; ix < 3; ++ix) {
			to[ix] = (Float32)from[ix];
		}
		return to;
	}

	static Vector<2>& Convert(Vector<2>& to, const FbxVector2& from) {
		for (int ix = 0; ix < 2; ++ix) {
			to[ix] = (Float32)from[ix];
		}
		return to;
	}*/
};