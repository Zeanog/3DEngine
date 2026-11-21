#include "FBXNodeParser_SkeletalJoint.h"

Bool FBXNodeParser_SkeletalJoint::Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Joint& outJoint) {
	if (!CanParse(node)) {
		return false;
	}

	auto parentNode = node->GetParent();
	auto&& parentNodeName = parentNode->GetNameOnly();

	Int32 parentIndex = -1;
	if (m_NodeNameToJointIndexMap.Contains(parentNodeName)) {
		parentIndex = m_NodeNameToJointIndexMap[parentNodeName];
	}

	outJoint.Parent(parentIndex);
	auto&& nodeName = node->GetNameOnly();
	outJoint.Index(m_NodeNameToJointIndexMap.Size());
	m_NodeNameToJointIndexMap.Add(nodeName, m_NodeNameToJointIndexMap.Size());
	if (prefixToRemove.Length() > 0) {
		verify(nodeName.FindAndReplace(prefixToRemove.CStr(), ""));
	}
	outJoint.Name(nodeName.Buffer());
	return true;
}

void FBXNodeParser_SkeletalJoint::Clear() {
	m_NodeNameToJointIndexMap.Clear();
}