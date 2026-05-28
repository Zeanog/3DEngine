#include "FBXHierarchyParser_Skeleton.h"

//StaticString JointPrefix("HIKCharacterNode1_");//TODO: Find a better place for this
//StaticString JointPrefix("mixamorig:");//TODO: Find a better place for this

Bool FBXNodeParser_Joint::Parse(fbxsdk::FbxNode* node, const StaticString& prefixToRemove, Joint& outJoint) {
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
	m_NodeNameToJointIndexMap.Add(nodeName, m_NodeNameToJointIndexMap.Size());
	return true;
}

void FBXNodeParser_Joint::Clear() {
	m_NodeNameToJointIndexMap.Clear();	
}

Bool FBXHierarchyParser_Skeleton::Parse(fbxsdk::FbxScene* scene, const StaticString& prefixToRemove, FBXHierarchyParser_Skeleton::TJointNameToNodeMap& jointNameToNodeMap, Neo::Skeleton& outSkeleton) {
	Clear();
	VisitNode(scene->GetRootNode(), [this](fbxsdk::FbxNode* node, const StaticString& prefixToRemove, FBXHierarchyParser_Skeleton::TJointNameToNodeMap& jointNameToNodeMap, Neo::Skeleton& outSkeleton) {
		Joint joint;
		if (Singleton<FBXNodeParser_Joint>::GetInstance()->Parse(node, prefixToRemove, joint)) {
			outSkeleton.Joints().AddUnique(joint);//TODO: This is a linear search
			jointNameToNodeMap.Add(joint.Name(), node);
			return true;
		}
		return false;
	}, prefixToRemove, jointNameToNodeMap, outSkeleton);
	return true;
}

Bool FBXHierarchyParser_Skeleton::Validate(fbxsdk::FbxScene* scene, const StaticString& prefixToRemove, const Neo::Skeleton& meshSkeleton) {
	Clear();
	FBXHierarchyParser_Skeleton::TJointNameToNodeMap jointNameToNodeMap;
	return VisitNode(scene->GetRootNode(), [this](fbxsdk::FbxNode* node, const StaticString& prefixToRemove, FBXHierarchyParser_Skeleton::TJointNameToNodeMap& nodes, const Neo::Skeleton& meshSkeleton) {
		Joint joint;
		if (Singleton<FBXNodeParser_Joint>::GetInstance()->Parse(node, prefixToRemove, joint)) {
			return meshSkeleton.Contains(joint.Name());
		}
		return true;
	}, prefixToRemove, jointNameToNodeMap, meshSkeleton);
}

void FBXHierarchyParser_Skeleton::Clear() {
}