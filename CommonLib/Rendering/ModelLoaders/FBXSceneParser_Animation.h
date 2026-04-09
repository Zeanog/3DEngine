#pragma once

#include "AFBXNodeParser.h"
#include "Rendering/Joint.h"
#include "Rendering/ModelLoaders/ModelDef.h"

class FBXSceneParser_Animation : public AFBXSceneParser<TYPELIST_3(List<Joint>, const ModelDef::TInFileAnimationMap, List<AnimationClip*>)> {
	INHERITED_CLASS_TYPEDEFS(FBXSceneParser_Animation, AFBXSceneParser)
	SINGLETON_DECLARATIONS(FBXSceneParser_Animation) {}

public:
	virtual Bool	Parse(fbxsdk::FbxScene* scene, List<Joint>& joints, const ModelDef::TInFileAnimationMap& inFileAnimMap, List<AnimationClip*>& outAnims) override;
	virtual	void	Clear() override {
	}
};