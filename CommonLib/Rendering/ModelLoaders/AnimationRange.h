#pragma once

#include "System/Typedefs.h"
#include "System/Singleton.h"
#include "System/Reflector.h"

struct AnimationRange {
	Int32			StartFrame = 0;
	Int32			EndFrame = 0;
};

template<>
class Reflector<AnimationRange> : public AReflectorJson {
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {
		REGISTER_MEMBER(TReflected, StartFrame);
		REGISTER_MEMBER(TReflected, EndFrame);
	}

public:
	typedef AnimationRange	TReflected;
};

template<>
class ValueParser<AnimationRange> : public AValueParser<AnimationRange> {
	INHERITED_CLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(ValueParser) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		try {
			auto encodedRange = value.GetString();
			auto index = String::FindIndexOf(encodedRange, ":");
			assert(index > -1);
			auto startLength = index;
			STACK_STRING(startFrameStr, startLength + 1);
			strncpy_s(startFrameStr.Str(), startLength + 1, encodedRange, startLength);
			auto endLength = String::Length(encodedRange) - (index + 1);
			STACK_STRING(endFrameStr, endLength + 1);
			strncpy_s(endFrameStr.Str(), endLength + 1, encodedRange + startLength + 1, endLength);

			Int32 startFrame = std::atoi(startFrameStr.CStr());
			Int32 endFrame = std::atoi(endFrameStr.CStr());

			outValue.StartFrame = startFrame;
			outValue.EndFrame = endFrame;
		}
		catch (...) {
			assert(false);
			outValue.StartFrame = 0;
			outValue.EndFrame = 0;
		}
	}

	virtual void Set(const TValue& value, rapidjson::Value& outValue) const override {
		const auto formatedRange = String::Format("%d:%d", value.StartFrame, value.EndFrame);
		outValue.SetString(formatedRange, String::Length(formatedRange));
	}
};