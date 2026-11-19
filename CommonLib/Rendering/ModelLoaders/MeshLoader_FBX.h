#pragma once

#include "Rendering/ModelLoaders/IMeshLoader.h"
#include "Rendering/ModelLoaders/AFBXNodeParser.h"

class MeshLoader_FBX : public IMeshLoader {
	INHERITED_CLASS_TYPEDEFS(MeshLoader_FBX, IMeshLoader)

public:
	typedef Map<StaticString, fbxsdk::FbxNode*>					TJointNameToNodeMap;
	typedef Map<Joint*, fbxsdk::FbxNode*>						TJointToNodeMap;
	typedef List<fbxsdk::FbxNode*>								TNodeList;
	typedef Map<fbxsdk::FbxNodeAttribute::EType, TNodeList>		TNodeTypeToNodeListMap;

protected:
	static FbxManager*	g_SdkManager;
	FbxScene*			m_Scene = nullptr;

	//TJointNameToNodeMap	m_JointNameToNodeMap;

protected:
	Bool		ExtractNodes(fbxsdk::FbxNode* node, TNodeTypeToNodeListMap& outNodeMap);

	Bool		LoadTextures(FbxScene* scene);
	Bool		ParseMesh(const TNodeTypeToNodeListMap& nodeMap, Neo::Mesh& outMesh);
	Bool		ParseSkeleton(const TNodeTypeToNodeListMap& nodeMap, const StaticString& jointPrefix, Neo::Skeleton& outSkeleton);
	Bool		ParseAnimations(const TNodeTypeToNodeListMap& nodeMap, const StaticString& jointPrefix, const ModelDef::TInFileAnimationMap& inFileAnimMap, Neo::Mesh& outMesh);

	Bool		ValidateSkeleton(const TNodeTypeToNodeListMap& nodeMap, const StaticString& jointPrefix, const Neo::Skeleton& skeleton);

	virtual	Bool LoadScene(const StaticString& filePath) override;

public:
	static void	CreateGlobals();
	static void	DestroyGlobals();

	MeshLoader_FBX();
	
	virtual Bool Load(const ModelDef& def, Neo::Mesh& outMesh) override;
	virtual void Clear() override;
};