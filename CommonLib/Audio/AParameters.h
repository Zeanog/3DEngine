#pragma once

#include "System\Typedefs.h"
#include "System/Reflection/Reflector.h"

template<typename _TParameters>
struct AParameters {
	CLASS_TYPEDEFS(AParameters)

public:
	typedef _TParameters	TParameters;

public:
	static Bool	UpdateFrom(const rapidjson::Value& value, TParameters& outParams) {
		assert(value.IsObject());

		auto reflector = Singleton<Reflector<TParameters>>::GetInstance();
		FOREACH_MEMBER(iter, value) {
			//if (!reflector->HasMember(iter->name.GetString())) {//Ignore "Type"
			//	continue;
			//}
			auto memberName = iter->name.GetString();
			reflector->Set(memberName, outParams, iter->value);
		}
		return true;
	}
};