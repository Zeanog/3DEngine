#pragma once

#include "AFBXNodeParser.h"
#include "Rendering/Joint.h"
#include "Rendering/ModelLoaders/ModelDef.h"
#include "Rendering/ModelLoaders/MeshLoader_FBX.h"

class FBXSceneParser_Animation : public AFBXSceneParser<TYPELIST_4(const MeshLoader_FBX::TNodeTypeToNodeListMap, const Neo::Skeleton, const ModelDef::TInFileAnimationMap, List<AnimationClip*>)> {
	INHERITED_CLASS_TYPEDEFS(FBXSceneParser_Animation, AFBXSceneParser)
	SINGLETON_DECLARATIONS(FBXSceneParser_Animation) {}

public:
	virtual Bool	Parse(fbxsdk::FbxScene* scene, const MeshLoader_FBX::TNodeTypeToNodeListMap& nodeMap, const Neo::Skeleton& skeleton, const ModelDef::TInFileAnimationMap& inFileAnimMap, List<AnimationClip*>& outAnims) override;
	virtual	void	Clear() override {
	}
};