#include "FBXNodeParser_SkeletonalJoint.h"

//StaticString JointPrefix("HIKCharacterNode1_");//TODO: Find a better place for this
//StaticString JointPrefix("mixamorig:");//TODO: Find a better place for this

Bool FBXNodeParser_SkeletonalJoint::Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Joint& outJoint) {
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
	if (prefixToRemove.Length() > 0) {
		verify(nodeName.FindAndReplace(prefixToRemove.CStr(), ""));
	}
	outJoint.Name(nodeName.Buffer());
	m_NodeNameToJointIndexMap.Add(nodeName, m_JointNameToNodeMap.Size());
	m_JointNameToNodeMap.Add(outJoint.Name(), node);
	return true;
}