#pragma once

#include "AFBXNodeParser.h"
#include "MeshLoader_FBX.h"

class FBXNodeParser_Skeleton : public AFBXNodeParser< TYPELIST_2(const StaticString, Joint) > {
	INHERITED_CLASS_TYPEDEFS(FBXNodeParser_Skeleton, AFBXNodeParser)
	SINGLETON_DECLARATIONS(FBXNodeParser_Skeleton) {
		m_NodeType = fbxsdk::FbxNodeAttribute::eSkeleton;
	}

protected:
	Map<fbxsdk::FbxString, Int32>		m_NodeNameToJointIndexMap;
	MeshLoader_FBX::TJointNameToNodeMap m_JointNameToNodeMap;

public:
	virtual Bool	Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Joint& outJoint) override;
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