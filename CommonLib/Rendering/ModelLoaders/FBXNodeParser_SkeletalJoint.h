#pragma once

#include "AFBXNodeParser.h"
#include "System/Map.h"

class FBXNodeParser_SkeletalJoint : public AFBXNodeParser< TYPELIST_2(const StaticString, Joint) > {
	INHERITED_CLASS_TYPEDEFS(FBXNodeParser_SkeletalJoint, AFBXNodeParser)
	SINGLETON_DECLARATIONS(FBXNodeParser_SkeletalJoint) {
		m_NodeType = fbxsdk::FbxNodeAttribute::eSkeleton;
	}

public:
	using TNodeNameToJointIndexMap = Map<fbxsdk::FbxString, Int32>;

protected:
	TNodeNameToJointIndexMap		m_NodeNameToJointIndexMap;

public:
	virtual Bool	Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Joint& outJoint) override;
	virtual	void	Clear() override;
};