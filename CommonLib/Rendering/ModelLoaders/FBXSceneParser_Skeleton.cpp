#include "FBXSceneParser_Skeleton.h"
#include "FBXNodeParser_SkeletonalJoint.h"

Bool FBXSceneParser_Skeleton::Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Neo::Skeleton& outSkeleton) {
	if (!CanParse(node)) {
		return false;
	}

	Joint joint;
	auto parentNode = node->GetParent();
	auto&& parentNodeName = parentNode->GetNameOnly();

	Int32 parentIndex = -1;
	if (m_NodeNameToJointIndexMap.Contains(parentNodeName)) {
		parentIndex = m_NodeNameToJointIndexMap[parentNodeName];
	}

	joint.Parent(parentIndex);
	auto&& nodeName = node->GetNameOnly();
	if (prefixToRemove.Length() > 0) {
		verify(nodeName.FindAndReplace(prefixToRemove.CStr(), ""));
	}
	joint.Name(nodeName.Buffer());
	m_NodeNameToJointIndexMap.Add(nodeName, m_JointNameToNodeMap.Size());
	m_JointNameToNodeMap.Add(joint.Name(), node);

	outSkeleton.Joints().Add(joint);

	for (int ix = 0; ix < node->GetChildCount(); ++ix) {
		Parse(node->GetChild(ix), prefixToRemove, outSkeleton);
	}

	return true;
}

Bool FBXSceneParser_Skeleton::Parse(fbxsdk::FbxScene* scene, const StaticString& prefixToRemove, Neo::Skeleton& outSkeleton) {
	Clear();
	return Parse(scene->GetRootNode(), prefixToRemove, outSkeleton);
}