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
			return false;
		}

		for (auto iter = reverbVal.MemberBegin(), endIter = reverbVal.MemberEnd(); iter != endIter; ++iter) {
			auto memberName = iter->name.GetString();
			verify(Singleton<Reflector<TParameters>>::GetInstance()->Set(memberName, this, iter->value));
		}
		return true;
	}
};