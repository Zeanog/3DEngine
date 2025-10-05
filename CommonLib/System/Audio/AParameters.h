#pragma once

#include "System\Typedefs.h"
#include "System/Reflector.h"

template<typename TParameters>
class AParameters : public TParameters  {
	INHERITEDCLASS_TYPEDEFS( AParameters, TParameters )

public:
	virtual void	SetToDefault() = 0;

	constexpr UInt64	Sizeof() const {
		return sizeof(TSuper);
	}

	virtual Bool	UpdateFrom(const rapidjson::Value& reverbVal) {
		if (!reverbVal.IsObject()) {
			assert(0);
			return false;
		}

		FOREACH_MEMBER(iter, reverbVal) {
			auto memberName = iter->name.GetString();
			verify(Singleton<Reflector<TParameters>>::GetInstance()->Set(memberName, this, iter->value));
		}
		return true;
	}
};