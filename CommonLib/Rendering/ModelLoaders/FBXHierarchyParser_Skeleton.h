#pragma once

#include "AFBXNodeParser.h"
#include "MeshLoader_FBX.h"

class FBXNodeParser_Joint : public AFBXNodeParser< TYPELIST_2(const StaticString, Joint) > {
	INHERITED_CLASS_TYPEDEFS(FBXNodeParser_Joint, AFBXNodeParser)
	SINGLETON_DECLARATIONS(FBXNodeParser_Joint) {
		m_NodeType = fbxsdk::FbxNodeAttribute::eSkeleton;
	}

protected:
	Map<fbxsdk::FbxString, Int32>		m_NodeNameToJointIndexMap;

public:
	virtual Bool	Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Joint& outJoint) override;
	virtual	void	Clear() override;
};

class FBXHierarchyParser_Skeleton : public AFBXSceneParser< TYPELIST_3(const StaticString, MeshLoader_FBX::TJointNameToNodeMap, Neo::Skeleton) > {
	INHERITED_CLASS_TYPEDEFS(FBXHierarchyParser_Skeleton, AFBXSceneParser)
	SINGLETON_DECLARATIONS(FBXHierarchyParser_Skeleton) {
	}

public:
	typedef Map<StaticString, fbxsdk::FbxNode*>		TJointNameToNodeMap;

public:
	virtual Bool	Parse(fbxsdk::FbxScene* scene, const StaticString& prefixToRemove, MeshLoader_FBX::TJointNameToNodeMap& jointNameToNodeMap, Neo::Skeleton& outSkeleton) override;
	Bool			Validate(fbxsdk::FbxScene* scene, const StaticString& prefixToRemove, const Neo::Skeleton& meshSkeleton);
	virtual	void	Clear() override;
};