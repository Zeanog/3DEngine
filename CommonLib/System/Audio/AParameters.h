#pragma once

#include "System\Typedefs.h"
#include "System/Reflector.h"

template<typename TParameters>
struct AParameters {
	CLASS_TYPEDEFS(AParameters)

public:
	static Bool	UpdateFrom(TParameters* params, const rapidjson::Value& value) {
		if (!value.IsObject()) {
			assert(0);
			return false;
		}

		FOREACH_MEMBER(iter, value) {
			auto memberName = iter->name.GetString();
			verify(Singleton<Reflector<TParameters>>::GetInstance()->Set(memberName, params, iter->value));
		}
		return true;
	}
};