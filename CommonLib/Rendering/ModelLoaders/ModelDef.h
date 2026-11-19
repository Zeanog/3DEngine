#pragma once

#include "System/Typedefs.h"
#include "System/String/StaticString.h"
#include "System/Map.h"
#include "Rendering/ModelLoaders/AnimationRange.h"

struct ModelDef {
	typedef StaticString							TFileName;
	typedef StaticString							TAnimName;
	typedef StaticString							TInFileAnimName;
	typedef Map<TAnimName, AnimationRange>			TFileAnimationMap;
	typedef Map<TInFileAnimName, TFileAnimationMap>	TInFileAnimationMap;
	typedef Map<TFileName, TInFileAnimationMap>		TAnimationMap;

	TFileName		Mesh;
	StaticString	SkeletalPrefix;
	TAnimationMap	AnimationFiles;
};

template<>
class Reflector<ModelDef> : public AReflectorJson {
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {
		REGISTER_MEMBER(TReflected, Mesh);
		REGISTER_MEMBER(TReflected, SkeletalPrefix);
		REGISTER_MEMBER(TReflected, AnimationFiles);
	}

public:
	typedef ModelDef	TReflected;
};

template<>
class ValueParser<ModelDef> : public AValueParser<ModelDef> {
	INHERITED_CLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(ValueParser) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		auto& meshVal = value["mesh"];
		Singleton<ValueParser<decltype(outValue.Mesh)>>::GetInstance()->Get(meshVal, outValue.Mesh);

		auto& skelVal = value["skeletalPrefix"];
		if (!skelVal.IsNull()) {
			Singleton<ValueParser<decltype(outValue.SkeletalPrefix)>>::GetInstance()->Get(skelVal, outValue.SkeletalPrefix);
		}

		auto& animVal = value["anims"];
		FOREACH_MEMBER(animFileIter, animVal) {
			const Char* animFile = animFileIter->name.GetString();
			assert(animFile && animFile[0]);
			assert(!outValue.AnimationFiles.Contains(animFile));

			outValue.AnimationFiles.Add(animFile, ModelDef::TInFileAnimationMap());
			auto& fileAnimMap = outValue.AnimationFiles[animFile];
			FOREACH_MEMBER(animInFileIter, animFileIter->value) {
				auto animInFileName = animInFileIter->name.GetString();
				assert(animInFileName && animInFileName[0]);
				assert(!fileAnimMap.Contains(animInFileName));

				fileAnimMap.Add(animInFileName, ModelDef::TFileAnimationMap());
				auto& animRangeMap = fileAnimMap[animInFileName];
				FOREACH_MEMBER(animRangeIter, animInFileIter->value) {
					auto animName = animRangeIter->name.GetString();
					assert(animName && animName[0]);
					AnimationRange animRange;
					Singleton<ValueParser<AnimationRange>>::GetInstance()->Get(animRangeIter->value, animRange);

					assert(!animRangeMap.Contains(animName));
					animRangeMap.Add(animName, animRange);
				}
			}
		}
	}

	virtual void Set(const TValue& value, rapidjson::Value& outValue) const override {
		outValue.SetObject();

		Singleton<ValueParser<decltype(value.Mesh)>>::GetInstance()->Set(value.Mesh, outValue["mesh"]);
		Singleton<ValueParser<decltype(value.SkeletalPrefix)>>::GetInstance()->Set(value.SkeletalPrefix, outValue["skeletalPrefix"]);
		//Singleton<ValueParser<decltype(value.Animations)>>::GetInstance()->Set(value.Animations, outValue["anims"]);
		rapidjson::Value& animVal = outValue["anims"];
		/*FOREACH_MEMBER(animIter, animVal) {
			const Char* animFile = animIter->name.GetString();
			FOREACH_MEMBER(animInfoIter, animIter->value) {
				value.Animations[animFile].Add({ animInfoIter->name.GetString(), 0.0f, 0.0f });
			}
		}*/
	}
};