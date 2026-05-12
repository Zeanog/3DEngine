#pragma once

#include "System/Typedefs.h"
#include "System/List.h"
#include "System/JsonValueParsers.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>

class SceneLoader {
	CLASS_TYPEDEFS(SceneLoader)

	DEFINE_MEMBER_EX(glm::vec3, CameraPosition)
	DEFINE_MEMBER_EX(glm::quat, CameraRotation)
	DEFINE_MEMBER_EX(List<class Model*>, Models)

protected:
	void	ParseModels(const rapidjson::Value& value);
	void	ParseAudioChannels(const rapidjson::Value& value);
	void	ParseOrientation(const rapidjson::Value& value, glm::vec3& outPos, glm::vec3& outRot);

public:
	virtual Bool	Load(const Char* fileName);
	virtual	void	Clear();
};