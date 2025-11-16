#pragma once

#include "System/Typedefs.h"

#include "System/Singleton.h"
#include "System/Reflector.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>

template<>
class Reflector<glm::quat> : public AReflector {
	INHERITEDCLASS_TYPEDEFS(Reflector, AReflector)
	SINGLETON_DECLARATIONS(TSelf) {
		REGISTER_MEMBER(TReflected, x);
		REGISTER_MEMBER(TReflected, y);
		REGISTER_MEMBER(TReflected, z);
		REGISTER_MEMBER(TReflected, w);
	}

public:
	typedef glm::quat	TReflected;
};

template<>
class ValueParser<glm::quat> : public AValueParser<glm::quat> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		FOREACH_MEMBER(memberIter, value) {
			StaticString memberName(memberIter->name.GetString());
			Singleton<Reflector<TValue>>::GetInstance()->Set(memberName, outValue, memberIter->value);
		}
	}
};

template<>
class Reflector<glm::vec3> : public AReflector {
	INHERITEDCLASS_TYPEDEFS(Reflector, AReflector)
	SINGLETON_DECLARATIONS(TSelf) {
		REGISTER_MEMBER(TReflected, x);
		REGISTER_MEMBER(TReflected, y);
		REGISTER_MEMBER(TReflected, z);
	}

public:
	typedef glm::vec3	TReflected;
};

template<>
class ValueParser<glm::vec3> : public AValueParser<glm::vec3> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		FOREACH_MEMBER(memberIter, value) {
			StaticString memberName(memberIter->name.GetString());
			Singleton<Reflector<TValue>>::GetInstance()->Set(memberName, outValue, memberIter->value);
		}
	}
};

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