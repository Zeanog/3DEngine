#pragma once

#include "System\Typedefs.h"
#include "System/Reflector.h"

template<typename _TParameters>
struct AParameters {
	CLASS_TYPEDEFS(AParameters)

public:
	typedef _TParameters	TParameters;

public:
	static Bool	UpdateFrom(TParameters& inOutParams, const rapidjson::Value& value) {
		assert(value.IsObject());

		FOREACH_MEMBER(iter, value) {
			auto memberName = iter->name.GetString();
			auto reflector = Singleton<Reflector<TParameters>>::GetInstance();
			reflector->Set(memberName, inOutParams, iter->value);//This is expected to fail when it attempts to parse "Type"
		}
		return true;
	}
};