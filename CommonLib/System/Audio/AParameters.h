#pragma once

#include "System\Typedefs.h"
#include "System/Reflector.h"

template<typename _TParameters>
struct AParameters {
	CLASS_TYPEDEFS(AParameters)

public:
	typedef _TParameters	TParameters;

public:
	static Bool	UpdateFrom(const rapidjson::Value& value, TParameters& outParams) {
		assert(value.IsObject());

		FOREACH_MEMBER(iter, value) {
			auto reflector = Singleton<Reflector<TParameters>>::GetInstance();
			reflector->Set(iter->name.GetString(), outParams, iter->value);//This is expected to fail when it attempts to parse "Type"
		}
		return true;
	}
};