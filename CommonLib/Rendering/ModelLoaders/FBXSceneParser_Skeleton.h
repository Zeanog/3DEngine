#pragma once

#include "AFBXNodeParser.h"
#include "Rendering/Joint.h"
#include "Rendering/ModelLoaders/MeshLoader_FBX.h"

class FBXSceneParser_Skeleton : public AFBXSceneParser<TYPELIST_2(const StaticString, Neo::Skeleton)> {
	INHERITED_CLASS_TYPEDEFS(FBXSceneParser_Skeleton, AFBXSceneParser)
	SINGLETON_DECLARATIONS(FBXSceneParser_Skeleton) {
	}

protected:
	Map<fbxsdk::FbxString, Int32>		m_NodeNameToJointIndexMap;
	MeshLoader_FBX::TJointNameToNodeMap m_JointNameToNodeMap;

	fbxsdk::FbxNodeAttribute::EType		m_NodeType = fbxsdk::FbxNodeAttribute::eSkeleton;

protected:
	virtual Bool CanParse(fbxsdk::FbxNode* node) const {
		if (!node) {
			return false;
		}
		auto attr = node->GetNodeAttribute();
		if (!attr) {
			return false;
		}
		return attr->GetAttributeType() == m_NodeType;
	}

	Bool			Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Neo::Skeleton& outSkeleton);

public:
	virtual Bool	Parse(fbxsdk::FbxScene* scene, const StaticString& prefixToRemove, Neo::Skeleton& outSkeleton) override;
	virtual	void	Clear() override {
		m_NodeNameToJointIndexMap.Clear();
		m_JointNameToNodeMap.Clear();
	}

	fbxsdk::FbxNode* JointNode(const StaticString& jointName) const {
		if (!m_JointNameToNodeMap.Contains(jointName)) {
			return nullptr;
		}
		return m_JointNameToNodeMap[jointName];
	}
};