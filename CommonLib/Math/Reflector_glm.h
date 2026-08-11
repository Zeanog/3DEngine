#pragma once

#include "System/Reflection/Reflector.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>

template<>
class Reflector<glm::quat> : public AReflectorJson {
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {
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
	INHERITED_CLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(ValueParser) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		FOREACH_MEMBER(memberIter, value) {
			Singleton<Reflector<TValue>>::GetInstance()->Set(memberIter->name.GetString(), outValue, memberIter->value);
		}
	}

	virtual void Set(const TValue& value, rapidjson::Value& outValue) const override {
		outValue.SetObject();
		Singleton<ValueParser<decltype(value.x)>>::GetInstance()->Set(value.x, outValue["x"]);
		Singleton<ValueParser<decltype(value.y)>>::GetInstance()->Set(value.y, outValue["y"]);
		Singleton<ValueParser<decltype(value.z)>>::GetInstance()->Set(value.z, outValue["z"]);
		Singleton<ValueParser<decltype(value.w)>>::GetInstance()->Set(value.w, outValue["w"]);
	}
};

//////////////////////////////////////////////////////////////////////////////////

template<>
class Reflector<glm::vec3> : public AReflectorJson {
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {
		REGISTER_MEMBER(TReflected, x);
		REGISTER_MEMBER(TReflected, y);
		REGISTER_MEMBER(TReflected, z);
	}

public:
	typedef glm::vec3	TReflected;
};

template<>
class ValueParser<glm::vec3> : public AValueParser<glm::vec3> {
	INHERITED_CLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(ValueParser) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		FOREACH_MEMBER(memberIter, value) {
			Singleton<Reflector<TValue>>::GetInstance()->Set(memberIter->name.GetString(), outValue, memberIter->value);
		}
	}

	virtual void Set(const TValue& value, rapidjson::Value& outValue) const override {
		outValue.SetObject();
		Singleton<ValueParser<decltype(value.x)>>::GetInstance()->Set(value.x, outValue["x"]);
		Singleton<ValueParser<decltype(value.y)>>::GetInstance()->Set(value.y, outValue["y"]);
		Singleton<ValueParser<decltype(value.z)>>::GetInstance()->Set(value.z, outValue["z"]);
	}
};

//////////////////////////////////////////////////////////////////////////////////